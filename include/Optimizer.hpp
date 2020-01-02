#pragma once

#include <set>
#include "GlobalState.hpp"

namespace Optimizer {

    std::set<Graph::NodeChange> findNeighbor();
    double findEnergy();
    double findProbability(double energyState, double energyNeighbor, double temperature);
    void stepSimulatedAnnealing ();
    void temperatureFunction();
}
