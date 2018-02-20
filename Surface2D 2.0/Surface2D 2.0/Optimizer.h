#pragma once
#include "defs.h"
#include "ThickSurface.h"
#include "Util.h"
#include "_2DSurface.h"

class Optimizer
{
public:
	ThickSurface  *org;                   // Reference to original surface
	_2DSurface  *innerCircle = NULL;	  // Inner and
	_2DSurface  *outerCircle = NULL;	  // outer shapes that constrain surface movement
	int gen;
	int maxT = 6500;

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
	ThickSurface *ln;   // Last neighbour calculated; useful for mid-evolution rendering
	ThickSurface *state;       // Local variables evolved state,
	ThickSurface nstate;      // new state,
	ThickSurface nghbr;       // evolved state and random neighbor

	// SA attributes b) For testing purposes - these are interesting to output
	double gray = 2000;         // Gray matter of any given solution (outer - inner areas)
	double white = 2000;        // White matter (inner area)
	double stretch = 2000;      // Stretch factor for any solution (diffMul * diff btwn grays ^ diffPow)
	double perimeter = 2000;    // Perimeter of any given solution (sum of all vector lengths)
	double energy = 2000;       // And fitness of solution ofc.

	// SA functions ------
	Optimizer(ThickSurface &org); //met constructor
	void init_SA(double scale = 1, int smooth = 0,
		double diffPow = 1, double diffMul = 1,
		double areaPow = 1, double areaMul = 1,
		double multiProb = 0, double forceOffsetRange = 0.066,
		double tempProb = 0, double compression = 1);
	void step_sa();
	double probability(ThickSurface &s);
	double temperature();
	ThickSurface* findNeighbor(ThickSurface &org);
	void evolve_sa(int kMax, bool time = true);


	Optimizer();
	~Optimizer();
};

