#include "stdafx.hpp"
#include "_2DSurface.hpp"

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
	delete this->graph;   // Interestingly, deleting the graph is still necessary after clearing.
	delete this->coords;
}

void _2DSurface::establishPartitions(int numHorzPartitions, int numVertPartitions)
{
	// Clear each set in the vector, then clear the vector
	for (size_t i = 0; i < this->parts.size(); i++)
	{
		this->parts[i].clear();
	}
	this->parts.clear();
	this->parts.resize(numHorzPartitions * numVertPartitions);

	SNode arcSource, arcTarget;
	for (ListDigraph::ArcIt it(*this->graph); it != INVALID; ++it)
	{
		arcSource = this->graph->source(it);
		arcTarget = this->graph->target(it);

		int srcPart = MathGeometry::findPartitionNumber((*this->coords)[arcSource], numHorzPartitions, numVertPartitions);
		int tgtPart = MathGeometry::findPartitionNumber((*this->coords)[arcTarget], numHorzPartitions, numVertPartitions);

		// After finding the partition for each of this arc's endpoints, add the arc to the set indexed by that part number.
		this->parts[srcPart].insert(it);
		this->parts[tgtPart].insert(it);
	}
}

void _2DSurface::generateCircularSurface(double radius, int pts, point_t center)
{
	// (1) Add one node to the graph,
	SNode fnode = this->graph->addNode();
	this->nNodes++;
	SNode prevToMap = fnode;
	SNode currToMap;
	// partitions[0] will contain each arc's source partition, [0] its target
	int partitions[2];
	//  and its coordinates to the map
	(*(this->coords))[prevToMap] = point_t(radius, 0) + center;

	for (size_t i = 1; i < pts; i++)
	{
		// Do (1) again, at the point resolution provided (number of pts)
		currToMap = this->graph->addNode();
		this->nNodes++;

		(*this->coords)[currToMap] = center + point_t(cos(i * (2 * PI) / pts) * radius, sin(i * (2 * PI) / pts) * radius);
		this->graph->addArc(prevToMap, currToMap);
		this->nEdges++;

		prevToMap = currToMap;
	}

	this->graph->addArc(prevToMap, fnode);
	this->nEdges++;
	prevToMap = currToMap;
}

void _2DSurface::smoothAdjacentNodes(SNode changedNode, point_t changedDifference, int smoothness, std::set<SNode> &changedSet, double (*func)(double u, double c))
{
	SNode prev, next;
	int u = smoothness;
	prev = next = changedNode;
	changedSet.insert(changedNode);

	for (int c = 1; c <= u; c++)
	{
		prev = this->graph->source(ListDigraph::InArcIt(*this->graph, prev));
		next = this->graph->target(ListDigraph::OutArcIt(*this->graph, next));

		// Previous and next nodes are also going to be altered as a matter of where they are in relation to their inner correspondents
		changedSet.insert(prev);
		changedSet.insert(next);

		double ratio = (*func)(u, c);
		(*this->coords)[prev].x += changedDifference.x * ratio;
		(*this->coords)[prev].y += changedDifference.y * ratio;
		(*this->coords)[next].x += changedDifference.x * ratio;
		(*this->coords)[next].y += changedDifference.y * ratio;
	}
	// The single node at the end and beginning of a set of changed smoothed nodes are also altered
	prev = this->graph->source(ListDigraph::InArcIt(*this->graph, prev));
	next = this->graph->target(ListDigraph::OutArcIt(*this->graph, next));

	changedSet.insert(prev);
	changedSet.insert(next);
}

void _2DSurface::smoothAdjacentNodesV2(SNode changedNode, point_t changedDifference, int smoothness, std::set<NodeChange_t> *changedSet, double (*func)(double u, double c))
{
	SNode prev, next;
	int u = smoothness;
	prev = next = changedNode;

	for (int c = 1; c <= u; c++)
	{
		prev = this->graph->source(ListDigraph::InArcIt(*this->graph, prev));
		next = this->graph->target(ListDigraph::OutArcIt(*this->graph, next));

		double ratio = (*func)(u, c);
		point_t coordsChange(changedDifference.x * ratio, changedDifference.y * ratio);

		// Previous and next nodes are also going to be altered as a matter of where they are in relation to their inner correspondents
		changedSet->insert(NodeChange_t(prev, coordsChange, this->graph));
		changedSet->insert(NodeChange_t(next, coordsChange, this->graph));
	}
	// The single node at the end and beginning of a set of changed smoothed nodes are also altered (updateInner has to see them)
	// Don't know if this is necessary, but for the sake of consistency, i'll add the redundant changes.
	prev = this->graph->source(ListDigraph::InArcIt(*this->graph, prev));
	next = this->graph->target(ListDigraph::OutArcIt(*this->graph, next));
	changedSet->insert(NodeChange_t(prev, point_t(0, 0), this->graph));
	changedSet->insert(NodeChange_t(next, point_t(0, 0), this->graph));
}

void _2DSurface::generateInnerSurface(_2DSurface &outerSurf, const std::vector<double> &thicknesses)
{
	SNode prev, next, last;
	point_t pPrev, pNext, pCurr, vd;

	// Here is where adding the first corresponding node in the inner surface starts.
	SNode fnode = outerSurf.graph->nodeFromId(0); // fnode = P0

	ListDigraph::InArcIt inCurrI(*outerSurf.graph, fnode);   // get Pn
	ListDigraph::OutArcIt outCurrI(*outerSurf.graph, fnode); // and P1

	prev = outerSurf.graph->source(inCurrI);  // cont ^
	next = outerSurf.graph->target(outCurrI); // cont ^

	pNext = (*outerSurf.coords)[next]; // get coordinates Pn
	pPrev = (*outerSurf.coords)[prev]; // and P0

	vd = MathGeometry::findDirectionVector(pPrev, pNext, (*outerSurf.coords)[fnode], MathGeometry::MEDIAN_ANGLE); // Get directional vector btwn inner & outer
	vd *= thicknesses[0];																						  // Directional vector returned by findDirectionVector is unitary

	SNode finode = this->graph->addNode();							   // Add node to this surface;
	SNode innerCurrToMap = finode;									   // Current node to map is first node
	this->nNodes++;													   // increment no. of nodes
	(*this->coords)[innerCurrToMap] = (*outerSurf.coords)[fnode] - vd; // set coordinates of inner node
	SNode innerPrevToMap = innerCurrToMap;							   // previous node to map is now the current node to map

	this->correspondence[this->graph->id(finode)] = outerSurf.graph->id(fnode);		// Set
	outerSurf.correspondence[outerSurf.graph->id(fnode)] = this->graph->id(finode); // Correspondences
	// After all that, we have the first node in the inner surface. We can count on it to build new nodes and edges between them.
	// ---------------------------------------------------------------------

	// Now do the same thing, but iteratively
	SNode curr;
	int count = 0;
	for (curr = next; curr != fnode; curr = next)
	{
		ListDigraph::OutArcIt outCurr(*outerSurf.graph, curr);
		ListDigraph::InArcIt inCurr(*outerSurf.graph, curr);

		next = outerSurf.graph->target(outCurr);
		prev = outerSurf.graph->source(inCurr);

		pNext = (*outerSurf.coords)[next];
		pPrev = (*outerSurf.coords)[prev];

		vd = MathGeometry::findDirectionVector(pPrev, pNext, (*outerSurf.coords)[curr], MathGeometry::MEDIAN_ANGLE);
		vd *= thicknesses[outerSurf.graph->id(curr)];

		innerCurrToMap = this->graph->addNode();
		this->nNodes++;
		(*this->coords)[innerCurrToMap] = (*outerSurf.coords)[curr] - vd;

		this->correspondence[this->graph->id(innerCurrToMap)] = outerSurf.graph->id(curr);	 // Set
		outerSurf.correspondence[outerSurf.graph->id(curr)] = this->graph->id(innerCurrToMap); // Correspondences

		this->graph->addArc(innerPrevToMap, innerCurrToMap);
		this->nEdges++;
		innerPrevToMap = innerCurrToMap;
	}
	this->graph->addArc(innerPrevToMap, finode);
	this->nEdges++;
}

void _2DSurface::updateInnerSurface(_2DSurface &outerSurf, const std::set<SNode> &changedNodes, const std::vector<double> &thicknesses)
{
	int count = 0;

	// TODO: Adapt so it adds the nodes to changes made.
	for (auto it = changedNodes.begin(); it != changedNodes.end(); it++)
	{
		count++;
		SNode prev, next, last;
		point_t pPrev, pNext, pCurr, vd;

		// Changed nodes have their correspondents updated one at a time
		SNode fnode = *it;
		int fnodeId = outerSurf.graph->id(fnode);

		ListDigraph::InArcIt inCurrI(*outerSurf.graph, fnode);   // get Pn
		ListDigraph::OutArcIt outCurrI(*outerSurf.graph, fnode); // and P1

		prev = outerSurf.graph->source(inCurrI);  // cont ^
		next = outerSurf.graph->target(outCurrI); // cont ^

		pNext = (*outerSurf.coords)[next]; // get coordinates Pn
		pPrev = (*outerSurf.coords)[prev]; // and P0

		vd = MathGeometry::findDirectionVector(pPrev, pNext, (*outerSurf.coords)[fnode], MathGeometry::MEDIAN_ANGLE); // Get directional vector btwn inner & outer
		vd *= thicknesses[outerSurf.graph->id(fnode)];																  // Directional vector returned by findDirectionVector is unitary

		// TODO: The following depends on the node IDs of inner and outer surfaces to ALWAYS REMAIN THE SAME.
		// Since IDs are calculated at the moment they're added to a graph, this shouldn't be an issue, since we always
		// traverse the surfaces in the same order (and will do the same when adding nodes). Still, it's worth noting.

		SNode finode = this->graph->nodeFromId(fnodeId);				   // Add node to this surface;
		SNode innerCurrToMap = finode;									   // Current node to map is first node
		(*this->coords)[innerCurrToMap] = (*outerSurf.coords)[fnode] - vd; // set coordinates of inner node
	}
}
void _2DSurface::updateInnerSurfaceV2(_2DSurface &outerSurf, const std::set<SNode> &changedNodes, const std::vector<double> &thicknesses,
									  std::set<NodeChange_t> *nodeChanges, std::set<ThicknessChange_t> *thicknessChanges)
{
	int count = 0;

	for (auto it = changedNodes.begin(); it != changedNodes.end(); it++)
	{
		count++;
		SNode prev, next, last;
		point_t pPrev, pNext, pCurr, vd;

		// Changed nodes have their correspondents updated one at a time
		SNode fnode = *it;
		int fnodeId = outerSurf.graph->id(fnode);

		ListDigraph::InArcIt inCurrI(*outerSurf.graph, fnode);   // get Pn
		ListDigraph::OutArcIt outCurrI(*outerSurf.graph, fnode); // and P1

		prev = outerSurf.graph->source(inCurrI);  // cont ^
		next = outerSurf.graph->target(outCurrI); // cont ^

		pNext = (*outerSurf.coords)[next]; // get coordinates Pn
		pPrev = (*outerSurf.coords)[prev]; // and P0

		vd = MathGeometry::findDirectionVector(pPrev, pNext, (*outerSurf.coords)[fnode], MathGeometry::MEDIAN_ANGLE); // Get directional vector btwn inner & outer
		vd *= thicknesses[outerSurf.graph->id(fnode)];																  // Directional vector returned by findDirectionVector is unitary

		// TODO: The following depends on the node IDs of inner and outer surfaces to ALWAYS REMAIN THE SAME.
		// Since IDs are calculated at the moment they're added to a graph, this shouldn't be an issue, since we always
		// traverse the surfaces in the same order (and will do the same when adding nodes). Still, it's worth noting.

		SNode finode = this->graph->nodeFromId(fnodeId); // Add node to this surface;
		SNode innerCurrToMap = finode;					 // Current node to map is first node
		nodeChanges->insert(
			NodeChange_t(
				innerCurrToMap,													   // node where the change is applied
				(*outerSurf.coords)[fnode] - vd - (*this->coords)[innerCurrToMap], // the difference (if we add this node's coords we'll get it back)
				this->graph));													   // inner node's graph
		(*this->coords)[innerCurrToMap] = (*outerSurf.coords)[fnode] - vd;		   // set coordinates of inner node
	}
}

double _2DSurface::findSurfaceAreaAndPerimeter(double &perim)
{
	double area = 0;
	perim = 0;

	SNode fnode, prev, next, no;
	fnode = this->graph->nodeFromId(0);
	no = fnode;
	do
	{
		ListDigraph::InArcIt inCurrI(*this->graph, no);
		ListDigraph::OutArcIt outCurrI(*this->graph, no);

		prev = this->graph->source(inCurrI);
		next = this->graph->target(outCurrI);

		// Perimeter calculation: -----------------------------------------------------
		point_t perimVector = (*this->coords)[no] - (*this->coords)[prev];
		double perimVectorNorm = MathGeometry::findNorm(perimVector);
		perim += perimVectorNorm;
		// End of perimeter calculation -----------------------------------------------

		area += (*this->coords)[no].x * ((*this->coords)[next].y - (*this->coords)[prev].y);
		no = next;
	} while (no != fnode);

	area /= 2;

	if (area < 0)
		area = 0;

	return area;
}