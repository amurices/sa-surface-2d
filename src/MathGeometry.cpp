#include "MathGeometry.hpp"

double MathGeometry::absol(double x)
{
	return (x > 0 ? x : -x);
}

double MathGeometry::findNorm(const point_t &a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

double MathGeometry::sineSmooth(double u, double c)
{
	return sin(0.5 * PI * (u - c) / u);
}

double MathGeometry::linearSmooth(double u, double c)
{
	return (u - c) / u;
}

double MathGeometry::inverseSmooth(double u, double c)
{
	return 1 / (c + 1);
}

static struct point_t* centroid;
// Return polar angle of p with respect to origin o
double to_angle(const struct point_t *p, const struct point_t *o) {
    return atan2(p->y - o->y, p->x - o->x);
}
void find_centroid(struct point_t *c, struct point_t *pts, int n_pts) {
    double x = 0, y = 0;
    for (int i = 0; i < n_pts; i++) {
        x += pts[i].x;
        y += pts[i].y;
    }
    c->x = x / n_pts;
    c->y = y / n_pts;
}
int by_polar_angle(const void *va, const void *vb) {
    double theta_a = to_angle((point_t*)va, centroid);
    double theta_b = to_angle((point_t*)vb, centroid);
    return theta_a < theta_b ? -1 : theta_a > theta_b ? 1 : 0;
}
void sort_by_polar_angle(struct point_t *pts, int n_pts) {
    find_centroid(centroid, pts, n_pts);
    qsort(pts, n_pts, sizeof pts[0], by_polar_angle);
}
double find_norm(const point_t &a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

// ---------------------------------------------
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the point_ts are collinear.
double cross(const point_t &A, const point_t &B, const point_t &O = point_t(0, 0))
{
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

double _2Dcross(const point_t &v, const point_t &w)
{
    return v.x * w.y - v.y * w.x;
}

int MathGeometry::findPartitionNumber(point_t p, int numHorzPartitions, int numVertPartitions)
{
#if defined(__APPLE__)
	p += point_t(1.0, 1.0);
	int partNumber = ((int)(p.y * numVertPartitions / 2.0) * numHorzPartitions + (int)(p.x * numHorzPartitions / 2.0));
	partNumber = partNumber < 0 ? 0 : partNumber; // TODO: This isn't ideal. Strictly vertical or horizontal out-of-boundness could be an issue
	partNumber = partNumber >= numHorzPartitions * numVertPartitions ? numHorzPartitions * numVertPartitions - 1 : partNumber;
	return partNumber;
#else
	return 0;
#endif
}

point_t MathGeometry::findDirectionVector(const point_t &a, const point_t &b, const point_t &c, direction_t Type)
{
	// TODO: All three types of direction vector are limited; there's always intersections.. On larger scale simulations, it's rarely an issue,
	// but I've basically exhausted my brain in trying to find a better solution that looks strictly at the two neighbors
	// (and I've found issues other than efficiency with solutions that look at more nodes, like the fact that jagged edges
	// would be weird in more holistic approaches). So I'm adding a routine that prevents intersection of inner nodes by
	// maybe pushing them outward, like collision resolution in game programming.
	point_t directionOffset;
	if (Type == ORTHOGONAL)
	{
		// Vd = P(n+1) - P(n-1)
		directionOffset = a - b;

		// vari�vel auxiliar invX
		double invX = directionOffset.x;

		// Vd = (-Vd.y ,Vd.x)
		directionOffset.x = -directionOffset.y;
		directionOffset.y = invX;

		// Vd = Vd / Norm(Vd) (return vector is unitary)
		double offsetNorm = findNorm(directionOffset);
		directionOffset.x /= offsetNorm;
		directionOffset.y /= offsetNorm;
	}

	else if (Type == MEDIAN)
	{
		// Vd = P(n+1) - P(n-1)
		point_t dO = a - b;

		// vari�vel auxiliar invX
		double invX = dO.x;

		// Vd = (-Vd.y ,Vd.x)
		dO.x = -dO.y;
		dO.y = invX;

		// Vd = Vd / Norm(Vd) (vetor retornado � unit�rio)
		double oN = findNorm(dO);
		dO.x /= oN;
		dO.y /= oN;

		// Shift a and b to the origin
		point_t aShift = a - c;
		point_t bShift = b - c;

		// Add both to get direction
		directionOffset = aShift + bShift;

		// Divide by norm
		double offsetNorm = findNorm(directionOffset);
		directionOffset.x /= offsetNorm;
		directionOffset.y /= offsetNorm;

		if (findNorm(dO - directionOffset) > findNorm(dO + directionOffset))
			directionOffset = directionOffset * (-1);
	}

	else if (Type == MEDIAN_ANGLE)
	{
		// Vd = P(n+1) - P(n-1)
		point_t dO = a - b;

		// vari�vel auxiliar invX
		double invX = dO.x;

		// Vd = (-Vd.y ,Vd.x)
		dO.x = -dO.y;
		dO.y = invX;

		// Vd = Vd / Norm(Vd) (vetor retornado � unit�rio)
		double oN = findNorm(dO);
		dO.x /= oN;
		dO.y /= oN;

		// Shift a and b to the origin
		point_t aShift = a - c;
		point_t bShift = b - c;

		// Normalize both
		double aNorm = findNorm(aShift);
		double bNorm = findNorm(bShift);
		aShift = aShift * (1 / aNorm);
		bShift = bShift * (1 / bNorm);

		// Add both to get direction
		directionOffset = aShift + bShift;
		directionOffset = directionOffset * 0.5;

		// Divide by norm
		double offsetNorm = findNorm(directionOffset);
		directionOffset.x /= offsetNorm;
		directionOffset.y /= offsetNorm;

		if (findNorm(dO - directionOffset) > findNorm(dO + directionOffset))
			directionOffset = directionOffset * (-1);
	}

	return directionOffset;
}
