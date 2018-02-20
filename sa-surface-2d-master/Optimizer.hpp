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
#include <stdlib.h>
#include "Auxiliares.hpp"
#include "Outsiders/brkga/BRKGA.h"
#include "Outsiders/brkga/MTRand.h"
#include "SurfaceDecoder.hpp"


class Optimizer{
public:
    ThickSurface_t  *org;                   // Reference to original surface
    ThickSurface_t  *innerCircle = NULL;    // Inner and
    ThickSurface_t  *outerCircle = NULL;    // outer shapes that constrain surface movement

    int gen;
    int maxT = 6500;
    // GA attributes -----
    SurfaceDecoder  *dec;                            // Reference to decoder
    MTRand          *rng;                            // Reference to RNG object
    BRKGA< SurfaceDecoder, MTRand > *algorithm;      // heap-allocated evolver
    std::vector<double> bestSolution;                // Where best solution is stored
    
    unsigned X_INTVL;       // exchange best individuals at every X_INTVL generations
    unsigned X_NUMBER;      // exchange top X_NUMBER best
    unsigned MAX_GENS;      // run for MAX_GENS generations
    
    // GA functions ------
    Optimizer(ThickSurface_t &org, MTRand &rng, SurfaceDecoder &dec); //  ga constructor
    void init_GA(const unsigned ps, const double ep, const double mp, const double rhoe, const unsigned K, const unsigned MAXT, const unsigned x_intvl, const unsigned x_number, const unsigned max_gens);
    void step_ga();
    void evolve_ga(bool time = true);
    void update_surface_ga(std::vector<double> &sol);
    void find_intersections(std::vector<point_t> &is);
    
    // -------------------
    // SA attributes a) To actually function ------
    // a.1) Hyperparameters - They alter the simulation's behavior
    double  scale = 1.0;       // Index that will adjust some hyperparameters of evolution
    double  a0 = 1.0;          // Area of the initial (relaxed) state
    int     smooth = 1.0;      // Whether neighbors should be calculated using smoothing depressions or not
    double  diffPow = 1.0;     // Power to raise difference btwn A0 and AS
    double  diffMul = 1.0;     // Scalar ^
    double  areaPow = 1.0;     // Analogous to
    double  areaMul = 1.0;     // previous two
    double  multiProb = 1.0;   // Probability of forcing another vertex after the first one
    double  tempProb = 1.0;    // How much weight does temperature hold
    double  forceOffsetRange = 1.0; // How much can points be shifted every iteration
    double  compression = 1.0; // How much should cortex be compressed by force
    // a.2) References - What the simulation needs to see to run correctly
    ThickSurface_t *ln;   // Last neighbour calculated; useful for mid-evolution rendering
    ThickSurface_t *state;       // Local variables evolved state,
    ThickSurface_t nstate;      // new state,
    ThickSurface_t nghbr;       // evolved state and random neighbor
    // SA attributes b) For testing purposes - these are interesting to output
    double gray = 2000;         // Gray matter of any given solution (outer - inner areas)
    double white = 2000;        // White matter (inner area)
    double stretch = 2000;      // Stretch factor for any solution (diffMul * diff btwn grays ^ diffPow)
    double perimeter = 2000;    // Perimeter of any given solution (sum of all vector lengths)
    double energy = 2000;       // And fitness of solution ofc.
    
    // SA functions ------
    Optimizer(ThickSurface_t &org); //met constructor
    void init_SA(double scale = 1, int smooth = 0,
                 double diffPow = 1, double diffMul = 1,
                 double areaPow = 1, double areaMul = 1,
                 double multiProb = 0, double forceOffsetRange = 0.066,
                 double tempProb = 0, double compression = 1);
    void step_sa();
    double probability(ThickSurface_t &s, double a0, double &p1, double &p2);
    double temperature();
    void neighbor(ThickSurface_t &org, ThickSurface_t &n);
    void evolve_sa(int kMax, bool time = true);

};

#endif /* Optimizer_hpp */
