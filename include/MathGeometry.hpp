#pragma once
#include "defs.hpp"

/* MathGeometry namespace containing auxiliary geometric functions.
 *
 * The MathGeometry namespace handles the spatial mathematics of the simulation.
 * It is made to encapsulate functions and enumerations that correspond to nothing but abstract
 * mathematical equations that describe behavior of vectors and discrete surfaces.
 */

namespace MathGeometry
{
enum direction_t
{
	ORTHOGONAL,
	MEDIAN,
	MEDIAN_ANGLE
};

double absol(double x);
double findNorm(const point_t &a);
point_t findDirectionVector(const point_t &a, const point_t &b, const point_t &c, direction_t Type = MEDIAN_ANGLE);
int findPartitionNumber(point_t p, int numHorzPartitions, int numVertPartitions);
double sineSmooth(double u, double c);
double linearSmooth(double u, double c);
double inverseSmooth(double u, double c);

} // namespace MathGeometry