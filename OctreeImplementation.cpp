#include "OctreeMesh.hpp"
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>

OctreeNode::OctreeNode(const Point &c, double s)
    : center(c), size(s), value(0.0), isLeaf(true)
{
    init_bbox();
}

void OctreeNode::init_bbox()
{
    double half_size = size / 2.0;
    bbox = CGAL::Bbox_3(
        center.x - half_size, center.y - half_size, center.z - half_size,
        center.x + half_size, center.y + half_size, center.z + half_size);
}

void OctreeNode::split()
{
    if (!isLeaf)
        return;

    isLeaf = false;
    double new_size = size / 2.0;
    double offset = new_size / 2.0;

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                Point new_center(
                    center.x + (k == 0 ? -offset : offset),
                    center.y + (j == 0 ? -offset : offset),
                    center.z + (i == 0 ? -offset : offset));
                int index = i * 4 + j * 2 + k;
                children[index] = std::make_shared<OctreeNode>(new_center, new_size);
                children[index]->value = value;
            }
        }
    }
}

bool OctreeNode::contains_point(const Point_3 &point) const
{
    return (point.x() >= bbox.xmin() && point.x() <= bbox.xmax() &&
            point.y() >= bbox.ymin() && point.y() <= bbox.ymax() &&
            point.z() >= bbox.zmin() && point.z() <= bbox.zmax());
}

OctreeMesh::OctreeMesh(const Point &center, double size)
    : root(std::make_shared<OctreeNode>(center, size)) {}

void OctreeMesh::refine(const std::function<bool(const OctreeNode &)> &should_refine)
{
    refine_recursive(root, should_refine);
}

void OctreeMesh::refine_recursive(std::shared_ptr<OctreeNode> node,
                                  const std::function<bool(const OctreeNode &)> &should_refine)
{
    if (!node)
        return;

    if (node->isLeaf && should_refine(*node))
    {
        node->split();
    }

    if (!node->isLeaf)
    {
        for (auto &child : node->children)
        {
            refine_recursive(child, should_refine);
        }
    }
}

void OctreeMesh::set_value(const Point &point, double value)
{
    set_value_recursive(root, point, value);
}

void OctreeMesh::set_value_recursive(std::shared_ptr<OctreeNode> node,
                                     const Point &point, double value)
{
    if (!node || !node->contains_point(Point_3(point.x, point.y, point.z)))
        return;

    if (node->isLeaf)
    {
        node->value = value;
    }
    else
    {
        for (auto &child : node->children)
        {
            set_value_recursive(child, point, value);
        }
    }
}

void OctreeMesh::generate_mesh(Mesh &mesh) const
{
    mesh.clear();
    generate_mesh_recursive(root, mesh);
    CGAL::Polygon_mesh_processing::triangulate_faces(mesh);
}

void OctreeMesh::generate_mesh_recursive(std::shared_ptr<OctreeNode> node,
                                         Mesh &mesh) const
{
    if (!node)
        return;

    if (node->isLeaf)
    {
        double half_size = node->size / 2.0;
        std::vector<Mesh::Vertex_index> vertices;

        // Create vertices for cube
        for (int i = 0; i < 8; ++i)
        {
            Point_3 p(
                node->center.x + (i & 1 ? half_size : -half_size),
                node->center.y + (i & 2 ? half_size : -half_size),
                node->center.z + (i & 4 ? half_size : -half_size));
            vertices.push_back(mesh.add_vertex(p));
        }

        // Add faces
        std::vector<Mesh::Vertex_index> face;
        face.reserve(4);

        // Front face
        face = {vertices[0], vertices[1], vertices[3], vertices[2]};
        mesh.add_face(face);

        // Back face
        face = {vertices[4], vertices[6], vertices[7], vertices[5]};
        mesh.add_face(face);

        // Top face
        face = {vertices[2], vertices[3], vertices[7], vertices[6]};
        mesh.add_face(face);

        // Bottom face
        face = {vertices[0], vertices[4], vertices[5], vertices[1]};
        mesh.add_face(face);

        // Left face
        face = {vertices[0], vertices[2], vertices[6], vertices[4]};
        mesh.add_face(face);

        // Right face
        face = {vertices[1], vertices[5], vertices[7], vertices[3]};
        mesh.add_face(face);
    }
    else
    {
        for (const auto &child : node->children)
        {
            generate_mesh_recursive(child, mesh);
        }
    }
}