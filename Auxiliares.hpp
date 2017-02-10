//
//  Auxiliares.hpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Auxiliares_hpp
#define Auxiliares_hpp

#include <stdio.h>
#include "defs.h"

void copySurface(surface* target, surface* origin);

std::vector<float> convertStringVectortoDoubleVector(const std::vector<std::string>& stringVector);

void split(const std::string &s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);

void printGene(const GA1DArrayAlleleGenome<double>& genomaTeste);

void printSet(std::vector<point*> toPrint);

void printEdge(link_g e);

void printPoint(point x);

bool checkIntersection(point p1, point p2, point p3, point p4);

double dist(point p1, point p2);

int IsPointInBoundingBox(float x1, float y1, float x2, float y2, float px, float py);


int LineIntersection(float l1x1, float l1y1, float l1x2, float l1y2,
                      float l2x1, float l2y1, float l2x2, float l2y2,
                      float *m1, float *c1, float *m2, float *c2,
                      float* intersection_X, float* intersection_Y);

int LineSegmentIntersection(float l1x1, float l1y1, float l1x2, float l1y2,
                             float l2x1, float l2y1, float l2x2, float l2y2,
                             float *m1, float *c1, float *m2, float *c2,
                             float* intersection_X, float* intersection_Y);

int countIntersections(const surface& surf, std::vector< std::pair<float, float> >& where);


double CCW(point a, point b, point c);

int middle(int a, int b, int c);

int intersect(link_g a, link_g b);

#endif /* Auxiliares_hpp */
