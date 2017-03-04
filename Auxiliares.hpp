//
//  Auxiliares.hpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Auxiliares_hpp
#define Auxiliares_hpp

// Definições, tipos, etc
#include "defs.h"

#include <ga/ga.h>
#include <ga/GARealGenome.h>
#include <vector>
#include <math.h>
#include <iomanip>
#include <sstream>


void copySurface(surface* target, surface* origin);

std::vector<float> convertStringVectortoDoubleVector(const std::vector<std::string>& stringVector);

void split(const std::string &s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);

void printGene(const GA1DArrayAlleleGenome<double>& genomaTeste);

void printSet(std::vector<point_g*> toPrint);

void printEdge(link_g e);

void printPoint(point_g x);

bool checkIntersection(point_g p1, point_g p2, point_g p3, point_g p4);

double dist(point_g p1, point_g p2);

int countIntersections(const surface& surf, std::vector< std::pair<float, float> >& where);

double CCW(point_g a, point_g b, point_g c);

int middle(int a, int b, int c);

int intersect(link_g a, link_g b);

#endif /* Auxiliares_hpp */
