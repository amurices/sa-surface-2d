#pragma once
#include "defs.h"


namespace Geometry {
	enum direction_t { ORTHOGONAL, MEDIAN, MEDIAN_ANGLE };

	double findNorm(const point_t &a);
	point_t findDirectionVector(const point_t &a, const point_t &b, const point_t &c, direction_t Type);
}