//
// Contains every reference to any global state.
//
#include <GlobalState.hpp>
#include <vector>
#include <MathGeometry.hpp>
#include <Util.hpp>

Graph::ThickSurface2 GlobalState::thickSurface;
GlobalState::SurfaceParameters GlobalState::surfaceParameters;
GlobalState::OptimizerParameters GlobalState::optimizerParameters;
Graph::Intersectables GlobalState::intersectables; // <- BOA IDEIA
bool GlobalState::shouldStep = false;
bool GlobalState::singleStep = false;
bool GlobalState::recording = false;


void GlobalState::setSurfaceParameters(double radius, double thickness, double centerX, double centerY, int points) {
    GlobalState::surfaceParameters.radius = radius;
    GlobalState::surfaceParameters.thickness = thickness;
    GlobalState::surfaceParameters.centerX = centerX;
    GlobalState::surfaceParameters.centerY = centerY;
    GlobalState::surfaceParameters.points = points;
}

void GlobalState::setOptimizerParameters(double initialGrayMatter, int smoothness, double diffMul, double diffPow,
                                         double areaMul, double areaPow, double multiProb, double tempProb,
                                         double forceOffsetRange, double compression,
                                         double (*smoothnessFunction)(double, double), double temperature) {
    GlobalState::optimizerParameters.initialGrayMatter = initialGrayMatter;
    GlobalState::optimizerParameters.smoothness = smoothness;
    GlobalState::optimizerParameters.diffMul = diffMul;
    GlobalState::optimizerParameters.diffPow = diffPow;
    GlobalState::optimizerParameters.areaMul = areaMul;
    GlobalState::optimizerParameters.areaPow = areaPow;
    GlobalState::optimizerParameters.multiProb = multiProb;
    GlobalState::optimizerParameters.tempProb = tempProb;
    GlobalState::optimizerParameters.forceOffsetRange = forceOffsetRange;
    GlobalState::optimizerParameters.compression = compression;
    GlobalState::optimizerParameters.smoothnessFunction = smoothnessFunction;
    GlobalState::optimizerParameters.temperature = temperature;
}

void GlobalState::initThickSurface() {
    GlobalState::thickSurface = Graph::generateCircularThicksurface(surfaceParameters.centerX,
                                                                    surfaceParameters.centerY, surfaceParameters.radius,
                                                                    surfaceParameters.thickness,
                                                                    surfaceParameters.points);
}
