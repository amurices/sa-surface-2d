//
//  Optimizer.cpp
//  Surface
//
//  Created by André Muricy on 26/4/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Optimizer.hpp"

void Optimizer::init_GA(const unsigned ps, const double ep, const double mp, const double rhoe, const unsigned K, const unsigned MAXT, const unsigned exi, const unsigned exn, const unsigned num_gens, const unsigned x_intvl, const unsigned x_number, const unsigned max_gens)
{
    const long unsigned rngSeed = 0;
    MTRand rng(rngSeed);                        // initialize the random number generator
    const unsigned n = org->outer.nNodes * 2;	// size of chromosomes
    X_INTVL = x_intvl;
    X_NUMBER = x_number;
    MAX_GENS = max_gens;
    algorithm = new BRKGA< SurfaceDecoder, MTRand >(n, ps, ep, mp, rhoe, *decoder, rng, K, MAXT);

}

void Optimizer::evolve(int MAX_GENS, bool time)
{
    int generation = 0;
    do {
        struct timeval tvalBefore;
        if (time) // Only calculate time if bool is set
            time_b(tvalBefore);
        
        step();	// evolve the population for one generation
        
        if (time) // 
            printf("gen %d:\n Time: %f seconds\n",generation, time_a(tvalBefore));
        
        if((++generation) % X_INTVL == 0) {     // if we're at one of the appropriate intervals
            algorithm->exchangeElite(X_NUMBER);	// exchange top individuals
        }
    } while (generation < MAX_GENS);
    
}

void Optimizer::step()
{
    algorithm->evolve();	// evolve the population for one generation
}
