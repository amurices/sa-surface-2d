//
//  Auxiliares.cpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Auxiliares.hpp"

bool ltTolerance(double a, double b, double tol)
{
    return (a < (b - tol));
}
bool gtTolerance(double a, double b, double tol)
{
    return (a > (b + tol));
}

bool eqTolerance(double a, double b, double tol)
{
    return (a > b - tol) && (a < b + tol);
}

double absol(double x)
{
    if (x < 0)
        return -x;
    return x;
}




void shift_to_origin(ThickSurface_t &org)
{
    point_t acc(0,0);
    int count = 0;
    // Apply offsets to surface
    for (ListDigraph::NodeIt no(org.outer.graph); no != INVALID; ++no)
    {
        count++;
        acc = acc + (*org.outer.coords)[no];
    }
    
    acc = acc * (1/(double)org.outer.nNodes);
    
    // Shift points to origin
    for (ListDigraph::NodeIt no(org.outer.graph); no != INVALID; ++no)
    {
        (*org.outer.coords)[no].x -= acc.x;
        (*org.outer.coords)[no].y -= acc.y;
        (*org.inner.coords)[no].x -= acc.x;
        (*org.inner.coords)[no].y -= acc.y;
    }
    // Shift "bridges" to origin
    for (ListDigraph::NodeIt no(org.bridges.graph); no != INVALID; ++no)
    {
        (*org.bridges.coords)[no].x -= acc.x;
        (*org.bridges.coords)[no].y -= acc.y;
    }
}

void copy_surface(const SurfaceData_t &org, SurfaceData_t &trg)
{
    if (trg.nNodes > 0) // If non-empty target surface, we wipe it out, avoiding a memory leak
    {
        trg.nNodes = 0;
        trg.nEdges = 0;
        trg.graph.clear();
    }
    
    SNode	prev, next, last;
    point_t	pPrev, pNext, pCurr, vd;
    
    SNode fnode		= org.graph.nodeFromId(0);				// fnode = P0

    ListDigraph::InArcIt	inCurrI(org.graph, fnode);		// get Pn
    ListDigraph::OutArcIt	outCurrI(org.graph, fnode);     // and P1
    
    prev = org.graph.source(inCurrI);		// cont ^
    next = org.graph.target(outCurrI);		// cont ^
    
    pNext = (*org.coords)[next];			// get coordinates Pn
    pPrev = (*org.coords)[prev];			// and P0
    
    SNode finode = trg.graph.addNode();                 // Add node à inner; guarda a referencia ao primeiro no
    SNode trgCurrToMap = finode;                        // Primeiro no agora é o atual
    trg.nNodes++;                                       // incrementa no. de nós
    (*trg.coords)[trgCurrToMap] = (*org.coords)[fnode];	// seta coordenadas
    SNode trgPrevToMap = trgCurrToMap;                  // prev agora é o atual
    SNode curr;
    for (curr = next; curr != fnode; curr = next)
    {
        
        ListDigraph::OutArcIt	outCurr(org.graph, curr);
        ListDigraph::InArcIt	inCurr(org.graph, curr);
        
        next = org.graph.target(outCurr);
        prev = org.graph.source(inCurr);
        
        pNext = (*org.coords)[next];
        pPrev = (*org.coords)[prev];
     //   std::cout << "In copy! org: " << (*org.coords)[curr] << std::endl;
        trgCurrToMap = trg.graph.addNode();
        trg.nNodes++;
        (*trg.coords)[trgCurrToMap] = (*org.coords)[curr] - vd;
     //   std::cout << "In copy! trg: " << (*trg.coords)[curr] << std::endl;

        trg.graph.addArc(trgPrevToMap, trgCurrToMap);
        trg.nEdges++;
        trgPrevToMap = trgCurrToMap;
    }
    
    trg.graph.addArc(trgPrevToMap, finode);
    trg.nEdges++;
}

void copy_thick_surface(const ThickSurface_t &org, ThickSurface_t &trg)
{
    copy_surface(org.outer, trg.outer);                                  // Copying outer surface is O(n)
    trg.thickness = org.thickness;                                       // Copying thickness is O(1)
    Interfacer::generate_inner_s(trg.inner, trg.outer, trg.thickness);  // Generating inner surface is O(n)
    Interfacer::generate_bridges(trg);                                   // Generating bridges is O(n)
}

// Start timer
void time_b(struct timeval &tvalBefore)
{
    gettimeofday (&tvalBefore, NULL);
}

// End timer, return seconds since last time_b call
float time_a(struct timeval &tvalBefore)
{
    struct timeval tvalAfter;  // removed comma
    gettimeofday (&tvalAfter, NULL);
    float timeSince = tvalAfter.tv_sec - tvalBefore.tv_sec;
    float diff = tvalBefore.tv_usec - tvalAfter.tv_usec;
    if (diff > 0)
        timeSince -= 1.0;
    timeSince += std::abs(diff) / 1000000;
    return timeSince;
}

void print_nodes_coordinates(const SurfaceData_t &surf)
{
    int count = 0;
    for (ListDigraph::NodeIt ne(surf.graph); ne != INVALID; ++ne)
    {
        std::cout << count << ":\t " << (*surf.coords)[ne] << std::endl;
        count++;
    }
}

double dist (point_t p1, point_t p2)
{
    std::pair<double, double> norm = std::make_pair(p2.x - p1.x, p2.y - p1.y);
    return sqrt( norm.first * norm.first + norm.second * norm.second );
}

double CCW(point_t a, point_t b, point_t c)
{
    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
}


int middle(int a, int b, int c) {
    int t;
    if ( a > b ) {
        t = a;
        a = b;
        b = t;
    }
    if ( a <= c && c <= b ) return 1;
    return 0;
}


