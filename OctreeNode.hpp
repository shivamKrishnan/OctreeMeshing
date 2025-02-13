#ifndef OCTREE_NODE_HPP
#define OCTREE_NODE_HPP

#include "Point.hpp"
#include <array>
#include <memory>
#include <CGAL/Bbox_3.h>

class OctreeNode
{
public:
    static constexpr int NUM_CHILDREN = 8;

    Point center;
    double size;
    double value;
    bool isLeaf;
    std::array<std::shared_ptr<OctreeNode>, NUM_CHILDREN> children;
    CGAL::Bbox_3 bbox;

    OctreeNode(const Point &c, double s);
    void split();
    void merge();
    CGAL::Bbox_3 compute_bbox() const;
    bool contains_point(const Point_3 &point) const;

private:
    void init_bbox();
};

#endif // OCTREE_NODE_HPP