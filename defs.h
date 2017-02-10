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
#include <sstream>
#include <iostream>
#include <string>
#include <math.h>
#include <iomanip>
#include <utility>
#include <vector>
#include <map>
#include <cstdlib>
#include <ga/ga.h>
#include <ga/GARealGenome.h>



typedef struct point{
    int pid; // Facilita a cópia
    point *neighbor0, *neighbor1;
    double x;
    double y;
    point (double xn, double yn, int idn = 0)
    {
        neighbor0 = this; neighbor1 = this; x = xn; y = yn; pid = idn;
    }
}point;


typedef std::pair<point*, point*> link_g;

typedef struct surface{
    std::vector<point>   v;
    std::vector<link_g>    e;
} surface;

#endif /* defs_h */
