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

void copy_surface(const SurfaceData_t &org, SurfaceData_t &trg)
{
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
    
        trgCurrToMap = trg.graph.addNode();
        trg.nNodes++;
        (*trg.coords)[trgCurrToMap] = (*org.coords)[curr] - vd;
        
        trg.graph.addArc(trgPrevToMap, trgCurrToMap);
        trg.nEdges++;
        trgPrevToMap = trgCurrToMap;
    }
    
    trg.graph.addArc(trgPrevToMap, finode);
    trg.nEdges++;
}

// Start timer
void time_b(struct timeval &tvalBefore)
{
    gettimeofday (&tvalBefore, NULL);
}


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

void printDegrees(const SurfaceData_t &surf)
{
    for (ListDigraph::NodeIt ne(surf.graph); ne != INVALID; ++ne)
    {
        ListDigraph::InArcIt	inCurrI(surf.graph, ne);		// get Pn
        ListDigraph::OutArcIt	outCurrI(surf.graph, ne);		// and P1
        int countInc = 0;
        int countOut = 0;
        while (inCurrI != INVALID)
        {
            countInc++;
            ++inCurrI;
        }
        while (outCurrI != INVALID)
        {
            countOut++;
            ++outCurrI;
        }
        
        std::cout << "Node " << (*surf.coords)[ne] << ": " << countOut << " out, " << countInc << " in\n";
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

int intersect(const SurfaceData_t &surf, ListDigraph::Arc a, ListDigraph::Arc b) {
    point_t *p1a, *p2a;
    point_t *p1b, *p2b;
    
    // Pegamos as referências às coordenadas dos pontos que queremos verificar
    p1a = &(*surf.coords)[ surf.graph.source(a) ];
    p2a = &(*surf.coords)[ surf.graph.target(a) ];
    
    p1b = &(*surf.coords)[ surf.graph.source(b) ];
    p2b = &(*surf.coords)[ surf.graph.target(b) ];

    
    if ( ( CCW(*(p1a), *(p2a), *(p1b)) * CCW(*(p1a), *(p2a), *(p2b)) < 0 ) &&
        ( CCW(*(p1b), *(p2b), *(p1a)) * CCW(*(p1b), *(p2b), *(p2a)) < 0 ) ) return 1;
    
    if ( CCW(*(p1a), *(p2a), *(p1b)) == 0 && middle(p1a->x, p2a->x, p1b->x) && middle(p1a->y, p2a->y, p1b->y) ) return 1;
    if ( CCW(*(p1a), *(p2a), *(p2b)) == 0 && middle(p1a->x, p2a->x, p2b->x) && middle(p1a->y, p2a->y, p2b->y) ) return 1;
    if ( CCW(*(p1b), *(p2b), *(p1a)) == 0 && middle(p1b->x, p2b->x, p1a->x) && middle(p1b->y, p2b->y, p1a->y) ) return 1;
    if ( CCW(*(p1b), *(p2b), *(p2a)) == 0 && middle(p1b->x, p2b->x, p2a->x) && middle(p1b->y, p2b->y, p2a->y) ) return 1;
    
    return 0;
}
