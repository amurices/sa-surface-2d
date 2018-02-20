#include "stdafx.h"
#include "_2DSurface.h"


_2DSurface::_2DSurface()
{
	this->nEdges = 0;
	this->nNodes = 0;

	this->graph = new ListDigraph();
	this->coords = new Coords_t(*graph);
}

_2DSurface::_2DSurface(const _2DSurface &copied)
{
	if (copied.nNodes < 1) // TODO: Not sure if this is legal
		return;

	this->nEdges = copied.nEdges;
	this->nNodes = copied.nNodes;

	this->graph = new ListDigraph();
	this->coords = new Coords_t(*graph);

	if (this->nNodes < 1) // TODO: Not sure if this is legal
		return;

	// (1) Add one node to the graph,
	SNode fnode = this->graph->addNode();
	SNode prevToMap = fnode;
	SNode currToMap;
	//  and its coordinates to the map, equal to the coordinates of the first node added to the copied object
	(*(this->coords))[prevToMap] = (*copied.coords)[copied.graph->nodeFromId(0)];

	for (size_t i = 1; i < this->nNodes; i++)
	{
		// Do (1) again, adding nodes in the same order. 
		// This counts, of course, on LEMON attributing node id's according to the order in which they're added.
		// TODO: When adding nodes mid-simulation, this could lead to problems. We'll see
		currToMap = this->graph->addNode();

		// This assures that the copy surface has even the ids on its nodes identical to original
		(*this->coords)[currToMap] = (*copied.coords)[copied.graph->nodeFromId(i)];
		this->graph->addArc(prevToMap, currToMap);

		prevToMap = currToMap;
	}

	this->graph->addArc(prevToMap, fnode);
	prevToMap = currToMap;
}


_2DSurface::~_2DSurface()
{
	this->nEdges = 0;
	this->nNodes = 0;

	this->graph->clear(); // Amusing, but it seems clearing the graph is not necessary.
	delete this->graph; // Interestingly, deleting the graph is still necessary after clearing.
	delete this->coords;
}

void _2DSurface::generateCircularSurface(double radius, int pts, point_t center)
{
	// (1) Add one node to the graph,
	SNode fnode = this->graph->addNode(); this->nNodes++;
	SNode prevToMap = fnode;
	SNode currToMap;
	//  and its coordinates to the map
	(*(this->coords))[prevToMap] = point_t(radius, 0) + center;

	for (size_t i = 1; i < pts; i++)
	{
		// Do (1) again, at the point resolution provided (number of pts)
		currToMap = this->graph->addNode(); this->nNodes++;
		 
		(*this->coords)[currToMap] = center + point_t(cos(i * (2 * PI) / pts) * radius, sin(i * (2 * PI) / pts) * radius);
		this->graph->addArc(prevToMap, currToMap); this->nEdges++;
		
		prevToMap = currToMap;
	}

	this->graph->addArc(prevToMap, fnode); this->nEdges++;
	prevToMap = currToMap;
}

void _2DSurface::generateInnerSurface(_2DSurface &outerSurf, const std::vector<double> &thicknesses)
{
	SNode	prev, next, last;
	point_t	pPrev, pNext, pCurr, vd;

	// Here is where adding the first corresponding node in the inner surface starts.
	SNode fnode = outerSurf.graph->nodeFromId(0);				// fnode = P0

	ListDigraph::InArcIt	inCurrI(*outerSurf.graph, fnode);		// get Pn
	ListDigraph::OutArcIt	outCurrI(*outerSurf.graph, fnode);	// and P1

	prev = outerSurf.graph->source(inCurrI);		// cont ^
	next = outerSurf.graph->target(outCurrI);		// cont ^

	pNext = (*outerSurf.coords)[next];			// get coordinates Pn
	pPrev = (*outerSurf.coords)[prev];			// and P0

	vd = Geometry::findDirectionVector(pPrev, pNext, (*outerSurf.coords)[fnode], Geometry::MEDIAN_ANGLE);	// Get directional vector btwn inner & outer
	vd *= thicknesses[0];													// Directional vector returned by findDirectionVector is unitary

	SNode finode = this->graph->addNode();	// Add node to this surface;
	SNode innerCurrToMap = finode;			// Current node to map is first node
	this->nNodes++;							// increment no. of nodes
	(*this->coords)[innerCurrToMap] = (*outerSurf.coords)[fnode] - vd;	// set coordinates of inner node
	SNode innerPrevToMap = innerCurrToMap;							// previous node to map is now the current node to map

	this->correspondence[this->graph->id(finode)]			= outerSurf.graph->id(fnode);	// Set
	outerSurf.correspondence[outerSurf.graph->id(fnode)]				= this->graph->id(finode);		// Correspondences
	// After all that, we have the first node in the inner surface. We can count on it to build new nodes and edges between them.
	// ---------------------------------------------------------------------

	// Now do the same thing, but iteratively
	SNode curr; int count = 0;
	for (curr = next; curr != fnode; curr = next)
	{
		ListDigraph::OutArcIt	outCurr(*outerSurf.graph, curr);
		ListDigraph::InArcIt	inCurr(*outerSurf.graph, curr);

		next = outerSurf.graph->target(outCurr);
		prev = outerSurf.graph->source(inCurr);

		pNext = (*outerSurf.coords)[next];
		pPrev = (*outerSurf.coords)[prev];

		vd = Geometry::findDirectionVector(pPrev, pNext, (*outerSurf.coords)[curr], Geometry::MEDIAN_ANGLE);
		vd *= thicknesses[outerSurf.graph->id(curr)];

		innerCurrToMap = this->graph->addNode();
		this->nNodes++;
		(*this->coords)[innerCurrToMap] = (*outerSurf.coords)[curr] - vd;

		this->correspondence[this->graph->id(innerCurrToMap)] = outerSurf.graph->id(curr);		// Set
		outerSurf.correspondence[outerSurf.graph->id(curr)] = this->graph->id(innerCurrToMap);		// Correspondences

		this->graph->addArc(innerPrevToMap, innerCurrToMap);
		this->nEdges++;
		innerPrevToMap = innerCurrToMap;
	}
	this->graph->addArc(innerPrevToMap, finode);
	this->nEdges++;

}

void _2DSurface::updateInnerSurface(_2DSurface &outerSurf, const std::set<SNode> &changedNodes, const std::vector<double> &thicknesses)
{
	for (auto it = changedNodes.begin(); it != changedNodes.end(); it++)
	{
		SNode	prev, next, last;
		point_t	pPrev, pNext, pCurr, vd;

		// Changed nodes have their correspondents updated one at a time
		SNode fnode = *it;
		int fnodeId = outerSurf.graph->id(fnode);

		ListDigraph::InArcIt	inCurrI(*outerSurf.graph, fnode);		// get Pn
		ListDigraph::OutArcIt	outCurrI(*outerSurf.graph, fnode);		// and P1

		prev = outerSurf.graph->source(inCurrI);		// cont ^
		next = outerSurf.graph->target(outCurrI);		// cont ^

		pNext = (*outerSurf.coords)[next];			// get coordinates Pn
		pPrev = (*outerSurf.coords)[prev];			// and P0

		vd = Geometry::findDirectionVector(pPrev, pNext, (*outerSurf.coords)[fnode], Geometry::MEDIAN_ANGLE);	// Get directional vector btwn inner & outer
		vd *= thicknesses[outerSurf.graph->id(fnode)];							// Directional vector returned by findDirectionVector is unitary

		SNode finode = this->graph->nodeFromId(outerSurf.correspondence[fnodeId]);	// Add node to this surface;
		SNode innerCurrToMap = finode;												// Current node to map is first node
		(*this->coords)[innerCurrToMap] = (*outerSurf.coords)[fnode] - vd;			// set coordinates of inner node
	}

}
