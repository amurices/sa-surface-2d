//
//  defs.h
//  Surface BRKGA
//
//  Created by André Muricy on 2/1/17.
//
//

#ifndef defs_h
#define defs_h

#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <cmath>
#include <utility>
#include <vector>
#include <map>

#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Sweep_line_2_algorithms.h>

#include <lemon/list_graph.h>

using namespace lemon;

const double TOLERANCE  = 0.00000001;
const double PI         = 3.14159265358979323846;

typedef struct triple_t{
    double _1;
    double _2;
    double _3;
    
    triple_t (double r, double g, double b){ _1 = r; _2 = g; _3 = b; }
} triple_t;

typedef struct point_t{
    double x;
    double y;
    point_t (double xn, double yn)
    {
        x = xn; y = yn;
    }
    point_t ()
    {
        
    }
    bool operator <(const point_t &p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
    
    bool operator ==(const point_t &p) const {
        return x == p.x && y == p.y;
    }
    
    point_t operator +(const point_t &p) const {
        return point_t(p.x + x, p.y + y);
    }
    
    point_t operator -(const point_t &p) const {
        return point_t(x - p.x, y - p.y);
    }
    
    point_t operator *(const double &a) const {
        return point_t(x*a, y*a);
    }
    
    void operator *= (const double &a){
        x *= a;
        y *= a;
    }
    
}point_t;

typedef struct lines_t{
    point_t p1;
    point_t p2;
    
    lines_t(double x1n, double y1n, double x2n, double y2n)
    {
        p1.x = x1n; p2.x = x2n; p1.y = y1n; p2.y = y2n;
    }
    
    lines_t(point_t p1n, point_t p2n)
    {
        p1 = p1n;
        p2 = p2n;
    }
    
    lines_t()
    {
        
    }
}lines_t;

// CGAL typedefs
typedef CGAL::Quotient<CGAL::MP_Float>      NT;
typedef CGAL::Cartesian<NT>                 Kernel;
typedef Kernel::Point_2                     point_cg;
typedef CGAL::Arr_segment_traits_2<Kernel>  traits_cg;
typedef traits_cg::Curve_2                  lines_cg;



inline std::ostream& operator<<(std::ostream&os, point_t const &p){
    return os << "(" << p.x << ", " << p.y << ")";
}
inline std::ostream& operator<<(std::ostream&os, lines_t const &p){
    return os << "(" << p.p1.x << ", " << p.p1.y << ") -> " << "(" << p.p2.x << ", " << p.p2.y << ")";
}

typedef ListDigraph::NodeMap<point_t> Coords_t; // _t for type
typedef ListDigraph::Node SNode;                // S for surface
typedef struct SurfaceData_t{                   // _t for type
    Coords_t    *coords;
    ListDigraph graph;
    
    std::map<int, int> correspondence;
    
    SurfaceData_t(){
        coords = new Coords_t(graph);
        // Nota yurosal: Fazer destruidor ou consertar esta caralha e fazer deixar de ser apontador
        nEdges = 0;
        nNodes = 0;
    }
    
    int nEdges;
    int nNodes;
} SurfaceData_t;

typedef ListDigraph::NodeMap<double> Thicks_t;
typedef struct ThickSurface_t{
    Thicks_t *thicknesses;
    // ^ TIRAR DE APONTADOR
    
    SurfaceData_t inner;
    SurfaceData_t outer;
    
    SurfaceData_t bridges;
    
    int *nNodes;
    double scale = 1.0;
} ThickSurface_t;

#endif /* defs_h */
