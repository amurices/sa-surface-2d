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

point_t find_direction_vector(const point_t &a, const point_t &b, const point_t &c, direction_t Type)
{
    point_t directionOffset;
    if (Type == ORTHOGONAL)
    {
        // Vd = P(n+1) - P(n-1)
        directionOffset = a - b;

        // variável auxiliar invX
        double invX = directionOffset.x;

        // Vd = (-Vd.y ,Vd.x)
        directionOffset.x = - directionOffset.y;
        directionOffset.y = invX;

        // Vd = Vd / Norm(Vd) (vetor retornado é unitário)
        double offsetNorm = find_norm(directionOffset);
        directionOffset.x /= offsetNorm;
        directionOffset.y /= offsetNorm;

    }
    
    else if (Type == MEDIAN)
    {
        // Vd = P(n+1) - P(n-1)
        point_t dO = a - b;
        
        // variável auxiliar invX
        double invX = dO.x;
        
        // Vd = (-Vd.y ,Vd.x)
        dO.x = - dO.y;
        dO.y = invX;
        
        // Vd = Vd / Norm(Vd) (vetor retornado é unitário)
        double oN = find_norm(dO);
        dO.x /= oN;
        dO.y /= oN;
        
        
        // Shift a and b to the origin
        point_t aShift = a - c;
        point_t bShift = b - c;
        
        // Add both to get direction
        directionOffset = aShift + bShift;
        
        // Divide by norm
        double offsetNorm = find_norm(directionOffset);
        directionOffset.x /= offsetNorm;
        directionOffset.y /= offsetNorm;
        
        if (find_norm(dO - directionOffset) > find_norm(dO + directionOffset))
            directionOffset = directionOffset * (-1);
        
    }
    
    
    else if (Type == MEDIAN_ANGLE)
    {
        // Vd = P(n+1) - P(n-1)
        point_t dO = a - b;
        
        // variável auxiliar invX
        double invX = dO.x;
        
        // Vd = (-Vd.y ,Vd.x)
        dO.x = - dO.y;
        dO.y = invX;
        
        // Vd = Vd / Norm(Vd) (vetor retornado é unitário)
        double oN = find_norm(dO);
        dO.x /= oN;
        dO.y /= oN;
        
        
        // Shift a and b to the origin
        point_t aShift = a - c;
        point_t bShift = b - c;
        
        // Normalize both
        double aNorm = find_norm(aShift);
        double bNorm = find_norm(bShift);
        aShift = aShift * (1 / aNorm);
        bShift = bShift * (1 / bNorm);
        
        // Add both to get direction
        directionOffset = aShift + bShift;
        directionOffset = directionOffset * 0.5;
        
        // Divide by norm
        double offsetNorm = find_norm(directionOffset);
        directionOffset.x /= offsetNorm;
        directionOffset.y /= offsetNorm;
        
        if (find_norm(dO - directionOffset) > find_norm(dO + directionOffset))
            directionOffset = directionOffset * (-1);
        
    }

    return directionOffset;
}

double distance_point_to_line_segment(point_t p, lines_t l){
    
    double  A = p.x - l.p1.x;
    double  B = p.y - l.p1.y;
    double  C = l.p2.x - l.p1.x;
    double  D = l.p2.y - l.p1.y;
    
    double  dot = A * C + B * D;
    double  len_sq = C * C + D * D;
    double  param = -1;
    if (len_sq != 0) //in case of 0 length line
        param = dot / len_sq;
    
    double  xx, yy;
    
    if (param < 0) {
        xx = l.p1.x;
        yy = l.p1.y;
    }
    else if (param > 1) {
        xx = l.p2.x;
        yy = l.p2.y;
    }
    else {
        xx = l.p1.x + param * C;
        yy = l.p1.y + param * D;
    }
    
    double  dx = p.x - xx;
    double  dy = p.y - yy;
    return sqrt(dx * dx + dy * dy);
}

// ---------------------------------------------
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the point_ts are collinear.
double cross(const point_t &A, const point_t &B, const point_t &O = point_t(0, 0))
{
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

double _2Dcross(const point_t &v, const point_t &w)
{
    return v.x * w.y - v.y * w.x;
}

// Return mutually exclusive subset of points in a set G, given a subset H
std::vector<point_t> find_inner_points(const std::vector<point_t> &G, const std::vector<point_t> &H)
{
    std::vector<point_t> res;
    for (size_t i = 0; i < G.size(); i++)
    {
        if (std::find(H.begin(), H.end(), G[i]) != H.end())
        {
            continue;
        }
        else
            res.push_back(G[i]);
    }
    return res;
}

// Returns a list of point_ts on the convex hull in counter-clockwise order.
// Note: the last point_t in the returned list is the same as the first one.
std::vector<point_t> convex_hull(std::vector<point_t> P, std::vector<point_t> &is)
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
    std::vector<point_t> conc = concave_hull(H, P, 1, is);
/*    std::cout << "Concave edges:\n";
    for (size_t jarel = 0; jarel < conc.size(); jarel++)
    {
        std::cout << conc[jarel].p1 << "->" << conc[jarel].p2 << std::endl;
    } */
    
    return conc;
}

std::vector<point_t> concave_hull(const std::vector<point_t> &cH, std::vector<point_t> &G, double threshold, std::vector<point_t> &is)
{
    // Small cheat below; we dedicate a small time to turn input into edges instead of nodes
    std::vector<lines_t> convexEdges(cH.size());
    for (size_t i = 0; i < convexEdges.size() - 1; i++)
    {
        convexEdges[i] = lines_t(cH[i], cH[i + 1]);
    }
    convexEdges[convexEdges.size()-1] = lines_t(cH[convexEdges.size()-1], cH[0]);
    // -------- There we go. Now edges[] should be a vector of pairs of nodes corresponding to our surface
    // Now we copy convexEdges to concaveEdges
    std::vector<lines_t> concaveEdges = convexEdges;
    // And now, briefly build a set of points that are interior to the hull
    std::vector<point_t> innerPoints = find_inner_points(G, cH);
    for (size_t j = 0; j < innerPoints.size(); ) // Isso pode dar merda; n sei como concaveEdges atualiza
    {
        int closestIndex; double closestDistance = INFINITY; // We're gonna need this for the search below
        double thisDistance; // This will hold the value of the d_pt_ls func so we only compute it once/iteration
       
        // First find closest inner point to the edge
        for (size_t i = 0; i < concaveEdges.size(); i++)
        {
            thisDistance = distance_point_to_line_segment(innerPoints[j], concaveEdges[i]);
            if (thisDistance <= closestDistance)
            {
                closestIndex = i;
                closestDistance = thisDistance;
            }
        }
        
        // Now closestIndex should hold the index (in innerPoints) of the closest inner point to current edge
        // If it doesn't, it means there are no more inner points. We won't execute the next part.
        if (innerPoints.size() == 0 || concaveEdges.size() == 0)
        {
            break;
        }
        // We get the edge length
        double edgeLength = find_norm(concaveEdges[closestIndex].p2 - concaveEdges[closestIndex].p1);
        
        // Decide which end of the edge is closest to the closest inner point
        double distToP2 = find_norm(concaveEdges[closestIndex].p2 - innerPoints[j]);
        double distToP1 = find_norm(concaveEdges[closestIndex].p1 - innerPoints[j]);
        double dd;
        if (distToP2 < distToP1)
            dd = distToP2;
        else
            dd = distToP1;
        
        if (edgeLength / dd > threshold) // If our threshold is satisfied
        {
            // Then we'll remove one edge and add 2, this we don't increase the counter, lest we skip an edge
            concaveEdges.push_back(lines_t(concaveEdges[closestIndex].p1, innerPoints[j]));
            concaveEdges.push_back(lines_t(concaveEdges[closestIndex].p2, innerPoints[j]));
            concaveEdges.erase(concaveEdges.begin() + closestIndex);
            
            // We'll also remove this point from innerPoints, as it now sits on the boundary
            innerPoints.erase(innerPoints.begin() + j);
        }
        else // else we'll advance the counter, but not alter our current hull
            j++; // We advance only conditionally
    }
    // Check for intersections
    for (size_t i = 0; i < concaveEdges.size(); i++)
    {
        for (size_t j = i; j < concaveEdges.size(); j++)
        {
            point_t where;
            if (find_lines_intersection(concaveEdges[i], concaveEdges[j], where))
            {
              //  std::cout << "at " << where << ", btwn " << concaveEdges[i] << " and " << concaveEdges[j]<< std::endl;
                is.push_back(where);
            }
        }
    }
    
    std::vector<point_t> cycle;
    
    cycle.push_back(concaveEdges[0].p1);

    while (cycle.size() < concaveEdges.size())
    {
        for (size_t i = 0; i < concaveEdges.size(); i++)
        {
            if (concaveEdges[i].p1 == cycle[cycle.size() - 1] && (std::find(cycle.begin(), cycle.end(), concaveEdges[i].p2) == cycle.end()))
            {
                cycle.push_back(concaveEdges[i].p2);
                break;
            }
            if (concaveEdges[i].p2 == cycle[cycle.size() - 1] && (std::find(cycle.begin(), cycle.end(), concaveEdges[i].p1) == cycle.end()))
            {
                cycle.push_back(concaveEdges[i].p1);
                break;
            }
        }
    }
        
    
    for (size_t i = 0; i < cycle.size(); i++)
    {
        std::cout << cycle[i] << std::endl;
    }
    
    return cycle;
}

// Runs in O(1) time. Computes where the intersection of two linesegments lies
bool find_lines_intersection(lines_t a, lines_t b, point_t &where)
{
    // First rewrite linesegments a and b in vectorial form
    point_t p, q;
    point_t r, s;
    
    p = a.p1;         q = b.p1;
    r = a.p2 - a.p1;  s = b.p2 - b.p1;
    
    // Now we have: line = q + qv, and any point on the vector is obtainable by p + t*r, for some t
    // We want a t and u so p + t*pv = q + u*qv. Then t = (q − p) × s / (r × s) and u = (q − p) × r / (r × s)
    double t, u;
    double crossRandS = cross(r, s);
    double crossQminusPandS = cross(q - p, s);
    double crossQminusPandR = cross(q - p, r);
    
    // Collinear and parallel possibilities don't matter here. We simply treat them as non-intersections
    if (crossRandS == 0)
    {
        return false;
    }
    
    t = crossQminusPandS / crossRandS;
    u = crossQminusPandR / crossRandS;
    
    // If both T and U are in [0, 1] interval, then an intersection happens. We adopt an exclusive interval
    // so that neighbor edges aren't taken to be intersecting.
    if (gtTolerance(t, 0, TOLERANCE) && ltTolerance(t, 1, TOLERANCE) && gtTolerance(u, 0, TOLERANCE) && ltTolerance(u, 1, TOLERANCE))
    {
        where = p + r * t;
        return true;
    }
    
    // If control reaches this point, previous test failed. There is no intersection.
    return false;
}

int find_surface_intersections(const std::vector<SurfaceData_t*>& xs, std::vector<point_t> &is)
{
    struct timeval timer;
    time_b(timer);
   /* std::vector<lines_cg> linePool;
    
    // Compute all intersection points.
    std::list<point_cg>     pts;
    
    // This turns into n (constructing input) + n lg n (BentleyOttman)
    for (size_t i = 0; i < xs.size(); i++)  // For every surface in our input
    {
        for (ListDigraph::ArcIt ed(xs[i]->graph); ed != INVALID; ++ed) // We iterate thru its arcs
        {
            point_t pt1 = (*xs[i]->coords)[xs[i]->graph.source(ed)]; // And create a pool of all lines
            point_t pt2 = (*xs[i]->coords)[xs[i]->graph.target(ed)]; // which cannot intersect
            point_cg pc1(pt1.x, pt2.y);
            point_cg pc2(pt2.x, pt2.y);
            linePool.push_back(lines_cg(pc1, pc2));
        }
    }
    
    CGAL::compute_intersection_points (linePool.begin(), linePool.end(),
                                       std::back_inserter (pts));
    std::cout << "Time to find is: " << time_a(timer) << " with CGAL. linePool.size: " << linePool.size() << std::endl;
    return pts.size();
    */
    int count = 0;
    int linesCounted = 0;
    for (size_t i = 0; i < xs.size(); i++)
    {
        for (ListDigraph::ArcIt ed(xs[i]->graph); ed != INVALID; ++ed)
        {
            for (size_t j = i; j < xs.size(); j++)
            {
                for (ListDigraph::ArcIt edi(xs[j]->graph); edi != INVALID; ++edi)
                {
                    point_t where;
                    
                    lines_t ediL((*xs[j]->coords)[xs[j]->graph.source(edi)], (*xs[j]->coords)[xs[j]->graph.target(edi)]);
                    lines_t edL((*xs[i]->coords)[xs[i]->graph.source(ed)], (*xs[i]->coords)[xs[i]->graph.target(ed)]);
                    if (find_lines_intersection(ediL, edL, where))
                    {
                        is.push_back(where);
                        count++;
                    }
                }

            }
        }
    }
 //   std::cout << "Time to find is: " << time_a(timer) << " with brutish" << std::endl;
    return count;
    
}



double calculate_surface_area(const SurfaceData_t& s, double &perim)
{
    double area = 0;
    perim = 0;
    
    ListDigraph::InArcIt prev;
    ListDigraph::OutArcIt next;
    SNode fnode;
    
    SNode no = s.graph.nodeFromId(0);
    fnode = no;
    next = ListDigraph::OutArcIt(s.graph, no);
    no = s.graph.target(next);
    for(; no != fnode ;)
    {
        prev = ListDigraph::InArcIt(s.graph, no);
        next = ListDigraph::OutArcIt(s.graph, no);
        
        // Perimeter calculation: -----------------------------------------------------
        point_t perimVector = (*s.coords)[no] - (*s.coords)[s.graph.source(prev)]; //--
        double perimVectorNorm = find_norm(perimVector); //----------------------------
        perim += perimVectorNorm; //---------------------------------------------------
        // ----------------------------------------------------------------------------
        
        area += (*s.coords)[no].x * ( (*s.coords)[s.graph.target(next)].y - (*s.coords)[s.graph.source(prev)].y);
        no = s.graph.target(next);
    }
    prev = ListDigraph::InArcIt(s.graph, no);
    next = ListDigraph::OutArcIt(s.graph, no);
    
    // Perimeter calculation: -----------------------------------------------------
    point_t perimVector = (*s.coords)[no] - (*s.coords)[s.graph.source(prev)]; //--
    double perimVectorNorm = find_norm(perimVector); //----------------------------
    perim += perimVectorNorm; //---------------------------------------------------
    // ----------------------------------------------------------------------------
    
    area += (*s.coords)[no].x * ( (*s.coords)[s.graph.target(next)].y - (*s.coords)[s.graph.source(prev)].y);
    area /= 2;
    
    if (area < 0)
        area = 0;
    
    std::cout << "THIS CALL IS RETURNING " << perim << std::endl;
    
    return area;
}
