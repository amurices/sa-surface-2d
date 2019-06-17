#pragma once
#include "defs.hpp"
#include "ThickSurface.hpp"
#include "Util.hpp"
#include "_2DSurface.hpp"

class Optimizer
{
  public:
	ThickSurface *org;				// Reference to original surface
	_2DSurface *innerCircle = NULL; // Inner and
	_2DSurface *outerCircle = NULL; // outer shapes that constrain surface movement
	int gen;
	int maxT = 6500;
	bool changed;

	// -------------------
	// SA attributes a) To actually function ------
	// a.1) Hyperparameters - They alter the simulation's behavior
	InitSaParams* params;
	
	// a.2) References - What the simulation needs to see to run correctly
	// SA functions ------
	Optimizer(ThickSurface &org); //met constructor
	void init_SA(double scale = 1, int smooth = 0,
				 double diffPow = 1, double diffMul = 1,
				 double areaPow = 1, double areaMul = 1,
				 double multiProb = 0, double forceOffsetRange = 0.066,
				 double tempProb = 0, double compression = 1);
	void step_saV2(ThickSurface &state, double *temperature, double a0);

	void findNeighborV2(ThickSurface &org, std::set<NodeChange_t> *neighborChanges, std::set<ThicknessChange_t> *neighborThicknessChanges);

	void applyChanges(ThickSurface &thickSurface, std::set<NodeChange_t> &changes, std::set<ThicknessChange_t> &thicknessChanges);
	void revertChanges(ThickSurface &thickSurface, std::set<NodeChange_t> &changes, std::set<ThicknessChange_t> &thicknessChanges);

	/* Given a state S, that when relaxed has energy a0, calculates its internal energy.
	 *
	 * @param s the state of which the internal energy will be calculated.
	 * @param a0 the area of the initial (relaxed) state of the system.
	 */
	double findEnergy(const ThickSurface &s, double a0);

	/* Given temperature T and energy values of state S, neighbor N, returns the probability (between 0 and 1) of moving from S to N.
	 *
	 * @param s the current state of the simulated annealing.
	 * @param n some neighbor of s.
	 * @param t the current temperature of the system.
	 */
	double findProbability(double eS, double eN, double t);

	void evolve_sa(int kMax, bool time = true);

	Optimizer();
	Optimizer(InitSaParams* params);
	~Optimizer();
};
