//
//  Interfacer.hpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Interfacer_hpp
#define Interfacer_hpp

// #include <matio.h> We commented MatIO out because we're trying to purge our dependencies a little
#include <FTGL/ftgl.h> // Para carregar fontes

#include "defs.h"
#include "Geometry.hpp"
#include "Auxiliares.hpp"

class Interfacer{
public:
    Interfacer();
    static void generate_random     (ThickSurface_t &ts, double perim, int pts, std::vector<point_t> &is);
    static void generate_circle     (ThickSurface_t &ts, double radius, double thick_ratio, int pts, std::vector<point_t> &is);
    static void generate_outer_s    (SurfaceData_t &surf,   double perim, int pts, std::vector<point_t> &is);
    static void generate_hull       (SurfaceData_t &surf, SurfaceData_t &points);
    static void generate_inner_s    (SurfaceData_t &inner,  SurfaceData_t &surf, std::vector<double> &thickness);
    static void generate_bridges    (ThickSurface_t &ts);
    static void update_inner_s      (SurfaceData_t &inner,  SurfaceData_t &surf, std::vector<double> &thickness);
    static void update_inner_node   (SurfaceData_t &inner,  SurfaceData_t &surf, double thickness, int index);

    static void update_bridges      (ThickSurface_t &ts);

    static void get_from_matlab     (SurfaceData_t &surf,   const char* matFile);
    static void get_from_ttf        (FTGLPixmapFont *fonti, const char* ttfFile);
};

#endif /* Interfacer_hpp */
