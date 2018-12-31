#include "stdafx.hpp"
#include "Util.hpp"

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
	return (a > (b + tol));
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

double Util::getRandomRange(double lower, double upper)
{
	return lower + (upper - lower) * (double)rand() / RAND_MAX;
}

void Util::randColors(std::vector<triple_t> &colors, int numColors)
{
	colors.clear();
	for (int i = 0; i < numColors; i++)
	{
		colors.emplace_back(getRandomRange(0.0, 1.0), getRandomRange(0.0, 1.0), getRandomRange(0.0, 1.0));
	}
}
