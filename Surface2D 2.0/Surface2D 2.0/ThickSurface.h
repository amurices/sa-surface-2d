#pragma once
#include "defs.h"
#include "_2DSurface.h"

class ThickSurface
{
public:
	// Reference to the outside of the surface
	_2DSurface *outer;
	// Reference to the inner part of the surface
	_2DSurface *inner;
	// Vector containing distance between corresponding outer and inner nodes at each index
	std::vector<double> thicknesses;

	/*
	 * Generates a random thicknesses vector.
	 * @param pts the amount of points the inner and outer surfaces contain.
	 * @param upperPercentOfRadius the upper limit of random thickness, in terms of % of outer radius.
	 * @param lowerPercentOfRadius the lower limit of random thickness, in terms of % of outer radius.
	 */
	void generateRandomThicknesses(int pts, double upperPercentOfRadius = 0.2, double lowerPercentOfRadius = 0.0);
	void generateCircularThickSurface(double radius, int pts, bool randomThicknesses = true, double ub = 0.2, double lb = 0.0, point_t center = point_t(0.0, 0.0));

	ThickSurface();
	ThickSurface(const ThickSurface &copied);

	~ThickSurface();

	void operator =(const ThickSurface &p);
};

