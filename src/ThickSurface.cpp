#include "stdafx.hpp"
#include "ThickSurface.hpp"

ThickSurface::ThickSurface()
{
}

ThickSurface::ThickSurface(const ThickSurface &copied)
{
	this->outer = new _2DSurface(*copied.outer);
	this->outer->establishPartitions(2, 2);
	this->inner = new _2DSurface(*copied.inner);
	this->inner->establishPartitions(2, 2);
	this->thicknesses = copied.thicknesses;
}

void ThickSurface::operator=(const ThickSurface &p)
{
	delete this->outer;
	delete this->inner;
	this->thicknesses.clear();
	// TODO: What the fuck is this lmao
	this->outer = new _2DSurface(*p.outer);
	this->outer->establishPartitions(3, 3);
	this->inner = new _2DSurface(*p.inner);
	this->inner->establishPartitions(3, 3);
	this->thicknesses = p.thicknesses;
}

ThickSurface::~ThickSurface()
{
	delete this->outer;
	delete this->inner;

	this->thicknesses.clear();
}

void ThickSurface::updateInnerSurface(const std::set<SNode> &changedNodes)
{
	this->inner->updateInnerSurface(*this->outer, changedNodes, this->thicknesses);
}
void ThickSurface::updateInnerSurfaceV2(std::set<NodeChange_t> *nodeChanges)
{
	// TODO: Pass changes so updateInnerSurface can add what it does to the thick surface.
	this->inner->updateInnerSurfaceV2(*this->outer, this->thicknesses, nodeChanges);
}

void ThickSurface::smoothAdjacentThicknesses(double thicknessDiff, int smoothness, SNode changedNode, std::set<ThicknessChange_t> *changedSet, double (*func)(double u, double c)){
	SNode prev, next;
	int u = smoothness;
	prev = next = changedNode;
	for (int c = 1; c <= u; c++)
	{
		printf("yo %d\n", c);

		prev = this->outer->graph->source(ListDigraph::InArcIt(*this->outer->graph, prev));
		next = this->outer->graph->target(ListDigraph::OutArcIt(*this->outer->graph, next));

		printf("yo a\n");

		double ratio = (*func)(u, c);
		double thicknessChange = thicknessDiff * ratio;

		// Previous and next nodes are also going to be altered as a matter of where they are in relation to their inner correspondents
		changedSet->insert(ThicknessChange_t(this->outer->graph->id(prev), thicknessChange));
		printf("yo b\n");

		changedSet->insert(ThicknessChange_t(this->outer->graph->id(next), thicknessChange));
		printf("yo\n");
	}
	// The single node at the end and beginning of a set of changed smoothed nodes are also altered (updateInner has to see them)
	// Don't know if this is necessary, but for the sake of consistency, i'll add the redundant changes.
	prev = this->outer->graph->source(ListDigraph::InArcIt(*this->outer->graph, prev));
	next = this->outer->graph->target(ListDigraph::OutArcIt(*this->outer->graph, next));
	changedSet->insert(ThicknessChange_t(this->outer->graph->id(prev), 0.0));
	changedSet->insert(ThicknessChange_t(this->outer->graph->id(next), 0.0));
}

void ThickSurface::generateRandomThicknesses(int pts, double upperPercentOfRadius, double lowerPercentOfRadius)
{
	// If thicknesses was already populated
	this->thicknesses.clear();
	for (int i = 0; i < pts; i++)
	{
		double toPush = (double)rand() / RAND_MAX;
		toPush *= upperPercentOfRadius - lowerPercentOfRadius;
		toPush += lowerPercentOfRadius;
		this->thicknesses.push_back(toPush);
	}
}

void ThickSurface::generateCircularThickSurface(double radius, int pts, bool randomThicknesses, double ub, double lb, point_t center, int horzPartsNumber, int vertPartsNumber)
{
	// If this boolean variable is not set, we'll assume thicknesses to have been set elsewhere
	if (randomThicknesses)
		this->generateRandomThicknesses(pts, ub, lb);
	else if (thicknesses.empty())
	{
		throw std::exception();
	}
	this->outer = new _2DSurface();
	this->outer->generateCircularSurface(radius, pts, center);
	this->outer->establishPartitions(horzPartsNumber, vertPartsNumber);

	this->inner = new _2DSurface();
	this->inner->generateInnerSurface(*this->outer, this->thicknesses);
	this->inner->establishPartitions(horzPartsNumber, vertPartsNumber);
}
