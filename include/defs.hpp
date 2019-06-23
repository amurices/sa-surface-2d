#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <exception>
#include <lemon/list_graph.h>

#ifdef __APPLE__
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#endif
using namespace lemon;

const double TOLERANCE = 0.00000001;
const double PI = 3.14159265358979323846;

/*** TODO list:
    
    1. DONE (in Salvador) -- Have a memory-safe graph representation of surfaces. 
        1.1 DONE (in Salvador) -- Implement operator overloads: =, == for simple surface (_2DSurface
        1.2 DONE (in Salvador) -- Implement operator overloads or copying functions for thick surface (ThickSurface)

    2. ALMOST DONE -- Build a modular, safe, and easy to use API for graphically representing surfaces.
        2.1 DONE (in Salvador) -- Have functions for drawing individual nodes.
        2.2 DONE (in Salvador) -- Have function for drawing entire simple surfaces.
        2.3 DONE (in Salvador) -- Encapsulate everything in a Renderer class.
        2.4 NOT DONE -- Have functions for drawing fonts
        2.5 NOT DONE -- Implement a generic type RenderObject that includes metadata for different structures that need to be drawn.
        2.6 NOT DONE -- Implement basic I/O (to pause and trigger slow-mo mode)

    3. Have an implementation of Simulated Annealing.
        3.1 NOT DONE -- Implement a probability function.
        3.2 PARTIALLY DONE; FIND BETTER MATHEMATICAL ALTERNATIVES -- Implement a neighbor function. MOVE ON; COME BACK TO IT. DO THE REST OF SA.
        3.3 NOT DONE -- Implement a temperature function.
        3.4 NOT DONE -- Implement the most wide-encompassing parametrization:
            - Radius
            - Thickness
            - Scale
            - Smoothness
            - Difference between A0 and An multiplier
            - Difference between A0 and An power
            - White matter multiplier
            - White matter power
            - Multiple node addition probability
            - Maximum compression/expansion
            - Temperature
            - Compression factor
            - MAYBE outer/inner shapes that cannot intersect the surface?
        3.5 NOT DONE -- Implement a model of tension at individual points. Perhaps an additional vector in ThickSurface that tracks
                        how much that point has been compressed or stretched.

    4. Have a system to dynamically manipulate the simulation mid-execution.
        4.1 NOT DONE -- Implement node addition/removal. This will be especially difficult due to the way smoothness is implemented.
        4.2 NOT DONE -- Implement simulation reset.
        4.3 NOT DONE -- Implement multiple (parametrizable) simulation runs (for example, running with 1000 different parametrizations)
        4.4 NOT DONE -- Implement intelligent simulation output. Either output data to files, or showcase them on the screen.

    5. Finally, implement a GUI that handles whatever I/O (4) implements.

    6. Maybe find a solution to the fact that findDirectionVector produces intersections way too often. Collision resolution algorithms, perhaps?
***/

/* Some type definitions.
 *
 * Types utilized have operators overloaded when relevant, and when defined by
 * our program, they come with the suffix "_t".
 */
struct triple_t
{
    double _1;
    double _2;
    double _3;

    triple_t(double r, double g, double b)
    {
        _1 = r;
        _2 = g;
        _3 = b;
    }

    triple_t operator*(const double &a) const
    {
        return triple_t(_1 * a, _2 * a, _3 * a);
    }

    void operator*=(const double &a)
    {
        _1 *= a;
        _2 *= a;
        _3 *= a;
    }

    triple_t operator+(const double &a) const
    {
        return triple_t(_1 + a, _2 + a, _3 + a);
    }

    void operator+=(const double &a)
    {
        _1 += a;
        _2 += a;
        _3 += a;
    }

    void operator+=(const triple_t &a)
    {
        _1 += a._1;
        _2 += a._2;
        _3 += a._3;
    }
};

struct point_t
{
    double x;
    double y;

    point_t(double xn, double yn)
    {
        x = xn;
        y = yn;
    }
    point_t()
    {
    }
    bool operator<(const point_t &p) const
    {
        return x < p.x || (x == p.x && y < p.y);
    }

    bool operator==(const point_t &p) const
    {
        return x == p.x && y == p.y;
    }

    point_t operator+(const point_t &p) const
    {
        return point_t(p.x + x, p.y + y);
    }
    void operator=(const point_t &p)
    {
        x = p.x;
        y = p.y;
    }

    void operator+=(const point_t &p)
    {
        x += p.x;
        y += p.y;
    }

    point_t operator-(const point_t &p) const
    {
        return point_t(x - p.x, y - p.y);
    }
    void operator-=(const point_t &p)
    {
        x -= p.x;
        y -= p.y;
    }

    point_t operator*(const double &a) const
    {
        return point_t(x * a, y * a);
    }

    void operator*=(const double &a)
    {
        x *= a;
        y *= a;
    }

};
// CGAL typedefs
/*
typedef CGAL::Quotient<CGAL::MP_Float>      NT;
typedef CGAL::Cartesian<NT>                 Kernel;
typedef Kernel::Point_2                     point_cg;
typedef CGAL::Arr_segment_traits_2<Kernel>  traits_cg;
typedef traits_cg::Curve_2                  lines_cg;
*/
struct line_t
{
    point_t p1;
    point_t p2;

    line_t(double x1n, double y1n, double x2n, double y2n)
    {
        p1.x = x1n;
        p2.x = x2n;
        p1.y = y1n;
        p2.y = y2n;
    }

    line_t(point_t p1n, point_t p2n)
    {
        p1 = p1n;
        p2 = p2n;
    }

    line_t()
    {
    }
};

struct InitSaParams
{
    // Surface params
    double radius;
    double thickness;
    // Optimizer params
    double scale;
    double a0;
    int smooth;
    double diffMul;
    double diffPow;
    double areaPow;
    double areaMul;
    double multiProb;
    double tempProb;
    double forceOffsetRange;
    double compression;

    InitSaParams(double scale, double a0, 
                 int smooth, double diffMul,
                 double diffPow, double areaPow,
                 double areaMul, double multiProb,
                 double tempProb, double forceOffsetRange,
                 double compression) : 
                scale{scale}, a0{a0},
                smooth{smooth}, diffMul{diffMul}, 
                diffPow{diffPow}, areaPow{areaPow}, 
                areaMul{areaMul}, multiProb{multiProb}, 
                tempProb{tempProb}, forceOffsetRange{forceOffsetRange},
                compression{compression}
                {
                     printf("Params:\nscale: %.4f\na0: %.4f\nsmooth: %d\ndiffMul: %.4f\ndiffPow: %.4f\nareaPow: %.4f\nareaMul: %.4f\nmultiProb: %.4f\ntempProb: %.4f\nforceOffsetRange: %.4f\ncompression: %.4f\n",scale,a0,smooth,diffMul,diffPow,areaPow,areaMul,multiProb,tempProb,forceOffsetRange,compression);
                }
};

typedef ListDigraph::NodeMap<point_t> Coords_t;				  // _t for type
typedef std::vector<std::set<ListDigraph::Arc>> Partitions_t; // _t for type
typedef ListDigraph::Node SNode;							  // S for surface

/* 
 * Some pretty-printing operator overloads.
 */
inline std::ostream &operator<<(std::ostream &os, point_t const &p)
{
    return os << "(" << p.x << ", " << p.y << ")";
}
inline std::ostream &operator<<(std::ostream &os, line_t const &p)
{
    return os << "(" << p.p1.x << ", " << p.p1.y << ") -> "
              << "(" << p.p2.x << ", " << p.p2.y << ")";
}

/* ListDigraph manipulation types. _t suffix means type. */

typedef ListDigraph::NodeMap<point_t> Coords_t;
typedef ListDigraph::Node SNode; // S for surface

typedef struct ct
{
    SNode node;
    point_t change;
    ListDigraph *graph;

    ct(SNode node, point_t change, ListDigraph *graph)
    {
        this->node = node;
        this->change = change;
        this->graph = graph;
    }

    void operator=(const struct ct &otherChange)
    {
        this->node = otherChange.node;
        this->change = otherChange.change;
        this->graph = otherChange.graph;
    }

    bool operator<(const struct ct &otherChange) const
    {
        return (node < otherChange.node |
                change < otherChange.change |
                graph < otherChange.graph);
    }
} NodeChange_t;

typedef struct tct
{
    int nodeIndex;
    double change;

    tct(int nodeIndex, double change)
    {
        this->nodeIndex = nodeIndex;
        this->change = change;
    }

    void operator=(const struct tct &otherChange)
    {
        nodeIndex = otherChange.nodeIndex;
        change = otherChange.change;
    }

    bool operator<(const struct tct &otherChange) const
    {
        return (nodeIndex < otherChange.nodeIndex |
                change < otherChange.change);
    }
} ThicknessChange_t;
