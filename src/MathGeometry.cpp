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
