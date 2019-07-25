#pragma once
#include "defs.hpp"
namespace Util
{
double pointsDistance(point_t p1, point_t p2);

bool ltTolerance(double a, double b, double tol);
bool gtTolerance(double a, double b, double tol);
bool eqTolerance(double a, double b, double tol);

double absol(double x);

double getRandomRange(double lower, double upper);

void randColors(std::vector<triple_t> &colors, int numColors);

void time_before();
double time_after();
}; // namespace Util
