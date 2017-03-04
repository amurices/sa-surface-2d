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
#include <utility>
#include <vector>
#include <map>
#include <ga/GARealGenome.h>



typedef struct point_g{
    int pid; // Facilita a cópia
    point_g *neighbor0, *neighbor1;
    double x;
    double y;
    point_g (double xn, double yn, int idn = 0)
    {
        neighbor0 = this; neighbor1 = this; x = xn; y = yn; pid = idn;
    }
}point_g;


typedef std::pair<point_g*, point_g*> link_g;

typedef struct surface{
    std::vector<point_g>   v;
    std::vector<link_g>    e;
} surface;

#endif /* defs_h */
