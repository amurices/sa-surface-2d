#include <cmath>
#include "MathGeometry.hpp"

#include "Util.hpp"

double MathGeometry::findNorm(const MathGeometry::point_t &a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

double MathGeometry::findNorm2d(double x, double y)
{
    return sqrt(x * x + y * y);
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

static struct MathGeometry::point_t* centroid;
// Return polar angle of p with respect to origin o
double to_angle(const struct MathGeometry::point_t *p, const struct MathGeometry::point_t *o) {
    return atan2(p->y - o->y, p->x - o->x);
}
void find_centroid(struct MathGeometry::point_t *c, struct MathGeometry::point_t *pts, int n_pts) {
    double x = 0, y = 0;
    for (int i = 0; i < n_pts; i++) {
        x += pts[i].x;
        y += pts[i].y;
    }
    c->x = x / n_pts;
    c->y = y / n_pts;
}
int by_polar_angle(const void *va, const void *vb) {
    double theta_a = to_angle((MathGeometry::point_t*)va, centroid);
    double theta_b = to_angle((MathGeometry::point_t*)vb, centroid);
    return theta_a < theta_b ? -1 : theta_a > theta_b ? 1 : 0;
}
void sort_by_polar_angle(struct MathGeometry::point_t *pts, int n_pts) {
    find_centroid(centroid, pts, n_pts);
    qsort(pts, n_pts, sizeof pts[0], by_polar_angle);
}
double find_norm(const MathGeometry::point_t &a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

// ---------------------------------------------
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the MathGeometry::point_ts are collinear.
double cross(const MathGeometry::point_t &A, const MathGeometry::point_t &B, const MathGeometry::point_t &O = MathGeometry::point_t(0, 0))
{
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

double _2Dcross(const MathGeometry::point_t &v, const MathGeometry::point_t &w)
{
    return v.x * w.y - v.y * w.x;
}

int MathGeometry::findPartitionNumber(MathGeometry::point_t p, int numHorzPartitions, int numVertPartitions)
{
#if defined(__APPLE__)
    p += MathGeometry::point_t(1.0, 1.0);
    int partNumber = ((int)(p.y * numVertPartitions / 2.0) * numHorzPartitions + (int)(p.x * numHorzPartitions / 2.0));
    partNumber = partNumber < 0 ? 0 : partNumber; // TODO: This isn't ideal. Strictly vertical or horizontal out-of-boundness could be an issue
    partNumber = partNumber >= numHorzPartitions * numVertPartitions ? numHorzPartitions * numVertPartitions - 1 : partNumber;
    return partNumber;
#else
    return 0;
#endif
}

MathGeometry::point_t MathGeometry::findDirectionVector(const MathGeometry::point_t &a, const MathGeometry::point_t &b, const MathGeometry::point_t &c, direction_t Type)
{
    // TODO: All three types of direction vector are limited; there's always intersections.. On larger scale simulations, it's rarely an issue,
    // but I've basically exhausted my brain in trying to find a better solution that looks strictly at the two neighbors
    // (and I've found issues other than efficiency with solutions that look at more nodes, like the fact that jagged edges
    // would be weird in more holistic approaches). So I'm adding a routine that prevents intersection of inner nodes by
    // maybe pushing them outward, like collision resolution in game programming.
    MathGeometry::point_t directionOffset;
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
        MathGeometry::point_t dO = a - b;

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
        MathGeometry::point_t aShift = a - c;
        MathGeometry::point_t bShift = b - c;

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
        MathGeometry::point_t dO = a - b;

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
        MathGeometry::point_t aShift = a - c;
        MathGeometry::point_t bShift = b - c;

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

bool findLineSegmentIntersection(MathGeometry::point_t a1, MathGeometry::point_t a2, MathGeometry::point_t b1, MathGeometry::point_t b2, MathGeometry::point_t &where)
{
    // It's hard for this to be self-documenting haha
    MathGeometry::point_t r, s;
    r = a2 - a1;
    s = b2 - b1;

    // Now we have: line = q + qv, and any point on the vector is obtainable by p + t*r, for some t
    // We want a t and u so p + t*pv = q + u*qv. Then t = (q − p) × s / (r × s) and u = (q − p) × r / (r × s)
    double t, u;
    double crossRandS = cross(r, s);
    double crossQminusPandS = cross(b1 - a1, s);
    double crossQminusPandR = cross(b1 - a1, r);

    // Collinear and parallel possibilities don't matter here. We simply treat them as non-intersections
    if (crossRandS == 0)
    {
        return false;
    }

    t = crossQminusPandS / crossRandS;
    u = crossQminusPandR / crossRandS;

    // If both T and U are in [0, 1] interval, then an intersection happens. We adopt an exclusive interval
    // so that neighbor edges aren't taken to be intersecting.
    if (MathGeometry::gtTolerance(t, 0, MathGeometry::TOLERANCE) && MathGeometry::ltTolerance(t, 1, MathGeometry::TOLERANCE) && MathGeometry::gtTolerance(u, 0, MathGeometry::TOLERANCE) && MathGeometry::ltTolerance(u, 1, MathGeometry::TOLERANCE))
    {
        where = a1 + r * t;
        return true;
    }

    // If control reaches this point, previous test failed. There is no intersection.
    return false;
}

std::vector<MathGeometry::point_t> MathGeometry::surfaceIntersections(const std::vector<std::pair<MathGeometry::point_t, MathGeometry::point_t>> &lines){
    std::vector<MathGeometry::point_t> toReturn;
    for (auto outerIt = lines.begin(); outerIt != lines.end(); outerIt++){
        for (auto innerIt = outerIt; innerIt != lines.end(); innerIt++){
            MathGeometry::point_t where;
            if (findLineSegmentIntersection(outerIt->first, outerIt->second, innerIt->first, innerIt->second, where)){
                toReturn.push_back(where);
            }
        }
    }
    return toReturn;
}