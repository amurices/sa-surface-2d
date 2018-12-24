#pragma once
#include "defs.h"
namespace Util
{
double dist(point_t p1, point_t p2);

bool ltTolerance(double a, double b, double tol);
bool gtTolerance(double a, double b, double tol);
bool eqTolerance(double a, double b, double tol);

double absol(double x);

}; // namespace Util
