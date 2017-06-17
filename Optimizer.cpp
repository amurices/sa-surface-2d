//
//  Optimizer.cpp
//  Surface
//
//  Created by André Muricy on 26/4/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Optimizer.hpp"

Optimizer::Optimizer(ThickSurface_t & org, MTRand &rng, SurfaceDecoder &dec)
{
    this->org = &org;
    this->rng = &rng;
    this->dec = &dec;
}

void Optimizer::init_GA(const unsigned ps, const double ep, const double mp, const double rhoe, const unsigned K, const unsigned MAXT, const unsigned x_intvl, const unsigned x_number, const unsigned max_gens)
{
    const long unsigned rngSeed = 0;
    const unsigned n = org->outer.nNodes * 2;	// size of chromosomes
    X_INTVL = x_intvl;
    X_NUMBER = x_number;
    MAX_GENS = max_gens;
    algorithm = new BRKGA< SurfaceDecoder, MTRand >(n, ps, ep, mp, rhoe, *dec, *rng, K, MAXT);

}

// Wrapper around
void Optimizer::evolve_ga(bool time)
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
    bestSolution = algorithm->getBestChromosome();
}

void Optimizer::step()
{
    algorithm->evolve();	// evolve the population for one generation
}


void Optimizer::update_surface_ga(std::vector<double> &sol)
{
    int count = 0;
    point_t acc(0,0);
    // Apply offsets to surface
    for (ListDigraph::NodeIt no(org->outer.graph); no != INVALID; ++no)
    {
        (*org->outer.coords)[no].x += sol[count];
        (*org->outer.coords)[no].y += sol[count + sol.size() * 0.5];
        count++;
        acc = acc + (*org->outer.coords)[no];
    }
    acc = acc * (1/(double)org->outer.nNodes);
    
    // Reset inner part of surface
    Interfacer::update_inner_s(org->inner, org->outer, org->thicknesses);
    // Reset bridges between outer and inner surfaces
    Interfacer::update_bridges(*org);
    
    // Shift points to origin
    for (ListDigraph::NodeIt no(org->outer.graph); no != INVALID; ++no)
    {
        (*org->outer.coords)[no].x -= acc.x;
        (*org->outer.coords)[no].y -= acc.y;
        (*org->inner.coords)[no].x -= acc.x;
        (*org->inner.coords)[no].y -= acc.y;
    }
    // Shift "bridges" to origin
    for (ListDigraph::NodeIt no(org->bridges.graph); no != INVALID; ++no)
    {
        (*org->bridges.coords)[no].x -= acc.x;
        (*org->bridges.coords)[no].y -= acc.y;
    }
}

// Wrapper around method to find intersection
void Optimizer::find_intersections(std::vector<point_t> &is)
{
    std::vector<SurfaceData_t*> surfaces;
    surfaces.push_back(&org->outer);
    surfaces.push_back(&org->inner);
    surfaces.push_back(&org->bridges);
    std::cout << "Num ints: " << find_surface_intersections(surfaces, is) << std::endl;
}

void Optimizer::neighbor(ThickSurface_t &original, ThickSurface_t &n)
{
    copy_surface(original.outer, n.outer);
    int randomIndex = rand() % n.outer.nNodes;
    
    
    SNode randomNode = n.outer.graph.nodeFromId(randomIndex);
    
    double offsetX, offsetY;
    offsetX = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX) * 0.5 - 0.25; // Random values btwn
    offsetY = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX) * 0.5 - 0.25; // -0.25 and 0.25
    
    std::cout << "randomIndex: " << randomIndex << ", offX: " << offsetX << ", offY: " << offsetY << std::endl;

    
    (*n.outer.coords)[randomNode].x += offsetX;
    (*n.outer.coords)[randomNode].y += offsetY;
    
    n.thicknesses = new Thicks_t(n.outer.graph);
    for (int i = 0; i < n.outer.nNodes; i++)
    {
        (*n.thicknesses)[n.outer.graph.nodeFromId(i)] = (*original.thicknesses)[original.outer.graph.nodeFromId(i)];
    }

    Interfacer::generate_inner_s(n.inner, n.outer, n.thicknesses);

    
    // Todo -- update bridges and inner
    Interfacer::generate_bridges(n);
}

double Optimizer::probability(ThickSurface_t &s)
{
    double res = calculate_surface_area(s.outer);
    
    std::vector<SurfaceData_t*> surfaces;
    std::vector<point_t> inters;

    surfaces.push_back(&s.outer);
    surfaces.push_back(&s.inner);
 //   surfaces.push_back(&s.bridges);
    
    int intsOuter = find_surface_intersections(surfaces, inters);
    res += 100000 * intsOuter;
    return res;
}

void Optimizer::evolve_sa(int kMax, bool time)
{
    ThickSurface_t state;       // Local variables evolved state,
    ThickSurface_t nstate;      // new state,
    ThickSurface_t nghbr;    // evolved state and random neighbor
    
    copy_thick_surface(*org, state); // S = S0
    
    struct timeval timer;
    std::cout << "Alright... here we go. Timing" << std::endl;
    for (int k = 0; k < kMax; k++)
    {
        time_b(timer);
        neighbor(state, nghbr);
        double probN = probability(nghbr);
        double probS = probability(state);
        std::cout << "In the " << k << "th step, prob of neighbor is " << probN << " and of S is " << probS <<std::endl;
        if (probN < probS)
        {
            copy_thick_surface(nghbr, state);
        }
        std::cout << "K = " << k << " and time: " << time_a(timer) << std::endl;

    }
    
    copy_thick_surface(state, *org);
    
    point_t acc(0,0);
    int count = 0;
    // Apply offsets to surface
    for (ListDigraph::NodeIt no(org->outer.graph); no != INVALID; ++no)
    {
        count++;
        acc = acc + (*org->outer.coords)[no];
    }
    acc = acc * (1/(double)org->outer.nNodes);
    
    
    // Shift points to origin
    for (ListDigraph::NodeIt no(org->outer.graph); no != INVALID; ++no)
    {
        (*org->outer.coords)[no].x -= acc.x;
        (*org->outer.coords)[no].y -= acc.y;
        (*org->inner.coords)[no].x -= acc.x;
        (*org->inner.coords)[no].y -= acc.y;
    }
    // Shift "bridges" to origin
    for (ListDigraph::NodeIt no(org->bridges.graph); no != INVALID; ++no)
    {
        (*org->bridges.coords)[no].x -= acc.x;
        (*org->bridges.coords)[no].y -= acc.y;
    }
    
    
 //   Let s = s0
  //  For k = 0 through kmax (exclusive):
  //  T ← temperature(k ∕ kmax)
   // Pick a random neighbour, snew ← neighbour(s)
  //  If P(E(s), E(snew), T) ≥ random(0, 1):
  //  s ← snew
    

}
