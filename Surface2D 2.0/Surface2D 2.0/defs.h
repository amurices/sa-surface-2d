#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <exception>
#include <lemon/list_graph.h>

using namespace lemon;

const double TOLERANCE = 0.00000001;
const double PI = 3.14159265358979323846;


/* Some type definitions.
 *
 * Types utilized have operators overloaded when relevant, and when defined by
 * our program, they come with the suffix "_t".
 */
typedef struct triple_t {
	double _1;
	double _2;
	double _3;

	triple_t(double r, double g, double b) { _1 = r; _2 = g; _3 = b; }
} triple_t;

typedef struct point_t {
	double x;
	double y;

	point_t(double xn, double yn)
	{
		x = xn; y = yn;
	}
	point_t()
	{

	}
	bool operator <(const point_t &p) const {
		return x < p.x || (x == p.x && y < p.y);
	}

	bool operator ==(const point_t &p) const {
		return x == p.x && y == p.y;
	}

	point_t operator +(const point_t &p) const {
		return point_t(p.x + x, p.y + y);
	}
	void operator +=(const point_t &p) {
		x += p.x;
		y += p.y;
	}

	point_t operator -(const point_t &p) const {
		return point_t(x - p.x, y - p.y);
	}
	void operator -=(const point_t &p) {
		x -= p.x;
		y -= p.y;
	}

	point_t operator *(const double &a) const {
		return point_t(x*a, y*a);
	}

	void operator *= (const double &a) {
		x *= a;
		y *= a;
	}

}point_t;

typedef struct line_t {
	point_t p1;
	point_t p2;

	line_t(double x1n, double y1n, double x2n, double y2n)
	{
		p1.x = x1n; p2.x = x2n; p1.y = y1n; p2.y = y2n;
	}

	line_t(point_t p1n, point_t p2n)
	{
		p1 = p1n;
		p2 = p2n;
	}

	line_t()
	{

	}
} line_t;

typedef struct InitSaParams {
	// Surface params
	double radius;
	double thickness;
	// Optimizer params
	double scale;
	int smooth;
	double diffMul; 
	double diffPow;
	double areaPow; 
	double areaMul;
	double multiProb; 
	double forceOffsetRange;
	double temperature; 
	double compression;
} InitSaParams;


typedef ListDigraph::NodeMap<point_t> Coords_t; // _t for type
typedef ListDigraph::Node SNode;                // S for surface

/* 
 * Some pretty-printing operator overloads.
 */
inline std::ostream& operator<<(std::ostream&os, point_t const &p) {
	return os << "(" << p.x << ", " << p.y << ")";
}
inline std::ostream& operator<<(std::ostream&os, line_t const &p) {
	return os << "(" << p.p1.x << ", " << p.p1.y << ") -> " << "(" << p.p2.x << ", " << p.p2.y << ")";
}


typedef ListDigraph::NodeMap<point_t> Coords_t; // _t for type
typedef ListDigraph::Node SNode;                // S for surface
