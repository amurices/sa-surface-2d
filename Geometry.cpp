//
//  Geometry.cpp
//  Surface
//
//  Created by André Muricy on 5/3/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//


#include "Geometry.hpp"

static struct point_t* centroid;

// Return polar angle of p with respect to origin o
double to_angle(const struct point_t *p, const struct point_t *o) {
    return atan2(p->y - o->y, p->x - o->x);
}

void find_centroid(struct point_t *c, struct point_t *pts, int n_pts) {
    double x = 0, y = 0;
    for (int i = 0; i < n_pts; i++) {
        x += pts[i].x;
        y += pts[i].y;
    }
    c->x = x / n_pts;
    c->y = y / n_pts;
}


int by_polar_angle(const void *va, const void *vb) {
    double theta_a = to_angle((point_t*)va, centroid);
    double theta_b = to_angle((point_t*)vb, centroid);
    return theta_a < theta_b ? -1 : theta_a > theta_b ? 1 : 0;
}

void sort_by_polar_angle(struct point_t *pts, int n_pts) {
    find_centroid(centroid, pts, n_pts);
    qsort(pts, n_pts, sizeof pts[0], by_polar_angle);
}

double find_norm(const point_t &a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

point_t     find_direction_vector(const point_t &a, const point_t &b)
{
    // Vd = P(n+1) - P(n-1)
    point_t directionOffset = a - b;
    
    // variável auxiliar invX
    double invX = directionOffset.x;
    
    // Vd = (-Vd.y ,Vd.x)
    directionOffset.x = - directionOffset.y;
    directionOffset.y = invX;
    
    // Vd = Vd / Norm(Vd) (vetor retornado é unitário)
    double offsetNorm = find_norm(directionOffset);
    directionOffset.x /= offsetNorm;
    directionOffset.y /= offsetNorm;
    
    return directionOffset;
}



// ---------------------------------------------
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the point_ts are collinear.
double cross(const point_t &O, const point_t &A, const point_t &B)
{
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Returns a list of point_ts on the convex hull in counter-clockwise order.
// Note: the last point_t in the returned list is the same as the first one.
std::vector<point_t> convex_hull(std::vector<point_t> P)
{
    int n = P.size(), k = 0;
    std::vector<point_t> H(2*n);
    
    // Sort point_ts lexicographically
    sort(P.begin(), P.end());
    
    // Build lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
        H[k++] = P[i];
    }
    
    // Build upper hull
    for (int i = n-2, t = k+1; i >= 0; i--) {
        while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
        H[k++] = P[i];
    }
    
    H.resize(k-1);
    return H;
}

/* Pseudocode of the concave phase
    Data: list A with edges for the convex hull
    Result: list B with edges for a concave hull
 
    Sort list A after the length of the edges;
 
    while list A is not empty
        Select the longest edge e from list A;
        Remove edge e from list A;
        Calculate local maximum distance d for edges;
        if length of edge is larger than distance d
            Find the point_t p with the smallest maximum angle a;
            if angle a is small enough and point_t p is not on the boundary
                Create edges e2 and e3 between point_t p and endpoint_ts of edge e;
                if edge e2 and e3 don't intersect any other edge
                    Add edge e2 and e3 to list A;
                    Set point_t p to be on the boundary;
        if edge e2 and e3 was not added to list A
            Add edge e to list B;
 ---- //Fonte: http://www.it.uu.se/edu/course/homepage/projektTDB/ht13/project10/Project-10-report.pdf */
// ---------------------------------------------------------------

