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
        target->v.push_back(point_g(origin->v[i].x, origin->v[i].y, origin->v[i].pid));
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

void printSet(std::vector<point_g*> toPrint)
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

void printPoint(point_g x)
{
    std::cout << "(" << x.x << ", " << x.y << ")";
}

double dist (point_g p1, point_g p2)
{
    std::pair<double, double> norm = std::make_pair(p2.x - p1.x, p2.y - p1.y);
    return sqrt( norm.first * norm.first + norm.second * norm.second );
}

int countIntersections(const surface& surf, std::vector<std::pair<float, float> >& where)
{
    float intersection_X, intersection_Y;
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


double CCW(point_g a, point_g b, point_g c)
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
