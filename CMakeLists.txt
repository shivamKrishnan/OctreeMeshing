cmake_minimum_required(VERSION 3.5)
project(OctreeMesh)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CGAL REQUIRED)

add_executable(octree_mesh
    main.cpp
    OctreeImplementation.cpp
)

target_link_libraries(octree_mesh CGAL::CGAL)