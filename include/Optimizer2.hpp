//
// Created by Andre Muricy on 2019-12-25.
//

#ifndef SA_SURFACE_2D_OPTIMIZER2_H
#define SA_SURFACE_2D_OPTIMIZER2_H

#include <set>
#include "GlobalState.hpp"

namespace Optimizer2 {

    std::set<Graph::NodeChange> findNeighbor();
    double findEnergy();
    double findProbability(double energyState, double energyNeighbor, double temperature);
    double stepSimulatedAnnealing ();
    void temperatureFunction();
}

#endif //SA_SURFACE_2D_OPTIMIZER2_H
