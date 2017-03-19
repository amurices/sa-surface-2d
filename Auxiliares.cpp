//
//  Auxiliares.cpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Auxiliares.hpp"




void printGene(const GA1DArrayAlleleGenome<double>& genomaTeste)
{
    for (size_t i = 0; i <  genomaTeste.length(); i++)
    {
        std::cout << "gene " << i << std::setprecision(2) << genomaTeste.gene(i) << std::endl;
    }
}

void printPoint(point_t x)
{
    std::cout << "(" << x.x << ", " << x.y << ")";
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

int countIntersections(const SurfaceData_t& surf, std::vector<std::pair<float, float> >& where)
{
    float intersection_X, intersection_Y;
    int acc = 0;
    for (size_t i = 0; i < surf.nEdges; i++)
    {
        for (size_t j = surf.nEdges - 1; j > i; j--)
        {
            ListDigraph::Arc a, b;
            a = surf.graph.arcFromId(i);
            b = surf.graph.arcFromId(j);
            if (intersect(surf, a, b))
            {
                acc++;
                where.push_back(std::make_pair(intersection_X, intersection_Y));
            }
        }
    }
    return acc;
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
