//
// Created by Andre Muricy on 2019-12-30.
//

#include <GraphSurface.hpp>
#include <MathGeometry.hpp>
#include <Util.hpp>
#include <cmath>
#include <iostream>
qgitq
namespace Graph {
    std::ostream &operator<<(std::ostream &os, const NodeChange &nc) {
        os << "node: " << nc.node << "\n" <<
           "newX: " << nc.newX << "\n" <<
           "newY: " << nc.newY << "\n" <<
           "prevX: " << nc.prevX << "\n" <<
           "prevY: " << nc.prevY << "\n" <<
           "newTo: " << nc.newTo << "\n" <<
           "newFrom: " << nc.newFrom << "\n" <<
           "prevTo: " << nc.prevTo << "\n" <<
           "prevFrom: " << nc.prevFrom << "\n" <<
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
            auto randomChange = NodeChange{*it,
                                           (*it)->coords[Graph::X] + offsetX,
                                           (*it)->coords[Graph::Y] + offsetY,
                                           (*it)->coords[Graph::X],
                                           (*it)->coords[Graph::Y], true, true};
            toReturn = smoothAdjacentNodes(surface, randomChange, smoothness, f);
        }
        return toReturn;
    }

    void applyNodeChanges(std::set<NodeChange> &changes) {
        for (auto it = changes.begin(); it != changes.end(); it++) {
            if (it->existed && !it->willExist) {
                // This wont work because applyNodeChanges runs before "committing". We have to batch this up for deletion later
                delete it->node;
            } else if (!it->existed && it->willExist) {
                it->whoHasIt->nodes.push_back(it->node);
                it->node->coords[Graph::X] = it->newX;
                it->node->coords[Graph::Y] = it->newY;
                it->node->to = it->newTo;
                it->node->from = it->newFrom;
            } else {
                it->node->coords[Graph::X] = it->newX;
                it->node->coords[Graph::Y] = it->newY;
                it->node->to = it->newTo;
                it->node->from = it->newFrom;
            }
        }
    }

    void revertNodeChanges(std::set<NodeChange> &changes) {
        for (auto it = changes.begin(); it != changes.end(); it++) {
            if (it->existed && !it->willExist) {
                // Recover from the batch, which we'll have to pass somehow
            } else if (!it->existed && it->willExist){
                for (auto deletable = it->whoHasIt->nodes.begin(); deletable != it->whoHasIt->nodes.end(); deletable++){
                    if (*deletable == it->node){ it->whoHasIt->nodes.erase(deletable); break; }
                }
                delete it->node;
            }
            else {
                it->node->coords[Graph::X] = it->prevX;
                it->node->coords[Graph::Y] = it->prevY;
                it->node->to = it->prevTo;
                it->node->from = it->prevFrom;
            }
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
                                          fnode->coords[Graph::Y] -
                                          (*fnode->correspondents.begin())->coords[Graph::Y]));
            MathGeometry::point_t vd = MathGeometry::findDirectionVector(pPrev, pNext,
                                                                         MathGeometry::point_t(fnode->coords[Graph::X],
                                                                                               fnode->coords[Graph::Y]),
                                                                         MathGeometry::MEDIAN_ANGLE); // Get directional vector btwn inner & outer
            auto delta =
                    MathGeometry::point_t(it->newX, it->newY) // current new outer position
                    - (vd * distance) // plus direction (unit vector) times original distance = new position
                    - MathGeometry::point_t((*fnode->correspondents.begin())->coords[Graph::X],
                                            (*fnode->correspondents.begin())->coords[Graph::Y]); // because this will be added, subtract current position
            auto summin = (*fnode->correspondents.begin());
            toReturn.insert(
                    NodeChange{
                            summin,
                            summin->coords[Graph::X] + delta.x * compression,
                            summin->coords[Graph::Y] + delta.y * compression,
                            summin->coords[Graph::X],
                            summin->coords[Graph::Y],
                            true,
                            true,
                            summin->to,
                            summin->from,
                            summin->to,
                            summin->from,
                    }
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

    void mergeTwoNodes(Surface &belonging, Node *a, Node *b) {
        if (a->to == b && b->from == a) {
            auto aux = new Node();
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
            auto aux = new Node();
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

    Node *commonCorrespondent(Node *a, Node *b) {
        Node *toReturn = nullptr;
        for (auto itA = a->correspondents.begin(); itA != a->correspondents.end(); itA++) {
            for (auto itB = b->correspondents.begin(); itB != b->correspondents.end(); itB++) {
                if (*itA == *itB)
                    toReturn = *itA;
            }
        }
        return toReturn;
    }

    std::set<Node *> getCorrespondents(double newX, double newY, Node *a, Node *b, double bothCorrsDist) {
        std::set<Node *> toReturn;
        auto potentialCorrespondent = commonCorrespondent(a, b);
        if (potentialCorrespondent != nullptr) {
            toReturn.insert(potentialCorrespondent);
        } else {
            auto closestToNewNode = [newX, newY](Node *c0, Node *c1) {
                auto dist0 = MathGeometry::findNorm2d(c0->coords[Graph::X] - newX, c0->coords[Graph::Y] - newY);
                auto dist1 = MathGeometry::findNorm2d(c1->coords[Graph::X] - newX, c1->coords[Graph::Y] - newY);
                return dist0 < dist1;
            };
            auto leftCorr = std::min_element(a->correspondents.begin(), a->correspondents.end(), closestToNewNode);
            auto rightCorr = std::min_element(b->correspondents.begin(), b->correspondents.end(), closestToNewNode);
            auto leftCorrDist = MathGeometry::findNorm2d((*leftCorr)->coords[Graph::X] - newX, (*leftCorr)->coords[Graph::Y] - newY);
            auto rightCorrDist = MathGeometry::findNorm2d((*rightCorr)->coords[Graph::X] - newX, (*rightCorr)->coords[Graph::Y] - newY);
            if (leftCorrDist / rightCorrDist > bothCorrsDist && leftCorrDist / rightCorrDist < (1/bothCorrsDist)){
                std::cout << leftCorrDist / rightCorrDist << ", " << rightCorrDist / leftCorrDist << std::endl;
                toReturn.insert(*leftCorr);
                toReturn.insert(*rightCorr);
            } else
                toReturn.insert(closestToNewNode(*leftCorr, *rightCorr) ? *leftCorr : *rightCorr);
        }
        return toReturn;
    }

    int assertNeighbors(Node *a, Node *b) {
        if (a->to == b && b->from == a)
            return 0;
        else if (b->to == a && a->from == b)
            return 1;

        std::cout << "Nodes arent neighbors; a: " << a << ", b: " << b << std::endl <<
        "a->to: " << a->to << " a->from: "<< a->from << std::endl <<
        "b->to: " << b->to << " b->from: "<< b->from << std::endl;
        exit(0);
    }

    std::set<NodeChange> addNode(Surface* belonging, Node *a, Node *b) {
        std::set<NodeChange> toReturn;
        auto neighborlyStatus = assertNeighbors(a, b);
        if (neighborlyStatus == 2){
            printf("Neighborly status 2; a: %d, b: %d\na->to: %d, a->from: %d\nb->to: %d, b->from: %d\n", a, b, a->to, a->from, b->to, b->from);
            exit(0);
        }
        auto nodeToBeCommitted = new Node();
        toReturn.insert(NodeChange{nodeToBeCommitted,
                                   (a->coords[Graph::X] + b->coords[Graph::X]) / 2, // newX; will exist here
                                   (a->coords[Graph::Y] + b->coords[Graph::Y]) / 2, // newY; will exist here
                                   0.0, // prevX; dunmatter
                                   0.0, // prevY; dunmatter
                                   false, // existed
                                   true, // willExist
                                   neighborlyStatus == 1 ? a : b,
                                   neighborlyStatus == 1 ? b : a,
                                   nullptr,
                                   nullptr
                /* TODO: newCorrespondents */});
        toReturn.insert(NodeChange{a, a->coords[Graph::X], a->coords[Graph::Y], a->coords[Graph::X], a->coords[Graph::Y], true, true,
                                   neighborlyStatus == 1 ? a->to : nodeToBeCommitted,
                                   neighborlyStatus == 1 ? nodeToBeCommitted : a->from,
                                   a->to, a->from});
        toReturn.insert(NodeChange{b, b->coords[Graph::X], b->coords[Graph::Y], b->coords[Graph::X], b->coords[Graph::Y], true, true,
                                   neighborlyStatus == 0 ? b->to : nodeToBeCommitted,
                                   neighborlyStatus == 0 ? nodeToBeCommitted : b->from,
                                   b->to, b->from});
        return toReturn;
    }

    void addNode2(Surface* belonging, Node *a, Node *b, double bothCorrsDist) {
        auto neighborlyStatus = assertNeighbors(a, b);

        auto nodeToBeCommitted = new Node();
        nodeToBeCommitted->coords[Graph::X] =  (a->coords[Graph::X] + b->coords[Graph::X]) / 2;
        nodeToBeCommitted->coords[Graph::Y] =  (a->coords[Graph::Y] + b->coords[Graph::Y]) / 2;
        nodeToBeCommitted->to = neighborlyStatus == 1 ? a : b;
        nodeToBeCommitted->from = neighborlyStatus == 1 ? b : a;
        nodeToBeCommitted->correspondents = getCorrespondents(nodeToBeCommitted->coords[Graph::X], nodeToBeCommitted->coords[Graph::Y], a, b, bothCorrsDist);
        for (auto it = nodeToBeCommitted->correspondents.begin(); it != nodeToBeCommitted->correspondents.end(); it++){
            (*it)->correspondents.insert(*it);
        }
        belonging->nodes.push_back(nodeToBeCommitted);

        a->to = neighborlyStatus == 1 ? a->to : nodeToBeCommitted;
        a->from = neighborlyStatus == 1 ? nodeToBeCommitted : a->from;

        b->to = neighborlyStatus == 0 ? b->to : nodeToBeCommitted;
        b->from = neighborlyStatus == 0 ? nodeToBeCommitted : b->from;
    }
}