#pragma once

#include <set>
#include "GraphSurface.hpp"

namespace Optimizer {

    std::set<Graph::NodeChange> findNeighbor();
    double findEnergy();
    double findProbability(double energyState, double energyNeighbor, double temperature);
    void assertCorrespondences();
    void stepSimulatedAnnealing ();
    void temperatureFunction();
}
