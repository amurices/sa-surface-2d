#include "stdafx.hpp"
#include "Optimizer.hpp"

Optimizer::Optimizer()
{
}

Optimizer::~Optimizer()
{
}

// Done. But it's behaving weirdly. Find out why!
ThickSurface *Optimizer::findNeighbor(ThickSurface &org)
{
	ThickSurface *newNeighbor = new ThickSurface(org);
	std::vector<int> randomIndexes;
	std::set<SNode> changedNodes;

	// First, choose indices that will be modified by random neighbor search
	double coinFlip = 0.0;
	do
	{ // do {} while guarantees that at least one node is modified
		int randomIndex = rand() % newNeighbor->outer->nNodes;
		randomIndexes.push_back(randomIndex);
		coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	} while (coinFlip < this->multiProb);

	for (size_t i = 0; i < randomIndexes.size(); i++)
	{
		int randomIndex = randomIndexes[i];

		SNode randomNode = newNeighbor->outer->graph->nodeFromId(randomIndex);
		SNode randomInnerNode = org.inner->graph->nodeFromId(newNeighbor->outer->correspondence[randomIndex]);

		// Choose a random offset, bounded by optimizer params
		double offsetX, offsetY;
		offsetX = static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * forceOffsetRange - (forceOffsetRange / 2);
		offsetY = static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * forceOffsetRange - (forceOffsetRange / 2);
		point_t dir(offsetX, offsetY);

		point_t pdir = (*newNeighbor->outer->coords)[randomNode] + dir;
		point_t mdir = (*newNeighbor->outer->coords)[randomNode] - dir;

		(*newNeighbor->outer->coords)[randomNode] = pdir;

		// We now have the offset of the neighbor in relation to the current state. If we want to know whether
		// this particular force has stretched or compressed the surface, which will determine whether we multiply
		// thickness by a value greater or less than than 1, we have to know if the offset point in the outer
		// surface is closer to or further away from the original's corresponding inner point. This would be easily
		// determined by looking at both polygons' areas, but since we're not in that realm yet (it belongs to the
		// probability function) we can just compare the distance of the offset point if the offset were in the opposite
		// direction.

		double distPdir = Util::dist(pdir, (*org.inner->coords)[randomInnerNode]);
		double distMdir = Util::dist(mdir, (*org.inner->coords)[randomInnerNode]);

		// If the distance to the inner node is now larger, then the surface will be stretched, otherwise itll be compressed
		double thicknessDiff = newNeighbor->thicknesses[randomIndex]; // Useful for the smoothness routine
		newNeighbor->thicknesses[randomIndex] *= (distPdir > distMdir ? this->compression : 1 / this->compression);
		thicknessDiff -= newNeighbor->thicknesses[randomIndex];

		// Routine to smooth out neighbour's relationship to current state
		// --------------
		newNeighbor->outer->smoothAdjacentNodes(randomNode, dir, this->smooth, changedNodes, &MathGeometry::linearSmooth);
	}
	newNeighbor->updateInnerSurface(changedNodes);

	return newNeighbor;
}

double Optimizer::findEnergy(const ThickSurface &s, double a0)
{
	double res;

	double whiteMatterArea, whiteMatterPerimeter;
	double grayMatterArea, grayMatterPerimeter;

	// White matter area is just the inner surface's area
	whiteMatterArea = s.inner->findSurfaceAreaAndPerimeter(whiteMatterPerimeter);
	// Gray matter area is the difference between the outer surface and the inner surface
	grayMatterArea = s.outer->findSurfaceAreaAndPerimeter(grayMatterPerimeter) - whiteMatterArea;
	// Area must be non-negative
	grayMatterArea = MathGeometry::absol(grayMatterArea);

	// Difference between initial and current gray matter area.
	double grayMatterStretch = MathGeometry::absol(a0 - grayMatterArea);

	double p1, p2;
	p1 = pow(whiteMatterArea, this->areaPow);
	p2 = pow(grayMatterStretch + 1, this->diffPow); // Stretch is the difference; therefore p2 is raised to the right power here
													// We add 1 before raising to ensure the growth is not sublinear from the get-go

	// Simplified energy equation of 2D closed surface
	res = areaMul * p1 + diffMul * p2;
	return res;
}

double Optimizer::findProbability(double eS, double eN, double t)
{
	if (eN < eS)
		return 1;

	else
		return exp((eS - eN) / t);
}

void Optimizer::step_sa(ThickSurface &state, double temperature, double a0)
{
	ThickSurface *neighbor = findNeighbor(state);

	double eS = findEnergy(state, a0);
	double eN = findEnergy(*neighbor, a0);

	double prob = findProbability(eS, eN, temperature);
	double coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);

	if (coinFlip < prob)
	{
		state = *neighbor;
		delete neighbor;
	}
}