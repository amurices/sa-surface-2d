//
//  Optimizer.cpp
//  Surface
//
//  Created by André Muricy on 26/4/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Optimizer.hpp"

// Optimizer::Optimizer(ThickSurface_t & org, MTRand &rng, SurfaceDecoder &dec)
// {
//     gen = 0;
//     this->org = &org;
//     this->rng = &rng;
//     this->dec = &dec;
// }

Optimizer::Optimizer(ThickSurface_t & org)
{
    gen = 0;
    this->org = &org;
}

void Optimizer::init_SA(double scale, int smooth,
                        double diffMul, double diffPow,
                        double areaPow, double areaMul,
                        double multiProb, double forceOffsetRange,
                        double tempProb, double compression)
{
    this->diffPow               = diffPow;
    this->diffMul               = diffMul;
    this->areaPow               = areaPow;
    this->areaMul               = areaMul;
    this->smooth                = smooth;
    this->multiProb             = multiProb;
    this->scale                 = scale;
    this->forceOffsetRange      = forceOffsetRange;
    this->tempProb              = tempProb;
    this->compression           = compression;
    gen = 0;
    double p;

    a0 = calculate_surface_area(org->outer, p) - calculate_surface_area(org->inner, p); // initial gray matter area
    copy_thick_surface(*org, state); // S = S0

    // Loop is ready to roll
}

void Optimizer::step_sa()
{
    neighbor(state, nghbr);
    double part1N, part2N, part1S, part2S;
    double probN = probability(nghbr, a0, part1N, part2N);
    double probS = probability(state, a0, part1S, part2S); // Our accessible scope always gets
    // its parameters from S if we run probability() on S after we do it on N
    if (probN < probS)
    {
        copy_thick_surface(nghbr, state);
        copy_thick_surface(nghbr, *org);
    }
    else if (probN < 10)
    {
        double randProb = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX);
        double diffNS = absol(probN - probS);
        double prob = (1.0/diffNS < 1.0 ? 1.0/diffNS : 1.0) * ((double)maxT - (double)gen)/(double)maxT * tempProb;
   //     std::cout << "\n\nPROB: " << prob << "\n\n";
        if (prob > randProb)
        {
            copy_thick_surface(nghbr, state);
            copy_thick_surface(nghbr, *org);
        }
    }
  //  std::cout << "pN: " << probN << " (" << part1N << " + " << part2N << ")";
  //  std::cout << "\npS: " << probS << " (" << part1S << " + " << part2S << ")\n"  << std::endl;
}
//
// void Optimizer::init_GA(const unsigned ps, const double ep, const double mp, const double rhoe, const unsigned K, const unsigned MAXT, const unsigned x_intvl, const unsigned x_number, const unsigned max_gens)
// {
//     const long unsigned rngSeed = 0;
//     const unsigned n = org->outer.nNodes * 2;	// size of chromosomes
//     X_INTVL = x_intvl;
//     X_NUMBER = x_number;
//     MAX_GENS = max_gens;
//     algorithm = new BRKGA< SurfaceDecoder, MTRand >(n, ps, ep, mp, rhoe, *dec, *rng, K, MAXT);
//
// }
//
// // Wrapper around
// void Optimizer::evolve_ga(bool time)
// {
//     int generation = 0;
//     do {
//         struct timeval tvalBefore;
//         if (time) // Only calculate time if bool is set
//             time_b(tvalBefore);
//
//         step_ga();	// evolve the population for one generation
//
//         if (time) //
//             printf("gen %d:\n Time: %f seconds\n",generation, time_a(tvalBefore));
//
//         if((++generation) % X_INTVL == 0) {     // if we're at one of the appropriate intervals
//             algorithm->exchangeElite(X_NUMBER);	// exchange top individuals
//         }
//     } while (generation < MAX_GENS);
//     bestSolution = algorithm->getBestChromosome();
// }
//
// void Optimizer::step_ga()
// {
//     algorithm->evolve();	// evolve the population for one generation
// }
//
//
// void Optimizer::update_surface_ga(std::vector<double> &sol)
// {
//     int count = 0;
//     point_t acc(0,0);
//     // Apply offsets to surface
//     for (ListDigraph::NodeIt no(org->outer.graph); no != INVALID; ++no)
//     {
//         (*org->outer.coords)[no].x += sol[count];
//         (*org->outer.coords)[no].y += sol[count + sol.size() * 0.5];
//         count++;
//         acc = acc + (*org->outer.coords)[no];
//     }
//     acc = acc * (1/(double)org->outer.nNodes);
//
//     // Reset inner part of surface
//     Interfacer::generate_inner_s(org->inner, org->outer, org->thickness);
//  //   Interfacer::update_inner_s(org->inner, org->outer, org->thickness);
//     // Reset bridges between outer and inner surfaces
//     Interfacer::update_bridges(*org);
//
//     // Shift points to origin
//     for (ListDigraph::NodeIt no(org->outer.graph); no != INVALID; ++no)
//     {
//         (*org->outer.coords)[no].x -= acc.x;
//         (*org->outer.coords)[no].y -= acc.y;
//         (*org->inner.coords)[no].x -= acc.x;
//         (*org->inner.coords)[no].y -= acc.y;
//     }
//     // Shift "bridges" to origin
//     for (ListDigraph::NodeIt no(org->bridges.graph); no != INVALID; ++no)
//     {
//         (*org->bridges.coords)[no].x -= acc.x;
//         (*org->bridges.coords)[no].y -= acc.y;
//     }
// }

// Wrapper around method to find intersection
void Optimizer::find_intersections(std::vector<point_t> &is)
{
    std::vector<SurfaceData_t*> surfaces;
    surfaces.push_back(&org->outer);
    surfaces.push_back(&org->inner);
    surfaces.push_back(&org->bridges);
 //   std::cout << "Num ints: " << find_surface_intersections(surfaces, is) << std::endl;
}

void Optimizer::neighbor(ThickSurface_t &original, ThickSurface_t &n)
{
    copy_surface(original.outer, n.outer);
    std::vector<int> randomIndexes;

    double flip = 0.0;
    do{
        int randomIndex = rand() % n.outer.nNodes;
        randomIndexes.push_back(randomIndex);
        flip = static_cast<double>( rand() ) / static_cast<double>(RAND_MAX);
    } while (flip < this->multiProb);

    for (size_t i = 0; i < randomIndexes.size(); i++)
    {
        int randomIndex = randomIndexes[i];

        SNode randomNode        = n.outer.graph.nodeFromId(randomIndex);
        SNode randomInnerNode   = original.inner.graph.nodeFromId(n.outer.correspondence[randomIndex]);

        double offsetX, offsetY;
        offsetX = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX) * forceOffsetRange - (forceOffsetRange/2);
        offsetY = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX) * forceOffsetRange - (forceOffsetRange/2);
     //   std::cout << "Offsets: x " << offsetX << ", y: " << offsetY << std::endl;

        point_t dir(offsetX, offsetY);

        point_t pdir = (*n.outer.coords)[randomNode] + dir;
        point_t mdir = (*n.outer.coords)[randomNode] - dir;

        (*n.outer.coords)[randomNode] = pdir;

        // We now have the offset of the neighbor in relation to the current state. If we want to know whether
        // this particular force has stretched or compressed the surface, which will determine whether we multiply
        // thickness by a value greater or less than than 1, we have to know if the offset point in the outer
        // surface is closer to or further away from the original's corresponding inner point. This would be easily
        // determined by looking at both polygons' areas, but since we're not in that realm yet (it belongs to the
        // probability function) we can just compare the distance of the offset point if the offset were in the opposite
        // direction.


        n.thickness             = original.thickness; // What matter is avg thickness
        double distPdir         = dist(pdir, (*original.inner.coords)[randomInnerNode]);
        double distMdir         = dist(mdir, (*original.inner.coords)[randomInnerNode]);

        // If the distance to the inner node is now larger, then the surface will be stretched, otherwise itll be compressed
        double thicknessDiff = n.thickness[randomIndex]; // Useful for the smoothness routine
        n.thickness[randomIndex] *= (distPdir > distMdir ? compression : 1/compression);
        thicknessDiff -= n.thickness[randomIndex];
        // Routine to smooth out neighbour's relationship to current state
        // --------------
        SNode prev, next; int u = smooth; int prevIndex, nextIndex;
        prev = next = randomNode;
        for (int c = 1; c < u; c++)
        {
            prev = n.outer.graph.source(ListDigraph::InArcIt(n.outer.graph, prev));
            next = n.outer.graph.target(ListDigraph::OutArcIt(n.outer.graph, next));
            prevIndex = n.outer.graph.id(prev);
            nextIndex = n.outer.graph.id(next);

            float ratio = (float)(u - c)/(u);
            (*n.outer.coords)[prev].x += dir.x * ratio;
            (*n.outer.coords)[prev].y += dir.y * ratio;
            n.thickness[prevIndex]    -= thicknessDiff * ratio;
            (*n.outer.coords)[next].x += dir.x * ratio;
            (*n.outer.coords)[next].y += dir.y * ratio;
            n.thickness[nextIndex]    -= thicknessDiff * ratio;

        }
    }

    Interfacer::generate_inner_s(n.inner, n.outer, n.thickness);
    Interfacer::generate_bridges(n);

    ln = &n; // Update reference to last neighbour calculated
}

double Optimizer::probability(ThickSurface_t &s, double a0, double &p1, double &p2)
{
    double res;
    white = calculate_surface_area(s.inner, perimeter);
    gray = absol(calculate_surface_area(s.outer, perimeter) - white);
 //   std::cout << "gray: " << gray;
 //   std::cout << " a0 = " << a0 << std::endl;
    stretch = absol(a0 - gray);
    p1 = pow(calculate_surface_area(s.outer, perimeter), areaPow);
    p2 = pow((stretch + 1), diffPow); // Stretch is the difference; therefore p2 is raised to the right power here
                                      // We add 1 before raising to ensure the growth is not sublinear from the get-go
  //  std::cout << "stretch: " << stretch << "; " << diffMul << " * diff ^ " << diffPow << ": " << p2 << std::endl;

    res = areaMul * p1 + diffMul * p2;

    std::vector<SurfaceData_t*> surfaces;
    std::vector<point_t> inters;

    surfaces.push_back(&s.outer);
    surfaces.push_back(&s.inner);
    if (innerCircle != NULL)
        surfaces.push_back(&innerCircle->outer);
    if (outerCircle != NULL)
        surfaces.push_back(&outerCircle->outer);

    int intsOuter = find_surface_intersections(surfaces, inters);
    res += 100000 * intsOuter;  // Intersection prevention
    energy = res;
    return res;
}

void Optimizer::evolve_sa(int kMax, bool time)
{
    struct timeval timer;
    for (int k = 0; k < kMax; k++)
    {
        time_b(timer);
        step_sa();
        gen = k;
        std::cout << "K = " << k << " and time: " << time_a(timer) << std::endl;

    }
    copy_thick_surface(state, *org);

    shift_to_origin(*org);
}
