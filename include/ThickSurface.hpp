#pragma once
#include "defs.hpp"
#include "_2DSurface.hpp"

class ThickSurface
{
  public:
    // Reference to the outside of the surface
    _2DSurface *outer;
    // Reference to the inner part of the surface
    _2DSurface *inner;
    // Vector containing distance between corresponding outer and inner nodes at each index
    std::vector<double> thicknesses;

    /*
     * Generates a random thicknesses vector.
     *
     * @param pts the amount of points the inner and outer surfaces contain.
     * @param upperPercentOfRadius the upper limit of random thickness, in terms of % of outer radius.
     * @param lowerPercentOfRadius the lower limit of random thickness, in terms of % of outer radius.
     */
    void generateRandomThicknesses(int pts, double upperPercentOfRadius = 0.2, double lowerPercentOfRadius = 0.0);

    /*
     * Generates a circular thick surface.
     *
     * @param radius a float value for the radius (in the OpenGL scale (1.0 is the edge of the screen)) of the outer surface.
     * @param pts the amount of points the inner and outer surfaces contain.
     * @param randomThicknesses boolean value that determines wheether this surface's thickness should be randomly generated.
     * @param ub the upper limit of random thickness, in terms of % of outer radius.
     * @param lb the lower limit of random thickness, in terms of % of outer radius.
     * @param center where, in OpenGL coordinates, does the surface's center lie. 
     */
    void generateCircularThickSurface(double radius, int pts, bool randomThicknesses = true,
                                      double ub = 0.2, double lb = 0.0, point_t center = point_t(0.0, 0.0),
                                      int horzPartsNumber = 2, int vertPartsNumber = 2);

    /*
     * Given a set of changed nodes in the outer surface, updates the inner surface.
     *
     * @param changedNodes a unique set (no repeated elements) of nodes in the outer surface that need their correspondents updated.
     */
    void updateInnerSurfaceV2(std::set<NodeChange_t> *nodeChanges);

    void updateEntireInnerSurface();

    void smoothAdjacentThicknesses(double thicknessDiff, int smoothness, SNode startingIndex, std::set<ThicknessChange_t> *changedSet, double (*func)(double u, double c));

    /* 
     * Default constructor. Does nothing except allocate memory and set standard values to 0.
     */
    ThickSurface();

    /*
     * Copy constructor. 
     *
     * @param copied Thick Surface to be copied. Calls substructures' constructors.
     */
    ThickSurface(const ThickSurface &copied);

    /* Destructor.
     *
     * Calls the substructures' destructors.
     */
    ~ThickSurface();

    // Destroys this thick surface's substructures, then reconstructs them by calling their copy constructors on &p.
    void operator=(const ThickSurface &p);
};
