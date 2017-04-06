//
//  Auxiliares.hpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Auxiliares_hpp
#define Auxiliares_hpp


#include "defs.h"// Definições, tipos, etc
#include "Outsiders/prettyprint.hpp"

#include <ga/ga.h>
#include <ga/GARealGenome.h>
#include <vector>
#include <math.h>
#include <iomanip>
#include <sstream>



void printGene(const GA1DArrayAlleleGenome<double>& genomaTeste);

void printSet(std::vector<point_t*> toPrint);

void printDegrees(const SurfaceData_t &surf);

bool checkIntersection(point_t p1, point_t p2, point_t p3, point_t p4);

double dist(point_t p1, point_t p2);

int countIntersections(const SurfaceData_t& surf, std::vector<std::pair<float, float> >& where);

double CCW(point_t a, point_t b, point_t c);

int middle(int a, int b, int c);

int intersect(const SurfaceData_t &surf, ListDigraph::Arc a, ListDigraph::Arc b);

#endif /* Auxiliares_hpp */
