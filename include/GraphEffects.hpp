//
// Created by Andre Muricy on 2020-02-07.
//

#ifndef SA_SURFACE_2D_GRAPHEFFECTS_HPP
#define SA_SURFACE_2D_GRAPHEFFECTS_HPP


#include "GraphSurface.hpp"

namespace Effects {
    void mergeTwoNodes(Graph::Surface &belonging, Graph::Node *a, Graph::Node *b);

    void addNode2(Graph::Surface *belonging, Graph::Node *a, Graph::Node *b, double bothCorrsDist);

    void adjustNodeResolution(std::set<Graph::NodeChange> &changes, double splitThreshold, double bothCorrsDist);

    void applyNodeChanges(std::set<Graph::NodeChange> &changes);
    void revertNodeChanges(std::set<Graph::NodeChange> &changes);
}

#endif //SA_SURFACE_2D_GRAPHEFFECTS_HPP
