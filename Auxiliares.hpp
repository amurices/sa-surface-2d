//
//  Auxiliares.hpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Auxiliares_hpp
#define Auxiliares_hpp


#include "defs.h"// Definitions, typedefs etc
#include "Interfacer.hpp" // Generating inside of surface
#include "Outsiders/prettyprint.hpp"

#include <vector>
#include <sys/time.h>
#include <math.h>
#include <iomanip>
#include <sstream>

// Comparators for analytically precise floating point numbers. Tol must be positive.
bool ltTolerance(double a, double b, double tol);
bool gtTolerance(double a, double b, double tol);
bool eqTolerance(double a, double b, double tol);

double absol(double x);

void printSet(std::vector<point_t*> toPrint);
void print_nodes_coordinates(const SurfaceData_t &surf);

void shift_to_origin(ThickSurface_t &org);



void copy_surface(const SurfaceData_t &org, SurfaceData_t &trg);
void copy_thick_surface(const ThickSurface_t &org, ThickSurface_t &trg);

double dist(point_t p1, point_t p2);

double CCW(point_t a, point_t b, point_t c);

void time_b(struct timeval &tvalBefore);

float time_a(struct timeval &tvalBefore);

int middle(int a, int b, int c);



#endif /* Auxiliares_hpp */
