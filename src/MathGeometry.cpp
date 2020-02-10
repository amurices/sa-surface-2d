#include <cmath>
#include <iostream>
#include "MathGeometry.hpp"

#include "Util.hpp"
namespace MathGeometry {
    double findNorm(const point_t &a) {
        return sqrt(a.x * a.x + a.y * a.y);
    }

    double findNorm2d(double x, double y) {
        return sqrt(x * x + y * y);
    }

    double sineSmooth(double u, double c) {
        return sin(0.5 * PI * (u - c) / u);
    }

    double linearSmooth(double u, double c) {
        return (u - c) / u;
    }

    double inverseSmooth(double u, double c) {
        return 1 / (c + 1);
    }
    std::ostream &operator<<(std::ostream &os, const point_t &n){
        os << "(" << n.x << ", " << n.y << ")";
        return os;
    }

    static struct point_t *centroid;

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
        double theta_a = to_angle((point_t *) va, centroid);
        double theta_b = to_angle((point_t *) vb, centroid);
        return theta_a < theta_b ? -1 : theta_a > theta_b ? 1 : 0;
    }

    void sort_by_polar_angle(struct point_t *pts, int n_pts) {
        find_centroid(centroid, pts, n_pts);
        qsort(pts, n_pts, sizeof pts[0], by_polar_angle);
    }

    double find_norm(const point_t &a) {
        return sqrt(a.x * a.x + a.y * a.y);
    }

// ---------------------------------------------
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the point_ts are collinear.
    double cross(const point_t &A, const point_t &B, const point_t &O = point_t(0, 0)) {
        return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
    }

    double _2Dcross(const point_t &v, const point_t &w) {
        return v.x * w.y - v.y * w.x;
    }

    int findPartitionNumber(point_t p, int numHorzPartitions, int numVertPartitions) {
#if defined(__APPLE__)
        p += point_t(1.0, 1.0);
        int partNumber = ((int) (p.y * numVertPartitions / 2.0) * numHorzPartitions +
                          (int) (p.x * numHorzPartitions / 2.0));
        partNumber = partNumber < 0 ? 0
                                    : partNumber; // TODO: This isn't ideal. Strictly vertical or horizontal out-of-boundness could be an issue
        partNumber = partNumber >= numHorzPartitions * numVertPartitions ? numHorzPartitions * numVertPartitions - 1
                                                                         : partNumber;
        return partNumber;
#else
        return 0;
#endif
    }

    point_t findDirectionVector2(point_t p, point_t to, point_t from){
        point_t offsetTo = to - p;
        point_t offsetFrom = from - p;

        point_t normedTo = offsetTo * (1/findNorm(offsetTo));
        point_t normedFrom = offsetFrom * (1/findNorm(offsetFrom));

        point_t dir = (normedTo + normedFrom) * 0.5;
        if (dir.x == 0 && dir.y == 0){
            // then direction is "to" rotated 90 degrees
            dir.x = - offsetTo.y; dir.y = offsetTo.x;
        }
        point_t normalizedDir = dir * (1/findNorm(dir));
        point_t dO(from.y - to.y, to.x - from.x); // this is what was different
        dO = dO * findNorm(dO);
        if (findNorm(dO - normalizedDir) > findNorm(dO + normalizedDir)){
            normalizedDir = normalizedDir * (-1);
        }
        return normalizedDir;
    }

    bool findLineSegmentIntersection(point_t a1, point_t a2, point_t b1, point_t b2, point_t &where) {
        // It's hard for this to be self-documenting haha
        point_t r, s;
        r = a2 - a1;
        s = b2 - b1;

        // Now we have: line = q + qv, and any point on the vector is obtainable by p + t*r, for some t
        // We want a t and u so p + t*pv = q + u*qv. Then t = (q − p) × s / (r × s) and u = (q − p) × r / (r × s)
        double t, u;
        double crossRandS = cross(r, s);
        double crossQminusPandS = cross(b1 - a1, s);
        double crossQminusPandR = cross(b1 - a1, r);

        // Collinear and parallel possibilities don't matter here. We simply treat them as non-intersections
        if (crossRandS == 0) {
            return false;
        }

        t = crossQminusPandS / crossRandS;
        u = crossQminusPandR / crossRandS;

        // If both T and U are in [0, 1] interval, then an intersection happens. We adopt an exclusive interval
        // so that neighbor edges aren't taken to be intersecting.
        if (gtTolerance(t, 0, TOLERANCE) && ltTolerance(t, 1, TOLERANCE) && gtTolerance(u, 0, TOLERANCE) &&
            ltTolerance(u, 1, TOLERANCE)) {
            where = a1 + r * t;
            return true;
        }

        // If control reaches this point, previous test failed. There is no intersection.
        return false;
    }

    std::vector<point_t> surfaceIntersections(const std::vector<std::pair<point_t, point_t>> &lines) {
        std::vector<point_t> toReturn;
        for (auto outerIt = lines.begin(); outerIt != lines.end(); outerIt++) {
            for (auto innerIt = outerIt; innerIt != lines.end(); innerIt++) {
                point_t where;
                if (findLineSegmentIntersection(outerIt->first, outerIt->second, innerIt->first, innerIt->second,
                                                where)) {
                    toReturn.push_back(where);
                }
            }
        }
        return toReturn;
    }
}