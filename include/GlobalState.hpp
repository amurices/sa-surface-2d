//
// Created by Andre Muricy on 2019-12-22.
//

#ifndef SA_SURFACE_2D_GLOBALSTATE_HPP
#define SA_SURFACE_2D_GLOBALSTATE_HPP

#include <vector>
#include <set>
#include <map>
#include <GraphSurface.hpp>

namespace GlobalState {
    struct SurfaceParameters {
        double radius;
        double thickness;
        double centerX;
        double centerY;
        int points;
        double mergeThreshold;
        double splitThreshold;
        double bothCorrsDist;
    };

    struct OptimizerParameters {
        double initialGrayMatter;
        int smoothness;
        double diffMul;
        double diffPow;
        double areaPow;
        double areaMul;
        double multiProb;
        double tempProb;
        double forceOffsetRange;
        double compression;

        double (*smoothnessFunction)(double, double);

        double temperature;
        double craniumRadius;
    };

    extern SurfaceParameters surfaceParameters;
    extern OptimizerParameters optimizerParameters;
    extern Graph::ThickSurface thickSurface;
    extern Graph::Intersectables intersectables;
    extern bool singleStep;
    extern bool shouldStep;
    extern bool recording;
    extern bool showCorrespondences;
    extern bool adjustNodeResolution;
    extern std::set<std::string> recordedAttributes; // fns that receive a reference to a thick surface and calculate something of interest

    void setSurfaceParameters(double radius, double thickness, double centerX, double centerY, int points,
                              double bothCorrsDist,
                              double splitThreshold);

    void
    setOptimizerParameters(double initialGrayMatter, int smoothness, double diffMul, double diffPow, double areaMul,
                           double areaPow, double multiProb, double tempProb, double forceOffsetRange,
                           double compression, double (*smoothnessFunction)(double, double), double temperature, double craniumRadius);

    void initThickSurface();
    void deliberatelyDeleteBecauseDestructorIsCalledWheneverItWants();
}

#endif //SA_SURFACE_2D_GLOBALSTATE_HPP
