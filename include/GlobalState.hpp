//
// Created by Andre Muricy on 2019-12-22.
//

#ifndef SA_SURFACE_2D_GLOBALSTATE_HPP
#define SA_SURFACE_2D_GLOBALSTATE_HPP

#include <vector>
#include <set>
#include <GraphSurface.hpp>

namespace GlobalState {
    struct SurfaceParameters {
        double radius;
        double thickness;
        double centerX;
        double centerY;
        int points;
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
    };

    extern SurfaceParameters surfaceParameters;
    extern OptimizerParameters optimizerParameters;
    extern Graph::ThickSurface2 thickSurface;
    extern Graph::Intersectables intersectables;

    void setSurfaceParameters(double radius, double thickness, double centerX, double centerY, int points);

    void
    setOptimizerParameters(double initialGrayMatter, int smoothness, double diffMul, double diffPow, double areaMul,
                           double areaPow, double multiProb, double tempProb, double forceOffsetRange,
                           double compression, double (*smoothnessFunction)(double, double), double temperature);

    void initThickSurface();
}

#endif //SA_SURFACE_2D_GLOBALSTATE_HPP
