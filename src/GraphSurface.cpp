//
// Created by Andre Muricy on 2019-12-30.
//

#include <GraphSurface.hpp>
#include <MathGeometry.hpp>
#include <Util.hpp>
#include <cmath>
#include <iostream>

namespace Graph {
    std::ostream& operator<<(std::ostream& os, const NodeChange& nc)
    {
        os << "newX: " << nc.newX << "\n" <<
           "newY: " << nc.newY << "\n" <<
           "prevX: " << nc.prevX << "\n" <<
           "prevY: " << nc.prevY << "\n" <<
           "existed: " << nc.existed << "\n" <<
           "willExist: " << nc.willExist << "\n";
        return os;
    }

    Surface generateCircularGraph(double centerX, double centerY, double radius, int pts) {
        Surface toReturn;

        toReturn.nodes.push_back(new Node());
        toReturn.nodes[0]->coords[X] = centerX + radius;
        toReturn.nodes[0]->coords[Y] = centerY;

        for (int i = 1; i < pts; i++) {
            toReturn.nodes.push_back(new Node());
            toReturn.nodes[i]->coords[X] = centerX + cos(i * (2 * MathGeometry::PI) / pts) * radius;
            toReturn.nodes[i]->coords[Y] = centerY + sin(i * (2 * MathGeometry::PI) / pts) * radius;
            toReturn.nodes[i]->from = toReturn.nodes[i - 1];
            toReturn.nodes[i - 1]->to = toReturn.nodes[i];
        }

        toReturn.nodes[toReturn.nodes.size() - 1]->to = toReturn.nodes[0];
        toReturn.nodes[0]->from = toReturn.nodes[toReturn.nodes.size() - 1];

        return toReturn;
    }

    std::set<NodeChange>
    smoothAdjacentNodes(const Surface &surface, NodeChange initialChange, int smoothness,
                               double (*f)(double, double)) {
        std::set<NodeChange> toReturn;
        toReturn.insert(initialChange);

        Node *prev, *next;
        prev = next = initialChange.node;

        for (int c = 1; c <= smoothness; c++) {
            prev = prev->from;
            next = next->to;

            double ratio = (*f)(smoothness, c);
            auto initChangeXDelta = initialChange.newX - initialChange.prevX;
            auto initChangeYDelta = initialChange.newY - initialChange.prevY;
            NodeChange prevChange{prev,
                                  prev->coords[Graph::X] + (initChangeXDelta * ratio),
                                  prev->coords[Graph::Y] + (initChangeYDelta * ratio),
                                  prev->coords[Graph::X], prev->coords[Graph::Y], true, true};
            NodeChange nextChange{next,
                                  next->coords[Graph::X] + (initChangeXDelta * ratio),
                                  next->coords[Graph::Y] + (initChangeYDelta * ratio),
                                  next->coords[Graph::X], next->coords[Graph::Y], true, true};
            // Previous and next nodes are also going to be altered as a matter of where they are in relation to their inner correspondents
            toReturn.insert(prevChange);
            toReturn.insert(nextChange);
        }
        return toReturn;
    }

    double surfaceArea(const Surface &surface) {
        double toReturn = 0;
        Node *prev, *next, *no;
        no = surface.nodes[0];
        do {
            prev = no->from;
            next = no->to;
            toReturn += no->coords[Graph::X] * (next->coords[Graph::Y] - prev->coords[Graph::Y]);
            no = next;
        } while (no != surface.nodes[0]);

        toReturn /= 2;

        if (toReturn < 0)
            toReturn = 0;

        return toReturn;
    }

    ThickSurface
    generateCircularThicksurface(double centerX, double centerY, double outerRadius, double initialThickness,
                                        int pts) {
        ThickSurface toReturn;
        toReturn.layers[Graph::OUTER] = generateCircularGraph(centerX, centerY, outerRadius, pts);
        toReturn.layers[Graph::INNER] = generateCircularGraph(centerX, centerY, outerRadius - initialThickness, pts);

        /* One of the only places we can depend on nodes' indices */
        for (int i = 0; i < toReturn.layers[Graph::OUTER].nodes.size(); i++) {
            toReturn.layers[Graph::OUTER].nodes[i]->correspondents.insert(
                    toReturn.layers[Graph::INNER].nodes[i]
            );
            toReturn.layers[Graph::INNER].nodes[i]->correspondents.insert(
                    toReturn.layers[Graph::OUTER].nodes[i]
            );
        }
        return toReturn;
    }

    std::set<NodeChange>
    changesetForNodes(const Surface &surface, const std::vector<Node *> &nodesToPush,
                             double forceOffsetRange, int smoothness,
                             double (*f)(double, double)) {
        std::set<NodeChange> toReturn;
        for (auto it = nodesToPush.begin(); it != nodesToPush.end(); it++) {
            // Choose a random offset, bounded by optimizer params
            double offsetX, offsetY;
            offsetX = Util::getRandomRange(-forceOffsetRange, forceOffsetRange);
            offsetY = Util::getRandomRange(-forceOffsetRange, forceOffsetRange);

            // Collect change to outer node's position.
            auto randomChange = NodeChange(*it,
                    (*it)->coords[Graph::X] + offsetX,
                    (*it)->coords[Graph::Y] + offsetY,
                    (*it)->coords[Graph::X],
                    (*it)->coords[Graph::Y], true, true);
            toReturn = smoothAdjacentNodes(surface, randomChange, smoothness, f);
        }
        return toReturn;
    }

    void applyNodeChanges(std::set <NodeChange> &changes){
        for (auto it = changes.begin(); it != changes.end(); it++){
            it->node->coords[Graph::X] = it->newX;
            it->node->coords[Graph::Y] = it->newY;
        }
    }

/* This will assume the original changeset is in the outer surface, which means the changeset returned is
 * based on "new" coordinates of the outer surface. */
    std::set<NodeChange> innerChangesetFromOuterChangeset(const ThickSurface &thickSurface,
                                                                        const std::set<NodeChange> &outerChanges,
                                                                        double compression) {
        std::set<NodeChange> toReturn;
        for (auto it = outerChanges.begin(); it != outerChanges.end(); it++) {
            // Changed nodes have their correspondents updated one at a time
            Node *fnode = (*it).node;
            double nextX = fnode->to->coords[Graph::X];
            double nextY = fnode->to->coords[Graph::Y];
            double prevX = fnode->from->coords[Graph::X];
            double prevY = fnode->from->coords[Graph::Y];
            // Just to not have to reimplement findDirectionVector
            MathGeometry::point_t pPrev(prevX, prevY);
            MathGeometry::point_t pNext(nextX, nextY);
            // A little less obscene, but still in need of some cleaning up.
            auto distance = MathGeometry::findNorm(
                    MathGeometry::point_t(fnode->coords[Graph::X] - (*fnode->correspondents.begin())->coords[Graph::X],
                                          fnode->coords[Graph::Y] - (*fnode->correspondents.begin())->coords[Graph::Y]));
            MathGeometry::point_t vd = MathGeometry::findDirectionVector(pPrev, pNext,
                                                                         MathGeometry::point_t(fnode->coords[Graph::X], fnode->coords[Graph::Y]),
                                                                         MathGeometry::MEDIAN_ANGLE); // Get directional vector btwn inner & outer
            auto delta =
                    MathGeometry::point_t(it->newX, it->newY) // current new outer position
                    - (vd * distance) // plus direction (unit vector) times original distance = new position
                    - MathGeometry::point_t((*fnode->correspondents.begin())->coords[Graph::X], (*fnode->correspondents.begin())->coords[Graph::Y]); // because this will be added, subtract current position
            auto summin = (*fnode->correspondents.begin());
            toReturn.insert(
                    NodeChange(
                            summin,
                            summin->coords[Graph::X] + delta.x * compression,
                            summin->coords[Graph::Y] + delta.y * compression,
                            summin->coords[Graph::X],
                            summin->coords[Graph::Y],
                            true, true)
                    );
        }
        return toReturn;
    }

    std::vector<Node *> randomNodes(const Surface &surface, double multiProb) {
        std::vector<Node *> toReturn;
        double coinFlip;
        do {
            int randomIndex = rand() % surface.nodes.size();
            toReturn.push_back(surface.nodes[randomIndex]);
            coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        } while (coinFlip < multiProb);
        return toReturn;
    }

    std::set<NodeChange>
    generateTotalChangesetFromPushedOuterNodes(const ThickSurface &thickSurface,
                                                      const std::vector<Node *> &outerNodes, double compression,
                                                      double forceOffsetRange,
                                                      double multiProb, int smoothness,
                                                      double (*f)(double, double)) {
        std::set<NodeChange> toReturn;
        toReturn = changesetForNodes(thickSurface.layers[Graph::OUTER], outerNodes, forceOffsetRange, smoothness, f);
        auto innerChangeset = innerChangesetFromOuterChangeset(thickSurface, toReturn, compression);
        for (auto it = innerChangeset.begin(); it != innerChangeset.end(); it++) {
            toReturn.insert(*it);
        }
        return toReturn;
    }

    void mergeTwoNodes(Surface &belonging, Node* a, Node* b){
        if (a->to == b && b->from == a){
            auto aux = new Node();
            aux->coords[Graph::X] = (a->coords[Graph::X] + b->coords[Graph::X]) / 2;
            aux->coords[Graph::Y] = (a->coords[Graph::Y] + b->coords[Graph::Y]) / 2;
            aux->to = b->to;
            aux->from = a->from;
            b->from->to = aux;
            a->to->from = aux;
            for (auto it = b->correspondents.begin(); it != b->correspondents.end(); it++){
                aux->correspondents.insert(*it);
            }
            for (auto it = a->correspondents.begin(); it != a->correspondents.end(); it++){
                aux->correspondents.insert(*it);
            }
        } else if (a->from == b && b->to == a){
            auto aux = new Node();
            aux->coords[Graph::X] = (a->coords[Graph::X] + b->coords[Graph::X]) / 2;
            aux->coords[Graph::Y] = (a->coords[Graph::Y] + b->coords[Graph::Y]) / 2;
            aux->to = a->to;
            aux->from = b->from;
            b->to->from = aux;
            a->from->to = aux;
            for (auto it = b->correspondents.begin(); it != b->correspondents.end(); it++){
                aux->correspondents.insert(*it);
            }
            for (auto it = a->correspondents.begin(); it != a->correspondents.end(); it++){
                aux->correspondents.insert(*it);
            }
        }
    }

    Node* commonCorrespondent(Node *a, Node* b){
        Node* toReturn = nullptr;
        for (auto itA = a->correspondents.begin(); itA != a->correspondents.end(); itA++){
            for (auto itB = b->correspondents.begin(); itB != b->correspondents.end(); itB++){
                if (*itA == *itB)
                    toReturn = *itA;
            }
        }
        return toReturn;
    }

    std::set<Node*> getCorrespondents(double newX, double newY, Node* a, Node *b){
        std::set<Node*> toReturn;
        auto potentialCorrespondent = commonCorrespondent(a, b);
        if (potentialCorrespondent != nullptr){
            toReturn.insert(potentialCorrespondent);
        } else {
            auto closestToNewNode = [newX, newY](Node* c0, Node* c1){
                auto dist0 = MathGeometry::findNorm2d(c0->coords[Graph::X] - newX, c0->coords[Graph::Y] - newY);
                auto dist1 = MathGeometry::findNorm2d(c1->coords[Graph::X] - newX, c1->coords[Graph::Y] - newY);
                return dist0 < dist1;
            };
            auto leftCorr = std::min_element(a->correspondents.begin(), a->correspondents.end(), closestToNewNode);
            auto rightCorr = std::min_element(b->correspondents.begin(), b->correspondents.end(), closestToNewNode);
            toReturn.insert(*leftCorr);
            toReturn.insert(*rightCorr);

        }
        return toReturn;
    }

    int checkNeighbors(Node* a, Node* b){
        if (a->to == b && b->from == a)
            return 0;
        else if (b->to == a && a->from == b)
            return 1;
        else
            return 2;
    }

    // This should return a BUFFED NodeChange
    void addNode(Surface &belonging, Node* a, Node* b){
        if (checkNeighbors(a, b) == 2)
            return; // TODO: Blow up maybe?
        auto toReturn = new Node();
        toReturn->coords[Graph::X] = (a->coords[Graph::X] + b->coords[Graph::X]) / 2;
        toReturn->coords[Graph::Y] = (a->coords[Graph::Y] + b->coords[Graph::Y]) / 2;
        toReturn->correspondents = getCorrespondents(toReturn->coords[Graph::X], toReturn->coords[Graph::Y], a, b);
        if (checkNeighbors(a, b) == 1){
            toReturn->to = b;
            toReturn->from = a;
        }
    }
}