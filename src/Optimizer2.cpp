//
// Created by Andre Muricy on 2019-12-25.
//

#include <MathGeometry.hpp>
#include "Optimizer2.hpp"

std::set<Graph::NodeChange> Optimizer2::findNeighbor(){
    return Graph::neighborChangeset(GlobalState::thickSurface,
                                    GlobalState::optimizerParameters->compression,
                                    GlobalState::optimizerParameters->forceOffsetRange,
                                    GlobalState::optimizerParameters->multiProb,
                                    GlobalState::optimizerParameters->smoothness,
                                    GlobalState::optimizerParameters->smoothnessFunction);
}
double Optimizer2::findEnergy(){
    double res;

    double whiteMatterArea;
    double grayMatterArea;

    // White matter area is just the inner surface's area
    whiteMatterArea = Graph::surfaceAreaAndPerimeter(GlobalState::thickSurface.layers[Graph::INNER])[0];
    // Gray matter area is the difference between the outer surface and the inner surface
    grayMatterArea = Graph::surfaceAreaAndPerimeter(GlobalState::thickSurface.layers[Graph::OUTER])[0] - whiteMatterArea;
    // Area must be non-negative
    grayMatterArea = MathGeometry::absol(grayMatterArea);

    // Difference between initial and current gray matter area.
    double grayMatterStretch = MathGeometry::absol(GlobalState::optimizerParameters->initialGrayMatter - grayMatterArea);

    double p1, p2;
    p1 = pow(whiteMatterArea, GlobalState::optimizerParameters->areaPow);
    p2 = pow(grayMatterStretch + 1, GlobalState::optimizerParameters->diffPow); // Stretch is the difference; therefore p2 is raised to the right power here
    // We add 1 before raising to ensure the growth is not sublinear from the get-go

    // Simplified energy equation of 2D closed surface
    res = GlobalState::optimizerParameters->areaMul * p1 + GlobalState::optimizerParameters->diffMul * p2;
    return res;
}
double Optimizer2::findProbability(double energyState, double energyNeighbor, double temperature){
    double potentialResult = exp((energyState - energyNeighbor) / temperature);
    std::cout << "t is " << temperature <<  "; potentialResult (should be undefined) is: " << potentialResult << std::endl;
    if (energyNeighbor < energyState)
        return 1;
    else
        return potentialResult;
}

void Optimizer2::temperatureFunction(){
    // temperature is constant for now
    GlobalState::optimizerParameters->temperature = GlobalState::optimizerParameters->temperature;
}

double Optimizer2::stepSimulatedAnnealing (){
    auto neighborChanges = findNeighbor();
    double energyState  = findEnergy();
    for (auto it = neighborChanges.begin(); it != neighborChanges.end(); it++){
        it->node->coords[Graph::X] += it->changeX;
        it->node->coords[Graph::Y] += it->changeY;
    }
    double energyNeighbor = findEnergy();
    double prob = findProbability(energyState, energyNeighbor, GlobalState::optimizerParameters->temperature);
    double coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    if (coinFlip >= prob)
    {
        for (auto it = neighborChanges.begin(); it != neighborChanges.end(); it++){
            it->node->coords[Graph::X] -= it->changeX;
            it->node->coords[Graph::Y] -= it->changeY;
        }
    }
    temperatureFunction();
}