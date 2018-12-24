#include "stdafx.h"
#include "Util.h"

double Util::dist(point_t p1, point_t p2)
{
	std::pair<double, double> norm = std::make_pair(p2.x - p1.x, p2.y - p1.y);
	return sqrt(norm.first * norm.first + norm.second * norm.second);
}


bool Util::ltTolerance(double a, double b, double tol)
{
	return (a < (b - tol));
}
bool Util::gtTolerance(double a, double b, double tol)
{
	return (a >(b + tol));
}

bool Util::eqTolerance(double a, double b, double tol)
{
	return (a > b - tol) && (a < b + tol);
}

double Util::absol(double x)
{
	if (x < 0)
		return -x;
	return x;
}

