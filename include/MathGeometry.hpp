#pragma once

#include <vector>
#include <string>

/* MathGeometry namespace containing auxiliary geometric functions.
 *
 * The MathGeometry namespace handles the spatial mathematics of the simulation.
 * It is made to encapsulate functions and enumerations that correspond to nothing but abstract
 * mathematical equations that describe behavior of vectors and discrete surfaces.
 */

namespace MathGeometry
{
    const double PI = 3.14159265358979323846;
    const double TOLERANCE = 0.00000001;
    enum direction_t
{
    ORTHOGONAL,
    MEDIAN,
    MEDIAN_ANGLE
};

    struct point_t
    {
        double x;
        double y;

        // Trivial constructors
        point_t() = default;
        point_t& operator=(const point_t &p) = default;
        point_t(double xn, double yn){ x = xn; y = yn; }
        // Point (2D vectors) operations
        bool operator<(const point_t &p)    const { return x < p.x || (x == p.x && y < p.y); }
        bool operator==(const point_t &p)   const { return x == p.x && y == p.y; }
        point_t operator+(const point_t &p) const { return point_t(x + p.x, y + p.y); }
        point_t operator-(const point_t &p) const { return point_t(x - p.x, y - p.y); }
        point_t operator*(const double &a)  const { return point_t(x * a, y * a); }
        void operator+=(const point_t &p)   { x += p.x; y += p.y; }
        void operator-=(const point_t &p){ x -= p.x; y -= p.y; }
        void operator*=(const double &a) { x *= a; y *= a; }
    };

    inline bool ltTolerance(double a, double b, double tol){ return (a < (b - tol)); }
    inline bool gtTolerance(double a, double b, double tol){ return (a > (b + tol)); }
    inline bool eqTolerance(double a, double b, double tol){ return (a > b - tol) && (a < b + tol); }
    inline double absol(double x) {return x < 0 ? -x : x;}
    double findNorm(const point_t &a);
    double findNorm2d(double x, double y);
    point_t findDirectionVector(const point_t &a, const point_t &b, const point_t &c, direction_t Type = MEDIAN_ANGLE);
    int findPartitionNumber(point_t p, int numHorzPartitions, int numVertPartitions);
    double sineSmooth(double u, double c);
    double linearSmooth(double u, double c);
    double inverseSmooth(double u, double c);

std::vector<point_t> surfaceIntersections(const std::vector<std::pair<point_t, point_t>> &lines);
} // namespace MathGeometry