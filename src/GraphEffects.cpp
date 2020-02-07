//
// Created by Andre Muricy on 2020-02-07.
//

#include <MathGeometry.hpp>
#include "GraphEffects.hpp"

namespace Effects {
    void mergeTwoNodes(Graph::Surface &belonging, Graph::Node *a, Graph::Node *b) {
        if (a->to == b && b->from == a) {
            auto aux = new Graph::Node();
            aux->coords[Graph::X] = (a->coords[Graph::X] + b->coords[Graph::X]) / 2;
            aux->coords[Graph::Y] = (a->coords[Graph::Y] + b->coords[Graph::Y]) / 2;
            aux->to = b->to;
            aux->from = a->from;
            b->from->to = aux;
            a->to->from = aux;
            for (auto it = b->correspondents.begin(); it != b->correspondents.end(); it++) {
                aux->correspondents.insert(*it);
            }
            for (auto it = a->correspondents.begin(); it != a->correspondents.end(); it++) {
                aux->correspondents.insert(*it);
            }
        } else if (a->from == b && b->to == a) {
            auto aux = new Graph::Node();
            aux->coords[Graph::X] = (a->coords[Graph::X] + b->coords[Graph::X]) / 2;
            aux->coords[Graph::Y] = (a->coords[Graph::Y] + b->coords[Graph::Y]) / 2;
            aux->to = a->to;
            aux->from = b->from;
            b->to->from = aux;
            a->from->to = aux;
            for (auto it = b->correspondents.begin(); it != b->correspondents.end(); it++) {
                aux->correspondents.insert(*it);
            }
            for (auto it = a->correspondents.begin(); it != a->correspondents.end(); it++) {
                aux->correspondents.insert(*it);
            }
        }
    }

    Graph::Node *commonCorrespondent(Graph::Node *a, Graph::Node *b) {
        Graph::Node *toReturn = nullptr;
        for (auto itA = a->correspondents.begin(); itA != a->correspondents.end(); itA++) {
            for (auto itB = b->correspondents.begin(); itB != b->correspondents.end(); itB++) {
                if (*itA == *itB)
                    toReturn = *itA;
            }
        }
        return toReturn;
    }

    std::set<Graph::Node *> correspondToBothNeighbors(double newX, double newY, Graph::Node *a, Graph::Node *b, double bothCorrsDist){
        auto closestToNewNode = [newX, newY](Graph::Node *c0, Graph::Node *c1) {
            auto dist0 = MathGeometry::findNorm2d(c0->coords[Graph::X] - newX, c0->coords[Graph::Y] - newY);
            auto dist1 = MathGeometry::findNorm2d(c1->coords[Graph::X] - newX, c1->coords[Graph::Y] - newY);
            return dist0 < dist1;
        };
        auto leftCorr = std::min_element(a->correspondents.begin(), a->correspondents.end(), closestToNewNode);
        auto rightCorr = std::min_element(b->correspondents.begin(), b->correspondents.end(), closestToNewNode);
        auto leftCorrDist = MathGeometry::findNorm2d((*leftCorr)->coords[Graph::X] - newX,
                                                     (*leftCorr)->coords[Graph::Y] - newY);
        auto rightCorrDist = MathGeometry::findNorm2d((*rightCorr)->coords[Graph::X] - newX,
                                                      (*rightCorr)->coords[Graph::Y] - newY);
        if (leftCorrDist / rightCorrDist > bothCorrsDist && leftCorrDist / rightCorrDist < (1 / bothCorrsDist)) {
            return std::set<Graph::Node *> {*leftCorr, *rightCorr};
        } else
            return std::set<Graph::Node *> { closestToNewNode(*leftCorr, *rightCorr) ? *leftCorr : *rightCorr };
    }

    std::set<Graph::Node *> getCorrespondents(double newX, double newY, Graph::Node *a, Graph::Node *b, double bothCorrsDist) {
        auto potentialCorrespondent = commonCorrespondent(a, b);
        if (potentialCorrespondent != nullptr)
            return std::set<Graph::Node* >{ potentialCorrespondent };
        else
            return correspondToBothNeighbors(newX, newY, a, b, bothCorrsDist);
    }

    int assertNeighbors(Graph::Node *a, Graph::Node *b) {
        if (a->to == b && b->from == a)
            return 0;
        else if (b->to == a && a->from == b)
            return 1;

        std::cout << "Graph::Nodes arent neighbors; a: " << a << ", b: " << b << std::endl <<
                  "a->to: " << a->to << " a->from: " << a->from << std::endl <<
                  "b->to: " << b->to << " b->from: " << b->from << std::endl;
        exit(0);
    }

    void addNode2(Graph::Surface *belonging, Graph::Node *a, Graph::Node *b, double bothCorrsDist) {
        auto neighborlyStatus = assertNeighbors(a, b);

        auto nodeToBeCommitted = new Graph::Node();
        nodeToBeCommitted->coords[Graph::X] = (a->coords[Graph::X] + b->coords[Graph::X]) / 2;
        nodeToBeCommitted->coords[Graph::Y] = (a->coords[Graph::Y] + b->coords[Graph::Y]) / 2;
        nodeToBeCommitted->to = neighborlyStatus == 1 ? a : b;
        nodeToBeCommitted->from = neighborlyStatus == 1 ? b : a;
        nodeToBeCommitted->correspondents = getCorrespondents(nodeToBeCommitted->coords[Graph::X],
                                                              nodeToBeCommitted->coords[Graph::Y], a, b, bothCorrsDist);

        // TODO: We can have a 1-sized set without this working. We need a special case
        if (nodeToBeCommitted->correspondents.size() == 1){
            (*nodeToBeCommitted->correspondents.begin())->correspondents = std::set<Graph::Node*>{nodeToBeCommitted};
        }
        else {
            for (auto it = nodeToBeCommitted->correspondents.begin(); it != nodeToBeCommitted->correspondents.end(); it++) {
                (*it)->correspondents.insert(nodeToBeCommitted);
            }
        }

        belonging->nodes.push_back(nodeToBeCommitted);

        a->to = neighborlyStatus == 1 ? a->to : nodeToBeCommitted;
        a->from = neighborlyStatus == 1 ? nodeToBeCommitted : a->from;

        b->to = neighborlyStatus == 0 ? b->to : nodeToBeCommitted;
        b->from = neighborlyStatus == 0 ? nodeToBeCommitted : b->from;
    }

    void adjustNodeResolution(std::set<Graph::NodeChange> &changes, double splitThreshold, double bothCorrsDist) {
        for (auto it = changes.begin(); it != changes.end(); it++) {
            auto distTo = MathGeometry::findNorm2d(it->node->coords[Graph::X] - it->node->to->coords[Graph::X],
                                                   it->node->coords[Graph::Y] - it->node->to->coords[Graph::Y]);
            auto distFrom = MathGeometry::findNorm2d(it->node->coords[Graph::X] - it->node->from->coords[Graph::X],
                                                     it->node->coords[Graph::Y] - it->node->from->coords[Graph::Y]);
            if (distTo > splitThreshold)
                addNode2(it->whoHasIt, it->node, it->node->to, bothCorrsDist);
            if (distFrom > splitThreshold)
                addNode2(it->whoHasIt, it->node, it->node->from, bothCorrsDist);
        }
    }

    void applyNodeChanges(std::set<Graph::NodeChange> &changes) {
        for (auto it = changes.begin(); it != changes.end(); it++) {
            it->node->coords[Graph::X] = it->newX;
            it->node->coords[Graph::Y] = it->newY;
        }
    }

    void revertNodeChanges(std::set<Graph::NodeChange> &changes) {
        for (auto it = changes.begin(); it != changes.end(); it++) {
            it->node->coords[Graph::X] = it->prevX;
            it->node->coords[Graph::Y] = it->prevY;
        }
    }
}