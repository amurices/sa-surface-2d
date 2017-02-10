//
//  Auxiliares.cpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Auxiliares.hpp"

void copySurface(surface* target, surface* origin)
{
    for (size_t i = 0; i < origin->v.size(); i++)
    {
        target->v.push_back(point(origin->v[i].x, origin->v[i].y, origin->v[i].pid));
    }
    
    for (size_t i = 0; i < origin->e.size(); i++)
    {
        int originu = origin->e[i].first->pid;
        int originv = origin->e[i].second->pid;
        target->e.push_back(std::make_pair(&target->v[originu], &target->v[originv]));
        
        target->v[originu].neighbor0 = &target->v[origin->v[i].neighbor0->pid]; // Confuso mas aqui é onde
        target->v[originu].neighbor1 = &target->v[origin->v[i].neighbor1->pid]; // se traduz os vizinhos
    }
}

std::vector<float> convertStringVectortoDoubleVector(const std::vector<std::string>& stringVector){
    std::vector<float> doubleVector(stringVector.size());
    
    std::transform(stringVector.begin(), stringVector.end(), doubleVector.begin(), [](const std::string& val)
                   {
                       return stod(val);
                   });
    return doubleVector;
}

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


void printGene(const GA1DArrayAlleleGenome<double>& genomaTeste)
{
    for (size_t i = 0; i <  genomaTeste.length(); i++)
    {
        std::cout << "gene " << i << std::setprecision(2) << genomaTeste.gene(i) << std::endl;
    }
}

void printSet(std::vector<point*> toPrint)
{
    for (size_t i = 0; i < toPrint.size(); i++)
    {
        std::cout << "v" << i << ": " << toPrint[i]->x << ", " << toPrint[i]->y << std::endl;
        std::cout << "n0" << ": " << toPrint[i]->neighbor0->x << ", " << toPrint[i]->neighbor0->y << std::endl;
        std::cout << "n1" << ": " << toPrint[i]->neighbor1->x << ", " << toPrint[i]->neighbor1->y << std::endl;
        std::cout << std::endl;

    }
}

void printEdge(link_g e)
{
    std::cout << "(" << e.first->x << ", " << e.first->y << ") -> (" << e.second->x << ", " << e.second->y << ")";
}

void printPoint(point x)
{
    std::cout << "(" << x.x << ", " << x.y << ")";
}

double dist (point p1, point p2)
{
    std::pair<double, double> norm = std::make_pair(p2.x - p1.x, p2.y - p1.y);
    return sqrt( norm.first * norm.first + norm.second * norm.second );
}


int IsPointInBoundingBox(float x1, float y1, float x2, float y2, float px, float py)
{
    float left, top, right, bottom; // Bounding Box For Line Segment
    // For Bounding Box
    if(x1 < x2)
    {
        left = x1;
        right = x2;
    }
    else
    {
        left = x2;
        right = x1;
    }
    if(y1 < y2)
    {
        top = y1;
        bottom = y2;
    }
    else
    {
        top = y1;
        bottom = y2;
    }

    if( (px+0.001) >= left && (px-0.001) <= right &&
       (py+0.001) >= top && (py-0.001) <= bottom )
    {
        return 1;
    }
    else
        return 0;
}// ---------------------------------------  http://www.softwareandfinance.com/Visual_CPP/VCPP_Intersection_Two_line_Segments_EndPoints.html



int LineIntersection(float l1x1, float l1y1, float l1x2, float l1y2,
                     float l2x1, float l2y1, float l2x2, float l2y2,
                     float *m1, float *c1, float *m2, float *c2,
                     float* intersection_X, float* intersection_Y)

{
    float dx, dy;
    dx = l1x2 - l1x1;
    dy = l1y2 - l1y1;
    
    *m1 = dy / dx;
    // y = mx + c
    // intercept c = y - mx
    *c1 = l1y1 - *m1 * l1x1; // which is same as y2 - slope * x2
    
    
    dx = l2x2 - l2x1;
    dy = l2y2 - l2y1;
    
    *m2 = dy / dx;
    // y = mx + c
    // intercept c = y - mx
    *c2 = l2y1 - *m2 * l2x1; // which is same as y2 - slope * x2
    
    if( (*m1 - *m2) == 0)
        return 0;
    else
    {
        *intersection_X = (*c2 - *c1) / (*m1 - *m2);
        *intersection_Y = *m1 * *intersection_X + *c1;
        return 1;
    }
    
}// ---------------------------------------  http://www.softwareandfinance.com/Visual_CPP/VCPP_Intersection_Two_line_Segments_EndPoints.html



int LineSegmentIntersection(float l1x1, float l1y1, float l1x2, float l1y2,
                            float l2x1, float l2y1, float l2x2, float l2y2,
                            float *m1, float *c1, float *m2, float *c2,
                            float* intersection_X, float* intersection_Y)

{
    float dx, dy;
    dx = l1x2 - l1x1;
    dy = l1y2 - l1y1;
    
    *m1 = dy / dx;
    // y = mx + c
    // intercept c = y - mx
    *c1 = l1y1 - *m1 * l1x1; // which is same as y2 - slope * x2
    
    dx = l2x2 - l2x1;
    dy = l2y2 - l2y1;
    
    *m2 = dy / dx;
    // y = mx + c
    // intercept c = y - mx
    *c2 = l2y1 - *m2 * l2x1; // which is same as y2 - slope * x2
    
    if( (*m1 - *m2) == 0)
        return 0;
    else
    {
        *intersection_X = (*c2 - *c1) / (*m1 - *m2);
        *intersection_Y = *m1 * *intersection_X + *c1;
    }
    if(IsPointInBoundingBox(l1x1, l1y1, l1x2, l1y2, *intersection_X, *intersection_Y) == 1 &&
       IsPointInBoundingBox(l2x1, l2y1, l2x2, l2y2, *intersection_X, *intersection_Y) == 1)
    {
        return 1;
    }
    else
        return 0;
    
} // ---------------------------------------  http://www.softwareandfinance.com/Visual_CPP/VCPP_Intersection_Two_line_Segments_EndPoints.html

int countIntersections(const surface& surf, std::vector<std::pair<float, float> >& where)
{
    float m1, c1, m2, c2, intersection_X, intersection_Y;
    int acc = 0;
    for (size_t i = 0; i < surf.e.size(); i++)
    {
        for (size_t j = surf.e.size() - 1; j > i; j--)
        {
            link_g a, b;
            a = surf.e[i];
            b = surf.e[j];
            if (intersect(a, b))
            {
                acc++;
                where.push_back(std::make_pair(intersection_X, intersection_Y));
            }
        }
    }
    return acc;
}


double CCW(point a, point b, point c)
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

int intersect(link_g a, link_g b) {
    if ( ( CCW(*(a.first), *(a.second), *(b.first)) * CCW(*(a.first), *(a.second), *(b.second)) < 0 ) &&
        ( CCW(*(b.first), *(b.second), *(a.first)) * CCW(*(b.first), *(b.second), *(a.second)) < 0 ) ) return 1;
    
    if ( CCW(*(a.first), *(a.second), *(b.first)) == 0 && middle(a.first->x, a.second->x, b.first->x) && middle(a.first->y, a.second->y, b.first->y) ) return 1;
    if ( CCW(*(a.first), *(a.second), *(b.second)) == 0 && middle(a.first->x, a.second->x, b.second->x) && middle(a.first->y, a.second->y, b.second->y) ) return 1;
    if ( CCW(*(b.first), *(b.second), *(a.first)) == 0 && middle(b.first->x, b.second->x, a.first->x) && middle(b.first->y, b.second->y, a.first->y) ) return 1;
    if ( CCW(*(b.first), *(b.second), *(a.second)) == 0 && middle(b.first->x, b.second->x, a.second->x) && middle(b.first->y, b.second->y, a.second->y) ) return 1;
    
    return 0;
}
