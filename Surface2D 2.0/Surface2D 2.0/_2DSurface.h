#pragma once
#include "defs.h"
#include "Geometry.h"

class _2DSurface
{
public:
	// NOTE: Memory management has be done explicitly and perfectly when handling objects
	// directly through pointers. However, this makes for much cleaner code.

	// NodeMap mapping coordinates (type point_t) to SNodes
	Coords_t    *coords;

	// Lemon::ListDigraph contains all nodes and the directed connections between them.
	ListDigraph *graph;

	std::map<int, int> correspondence;

	int nEdges;
	int nNodes;

	/* 
	 * Generates a number of points around a center, arranged in a circular manner.
	 * 
	 * @param radius the radius of the circular graph to be built.
	 * @param pts the number of points of the graph.
	 * @param center the 2D center of the circle.
	 */
	void generateCircularSurface(double radius, int pts, point_t center = point_t(0.0, 0.0));

	/*
	 * Observing a reference to an outer surface, generates an inner one.
	 *
	 * @param outerSurf the reference to the outer surface to which this will be an inner one.
     * @param thicknesses vector of "thicknesses" (distance between inner and outer nodes at multiple indices).
	 */
	void generateInnerSurface(_2DSurface &outerSurf, const std::vector<double> &thicknesses);

	/*
	 * Observing a reference to an outer surface, given a set of altered notes, updates this inner surface.
	 *
	 * @param outerSurf the reference to the outer surface to which this IS an inner one.
	 * @param changedNodes set of all the nodes whose correspondent inner ones should be updated.
	 * @param thicknesses vector of "thicknesses" (distance between inner and outer nodes at multiple indices).
	 */
	void updateInnerSurface(_2DSurface &outerSurf, const std::set<SNode> &changedNodes, const std::vector<double> &thicknesses);

	_2DSurface();

	/* Copy constructor.
	 * 
	 * Traverses the copied surface's graph in order of node ids; using LEMON's API of course. Note that this depends
	 * on nodes' ids remaining strictly ordered always; independent of nodes that are deleted or added later. Since
	 * the graph is currently only built once, this isn't a problem, but adding/deleting nodes mid-simulation could break it.
	 */
	_2DSurface(const _2DSurface &copied);

	/* Destructor.
	 *
	 * Clears the graph using both the delete operator and LEMON's provided .clear() method.
	 */
	~_2DSurface();
};

