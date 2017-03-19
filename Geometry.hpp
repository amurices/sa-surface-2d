//
//  Geometry.hpp
//  Surface
//
//  Created by André Muricy on 5/3/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Geometry_hpp
#define Geometry_hpp

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "defs.h"

double      to_angle(const struct point_t *p, const struct point_t *o);

void        find_centroid(struct point_t *c, struct point_t *pts, int n_pts);

int         by_polar_angle(const void *va, const void *vb);

void        sort_by_polar_angle(struct point_t *pts, int n_pts);

double      cross(const point_t &O, const point_t &A, const point_t &B);

point_t     find_direction_vector(const point_t &a, const point_t &b);


std::vector<point_t> convex_hull(std::vector<point_t> P);


#endif /* Geometry_hpp */
