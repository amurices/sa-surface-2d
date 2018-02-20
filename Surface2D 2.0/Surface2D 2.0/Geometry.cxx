#include "Geometry.h"

double Geometry::findNorm(const point_t &a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

point_t Geometry::findDirectionVector(const point_t &a, const point_t &b, const point_t &c, direction_t Type)
{

	point_t directionOffset;
	if (Type == ORTHOGONAL)
	{
		// Vd = P(n+1) - P(n-1)
		directionOffset = a - b;

		// variável auxiliar invX
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

		// variável auxiliar invX
		double invX = dO.x;

		// Vd = (-Vd.y ,Vd.x)
		dO.x = -dO.y;
		dO.y = invX;

		// Vd = Vd / Norm(Vd) (vetor retornado é unitário)
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

		// variável auxiliar invX
		double invX = dO.x;

		// Vd = (-Vd.y ,Vd.x)
		dO.x = -dO.y;
		dO.y = invX;

		// Vd = Vd / Norm(Vd) (vetor retornado é unitário)
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