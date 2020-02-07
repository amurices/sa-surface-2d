//
// Created by Andre Muricy on 2019-12-25.
//

#include <MathGeometry.hpp>
#include <cmath>
#include <Renderer.hpp>
#include "Optimizer.hpp"

std::set<Graph::NodeChange> Optimizer::findNeighbor(){
    auto nodesToPush = Graph::randomNodes(GlobalState::thickSurface.layers[Graph::OUTER],
                                          GlobalState::optimizerParameters.multiProb);
    return Graph::generateTotalChangesetFromPushedOuterNodes(GlobalState::thickSurface,
                                                             nodesToPush,
                                                             GlobalState::optimizerParameters.compression,
                                                             GlobalState::optimizerParameters.forceOffsetRange,
                                                             GlobalState::optimizerParameters.multiProb,
                                                             GlobalState::optimizerParameters.smoothness,
                                                             GlobalState::optimizerParameters.smoothnessFunction);
}
double Optimizer::findEnergy(){
    double res;

    double whiteMatterArea;
    double grayMatterArea;

    // White matter area is just the inner surface's area
    whiteMatterArea = Graph::surfaceArea(GlobalState::thickSurface.layers[Graph::INNER]);
    // Gray matter area is the difference between the outer surface and the inner surface
    grayMatterArea = Graph::surfaceArea(GlobalState::thickSurface.layers[Graph::OUTER]) - whiteMatterArea;
    // Area must be non-negative
    grayMatterArea = MathGeometry::absol(grayMatterArea);

    // Difference between initial and current gray matter area.
    double grayMatterStretch = MathGeometry::absol(GlobalState::optimizerParameters.initialGrayMatter - grayMatterArea);

    double p1, p2;
    p1 = pow(whiteMatterArea, GlobalState::optimizerParameters.areaPow);
    p2 = pow(grayMatterStretch + 1, GlobalState::optimizerParameters.diffPow); // Stretch is the difference; therefore p2 is raised to the right power here
    // We add 1 before raising to ensure the growth is not sublinear from the get-go

    // Simplified energy equation of 2D closed surface
    res = GlobalState::optimizerParameters.areaMul * p1 + GlobalState::optimizerParameters.diffMul * p2;

    return res;
}

double Optimizer::findProbability(double energyState, double energyNeighbor, double temperature){
    if (MathGeometry::ltTolerance(temperature, 0.0, MathGeometry::TOLERANCE)){
        return energyNeighbor < energyState ? 1 : 0;
    } else if (temperature >= INFINITY) {
        return 1;
    } else {
        return exp((energyState - energyNeighbor) / temperature);
    }
}

void Optimizer::temperatureFunction(){
    // temperature is constant for now
    GlobalState::optimizerParameters.temperature = GlobalState::optimizerParameters.temperature;
}

std::vector<std::pair<MathGeometry::point_t, MathGeometry::point_t>> makeLines(){
    std::vector<std::pair<MathGeometry::point_t, MathGeometry::point_t>> toReturn;
    for (auto layerIt = GlobalState::thickSurface.layers.begin(); layerIt != GlobalState::thickSurface.layers.end(); layerIt++){
        for (auto surfaceIt = layerIt->nodes.begin(); surfaceIt != layerIt->nodes.end(); surfaceIt++){
            toReturn.push_back(std::make_pair(MathGeometry::point_t((*surfaceIt)->coords[Graph::X],
                                                          (*surfaceIt)->coords[Graph::Y]),
                                                  MathGeometry::point_t((*surfaceIt)->to->coords[Graph::X],
                                                          (*surfaceIt)->to->coords[Graph::Y])));
        }
    }
    return toReturn;
}

void assertCorrespondences(){
    auto beg = GlobalState::thickSurface.layers[Graph::OUTER].nodes[0];
    auto it = beg;
    do {
        for (auto corrsIt = it->correspondents.begin(); corrsIt != it->correspondents.end(); corrsIt++){
            bool found = false;
            for (auto reverseCorrsIt = (*corrsIt)->correspondents.begin(); reverseCorrsIt != (*corrsIt)->correspondents.end(); reverseCorrsIt++){
                if (*reverseCorrsIt == it){
                    found = true;
                    break;
                }
            }
            if (!found){
                std::cout << "Node " << it << " corresponds to " << *corrsIt << " but the latter's correspondents are: " << std::endl;
                for (auto reverseCorrsIt = (*corrsIt)->correspondents.begin(); reverseCorrsIt != (*corrsIt)->correspondents.end(); reverseCorrsIt++){
                    std::cout << *reverseCorrsIt << std::endl;
                }
            }
        }
        it = it->to;
    } while (it != beg);

}

void Optimizer::stepSimulatedAnnealing (){
    auto neighborChanges = findNeighbor();
    double energyState  = findEnergy();
    Graph::applyNodeChanges(neighborChanges);
    double energyNeighbor = findEnergy();

    auto surfaceLines = makeLines(); // <- Quando consertar as particoes tem que arrumar isso aqui
    auto intersections = MathGeometry::surfaceIntersections(surfaceLines);
    double prob = findProbability(energyState, energyNeighbor, GlobalState::optimizerParameters.temperature);
    if (!intersections.empty()){
        prob = 0.0;
    }
    double coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    if (coinFlip > prob)
    {
        Graph::revertNodeChanges(neighborChanges);
    } else {
        std::cout << "Nodes before: " << GlobalState::thickSurface.layers[Graph::OUTER].nodes.size() + GlobalState::thickSurface.layers[Graph::INNER].nodes.size();
        std::cout << " Lines before: " << Renderer::countNumberOfLines() << std::endl;

        Graph::adjustNodeResolution(GlobalState::thickSurface, GlobalState::surfaceParameters.splitThreshold, GlobalState::surfaceParameters.bothCorrsDist);

        std::cout << "Nodes after: " << GlobalState::thickSurface.layers[Graph::OUTER].nodes.size() + GlobalState::thickSurface.layers[Graph::INNER].nodes.size();
        std::cout << " Lines after: " << Renderer::countNumberOfLines() << std::endl;

    }
    assertCorrespondences();

    temperatureFunction();
}