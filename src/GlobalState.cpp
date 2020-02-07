//
// Contains every reference to any global state.
//
#include <GlobalState.hpp>
#include <vector>
#include <MathGeometry.hpp>
#include <Util.hpp>

namespace GlobalState {
    Graph::ThickSurface thickSurface;
    SurfaceParameters surfaceParameters;
    OptimizerParameters optimizerParameters;
    Graph::Intersectables intersectables; // <- BOA IDEIA
    bool shouldStep = false;
    bool singleStep = false;
    bool recording = false;
    std::set<std::string> recordedAttributes;


    void setSurfaceParameters(double radius, double thickness, double centerX, double centerY, int points,
                              double bothCorrsDist,
                              double splitThreshold) {
        surfaceParameters.radius = radius;
        surfaceParameters.thickness = thickness;
        surfaceParameters.centerX = centerX;
        surfaceParameters.centerY = centerY;
        surfaceParameters.points = points;
        surfaceParameters.bothCorrsDist = bothCorrsDist;
        surfaceParameters.splitThreshold = splitThreshold;
    }

    void setOptimizerParameters(double initialGrayMatter, int smoothness, double diffMul, double diffPow,
                                double areaMul, double areaPow, double multiProb, double tempProb,
                                double forceOffsetRange, double compression,
                                double (*smoothnessFunction)(double, double), double temperature) {
        optimizerParameters.initialGrayMatter = initialGrayMatter;
        optimizerParameters.smoothness = smoothness;
        optimizerParameters.diffMul = diffMul;
        optimizerParameters.diffPow = diffPow;
        optimizerParameters.areaMul = areaMul;
        optimizerParameters.areaPow = areaPow;
        optimizerParameters.multiProb = multiProb;
        optimizerParameters.tempProb = tempProb;
        optimizerParameters.forceOffsetRange = forceOffsetRange;
        optimizerParameters.compression = compression;
        optimizerParameters.smoothnessFunction = smoothnessFunction;
        optimizerParameters.temperature = temperature;
    }

    void initThickSurface() {
        thickSurface = Graph::generateCircularThicksurface(surfaceParameters.centerX,
                                                           surfaceParameters.centerY, surfaceParameters.radius,
                                                           surfaceParameters.thickness,
                                                           surfaceParameters.points);
    }

    void deliberatelyDeleteBecauseDestructorIsCalledWheneverItWants() {
        for (auto outerIt = thickSurface.layers.begin(); outerIt != thickSurface.layers.end(); outerIt++) {
            for (auto innerIt = outerIt->nodes.begin(); innerIt != outerIt->nodes.end(); innerIt++) {
                delete *innerIt;
            }
        }
    }
}