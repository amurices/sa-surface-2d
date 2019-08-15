#pragma once

#include <fstream>
#include "defs.hpp"
#include "ThickSurface.hpp"
#include "Util.hpp"
#include "_2DSurface.hpp"

class Optimizer
{
  public:
    bool changed = false;
    bool shouldStep = false;
    bool singleStep = false;
    std::set<NodeChange_t> neighborChanges;
    std::set<ThicknessChange_t> thicknessChanges;
    std::vector<_2DSurface*> constraintSurfaces; // Surfaces which org can't intersect with
    bool recording = false;
    std::ofstream outputFile;


    // -------------------
    // SA attributes a) To actually function ------
    // a.1) Hyperparameters - They alter the simulation's behavior
    InitSaParams* params;
    
    // a.2) References - What the simulation needs to see to run correctly
    void stepSimulatedAnnealing(ThickSurface *thickSurface, double *temperature, double a0);

    void findNeighbor(ThickSurface *thickSurface, std::set<NodeChange_t> *neighborChanges,
                      std::set<ThicknessChange_t> *neighborThicknessChanges);

    void applyChanges(ThickSurface *thickSurface, std::set<NodeChange_t> &changes, std::set<ThicknessChange_t> &thicknessChanges);
    void revertChanges(ThickSurface *thickSurface, std::set<NodeChange_t> &changes, std::set<ThicknessChange_t> &thicknessChanges);

    /* Given a state S, that when relaxed has energy a0, calculates its internal energy.
     *
     * @param s the state of which the internal energy will be calculated.
     * @param a0 the area of the initial (relaxed) state of the system.
     */
    double findEnergy(const ThickSurface *thickSurface, double a0);

    /* Given temperature T and energy values of state S, neighbor N, returns the probability (between 0 and 1) of moving from S to N.
     *
     * @param s the current state of the simulated annealing.
     * @param n some neighbor of s.
     * @param t the current temperature of the system.
     */
    double findProbability(double eS, double eN, double t);

    void commitToOutputFile(double eS, double eN, double t, double whiteS, double whiteN, bool changed);

    void reset();

    Optimizer();
    ~Optimizer();
};
