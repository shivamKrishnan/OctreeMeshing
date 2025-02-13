#ifndef OCTREE_MESH_HPP
#define OCTREE_MESH_HPP

#include "OctreeNode.hpp"
#include <CGAL/Surface_mesh.h>
#include <functional>
#include <vector>

typedef CGAL::Surface_mesh<Point_3> Mesh;

class OctreeMesh
{
public:
    OctreeMesh(const Point &center, double size);
    void refine(const std::function<bool(const OctreeNode &)> &should_refine);
    void generate_mesh(Mesh &mesh) const;
    void set_value(const Point &point, double value);

private:
    std::shared_ptr<OctreeNode> root;
    void refine_recursive(std::shared_ptr<OctreeNode> node,
                          const std::function<bool(const OctreeNode &)> &should_refine);
    void set_value_recursive(std::shared_ptr<OctreeNode> node,
                             const Point &point, double value);
    void generate_mesh_recursive(std::shared_ptr<OctreeNode> node,
                                 Mesh &mesh) const;
};

#endif // OCTREE_MESH_HPP