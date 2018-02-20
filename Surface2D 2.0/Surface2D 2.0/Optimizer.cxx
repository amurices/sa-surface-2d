#include "stdafx.h"
#include "Optimizer.h"


Optimizer::Optimizer()
{
}


Optimizer::~Optimizer()
{
}


// TODO: ADD CHANGED NODES TO SET AND UPDATE INNER SURFACE
ThickSurface* Optimizer::findNeighbor(ThickSurface &org)
{
	ThickSurface* newNeighbor = new ThickSurface(org);
	std::vector<int> randomIndexes;

	// First, choose indices that will be modified by random neighbor search
	double coinFlip = 0.0;
	do { // do {} while guarantees that at least one node is modified
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
		offsetX = static_cast<double>(rand()) / static_cast<double> (RAND_MAX) * forceOffsetRange - (forceOffsetRange / 2);
		offsetY = static_cast<double>(rand()) / static_cast<double> (RAND_MAX) * forceOffsetRange - (forceOffsetRange / 2);
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
		SNode prev, next; int u = smooth; int prevIndex, nextIndex;
		prev = next = randomNode;
		for (int c = 1; c < u; c++)
		{
			prev = newNeighbor->outer->graph->source(ListDigraph::InArcIt(*newNeighbor->outer->graph, prev));
			next = newNeighbor->outer->graph->target(ListDigraph::OutArcIt(*newNeighbor->outer->graph, next));
			prevIndex = newNeighbor->outer->graph->id(prev);
			nextIndex = newNeighbor->outer->graph->id(next);

			float ratio = (float)(u - c) / (u);
			(*newNeighbor->outer->coords)[prev].x += dir.x * ratio;
			(*newNeighbor->outer->coords)[prev].y += dir.y * ratio;
			newNeighbor->thicknesses[prevIndex] -= thicknessDiff * ratio;
			(*newNeighbor->outer->coords)[next].x += dir.x * ratio;
			(*newNeighbor->outer->coords)[next].y += dir.y * ratio;
			newNeighbor->thicknesses[nextIndex] -= thicknessDiff * ratio;

		}
	}
	return newNeighbor;
}


void Optimizer::step_sa()
{
/*	ThickSurface* neighbor = findNeighbor(*state);
	double part1N, part2N, part1S, part2S;
	double probN = probability(nghbr, a0, part1N, part2N);
	double probS = probability(*state, a0, part1S, part2S); // Our accessible scope always gets
															// its parameters from S if we run probability() on S after we do it on N
	if (probN < probS)
	{
		copy_thick_surface(nghbr, *state);
		copy_thick_surface(nghbr, *org);
	}
	else if (probN < 10)
	{
		double randProb = static_cast<double>(rand()) / static_cast<double> (RAND_MAX);
		double diffNS = absol(probN - probS);
		double prob = (1.0 / diffNS < 1.0 ? 1.0 / diffNS : 1.0) * ((double)maxT - (double)gen) / (double)maxT * tempProb;
		//     std::cout << "\n\nPROB: " << prob << "\n\n";
		if (prob > randProb)
		{
			copy_thick_surface(nghbr, *state);
			copy_thick_surface(nghbr, *org);
		}
	}*/
}