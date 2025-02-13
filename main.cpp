#include "OctreeMesh.hpp"
#include <CGAL/IO/OFF.h>
#include <CGAL/IO/STL.h>
#include <fstream>
#include <iostream>
#include <string>

Mesh import_stl(const std::string &filename)
{
    Mesh mesh;
    std::ifstream input(filename, std::ios::binary);
    if (!input || !CGAL::IO::read_STL(input, mesh))
    {
        std::cerr << "Error: cannot read file " << filename << std::endl;
        return Mesh();
    }
    return mesh;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " input.stl" << std::endl;
        return 1;
    }

    // Import STL file
    std::cout << "Reading STL file: " << argv[1] << std::endl;
    Mesh input_mesh = import_stl(argv[1]);

    if (input_mesh.is_empty())
    {
        std::cerr << "Failed to load STL file" << std::endl;
        return 1;
    }

    std::cout << "STL file loaded successfully with "
              << input_mesh.number_of_vertices() << " vertices and "
              << input_mesh.number_of_faces() << " faces." << std::endl;

    // Calculate bounding box of the input mesh
    CGAL::Bbox_3 bbox = CGAL::bbox_3(input_mesh.points().begin(),
                                     input_mesh.points().end());

    // Create octree mesh centered at bounding box center with size that encompasses the model
    double center_x = (bbox.xmin() + bbox.xmax()) / 2.0;
    double center_y = (bbox.ymin() + bbox.ymax()) / 2.0;
    double center_z = (bbox.zmin() + bbox.zmax()) / 2.0;

    double size_x = bbox.xmax() - bbox.xmin();
    double size_y = bbox.ymax() - bbox.ymin();
    double size_z = bbox.zmax() - bbox.zmin();
    double max_size = std::max({size_x, size_y, size_z}) * 1.1; // Add 10% margin

    OctreeMesh octree(Point(center_x, center_y, center_z), max_size);

    // Define refinement criterion (refine nodes that intersect with the input mesh)
    auto should_refine = [&](const OctreeNode &node)
    {
        // Simple refinement based on size for now
        return node.size > max_size / 16.0;
    };

    // Refine the mesh
    std::cout << "Refining octree mesh..." << std::endl;
    octree.refine(should_refine);

    // Generate the mesh
    Mesh output_mesh;
    octree.generate_mesh(output_mesh);

    // Save the mesh to a file
    std::string output_filename = "output.off";
    std::cout << "Writing mesh to " << output_filename << "..." << std::endl;
    std::ofstream out(output_filename);
    CGAL::IO::write_OFF(out, output_mesh);
    std::cout << "Done! The output mesh has " << output_mesh.number_of_vertices()
              << " vertices and " << output_mesh.number_of_faces() << " faces." << std::endl;

    return 0;
}