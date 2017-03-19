//
//  Interfacer.hpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Interfacer_hpp
#define Interfacer_hpp

#include <matio.h> // biblioteca de i/o pra .mat

#include "defs.h"
#include "Geometry.hpp"
#include "Auxiliares.hpp"

class Interfacer{
public:
    Interfacer();
    static void generate_random(SurfaceData_t &surf, double perim, int pts);
    static void generate_inner(SurfaceData_t &inner, const SurfaceData_t &surf);
    static void get_from_matlab(SurfaceData_t &surf, const char* matFile);
};

#endif /* Interfacer_hpp */
