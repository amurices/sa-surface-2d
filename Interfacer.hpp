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
#include <FTGL/ftgl.h> // Para carregar fontes

#include "defs.h"
#include "Geometry.hpp"
#include "Auxiliares.hpp"

class Interfacer{
public:
    Interfacer();
    static void generate_random     (ThickSurface_t &ts, double perim, int pts);
    static void generate_outer_s    (SurfaceData_t &surf,   double perim, int pts);
    static void generate_outer_s0    (SurfaceData_t &surf,   double perim, int pts);
    static void generate_inner_s    (SurfaceData_t &inner,  const SurfaceData_t &surf, Thicks_t *ts);
    static void get_from_matlab     (SurfaceData_t &surf,   const char* matFile);
    static void get_from_ttf        (FTGLPixmapFont *fonti, const char* ttfFile);
};

#endif /* Interfacer_hpp */
