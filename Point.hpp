#ifndef POINT_HPP
#define POINT_HPP

#include <CGAL/Simple_cartesian.h>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;

struct Point
{
    double x, y, z;
    Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    operator Point_3() const
    {
        return Point_3(x, y, z);
    }

    static Point from_cgal_point(const Point_3 &p)
    {
        return Point(p.x(), p.y(), p.z());
    }
};

#endif // POINT_HPP