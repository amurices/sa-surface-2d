//
//  Optimizer.hpp
//  Surface
//
//  Created by André Muricy on 26/4/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Optimizer_hpp
#define Optimizer_hpp

#include <stdio.h>
#include "defs.h"
#include "Auxiliares.hpp"
#include "SurfaceDecoder.hpp"
#include "BRKGA.h"
#include "MTRand.h"


class Optimizer{
public:
    ThickSurface_t *org;    // Reference to original surface
    SurfaceDecoder *decoder;
    BRKGA< SurfaceDecoder, MTRand > *algorithm;     // heap-allocated evolver
    unsigned X_INTVL;       // exchange best individuals at every X_INTVL generations
    unsigned X_NUMBER;      // exchange top X_NUMBER best
    unsigned MAX_GENS;      // run for MAX_GENS generations
    void init_GA(const unsigned ps, const double ep, const double mp, const double rhoe, const unsigned K, const unsigned MAXT, const unsigned exi, const unsigned exn, const unsigned num_gens, const unsigned x_intvl, const unsigned x_number, const unsigned max_gens);

    void step();
    void evolve(int MAX_GENS, bool time = true);
};

#endif /* Optimizer_hpp */
