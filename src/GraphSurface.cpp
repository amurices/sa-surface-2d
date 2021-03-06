//
// Created by Andre Muricy on 2019-12-30.
//

#include <GraphSurface.hpp>
#include <MathGeometry.hpp>
#include <Util.hpp>
#include <cmath>
#include <iostream>
#include <iomanip>

namespace Graph {
    std::ostream &operator<<(std::ostream &os, const Node *n) {
        os << "(" << n->coords[Graph::X] << ", " << n->coords[Graph::Y] << "). corrs: " << n->correspondents.size();
        os << ", at\n";
        for (auto it = n->correspondents.begin(); n->correspondents.end() != it; it++)
            os << "\t(" << (*it)->coords[Graph::X] << ", " << (*it)->coords[Graph::Y] << ")\n";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const std::set<Node *> &n) {
        for (auto it = n.begin(); n.end() != it; it++)
            os << "\t(" << (*it)->coords[Graph::X] << ", " << (*it)->coords[Graph::Y] << ")\n";
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
    smoothAdjacentNodes(Surface &surface, NodeChange initialChange, int smoothness,
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
            NodeChange prevChange(prev,
                                  prev->coords[Graph::X] + (initChangeXDelta * ratio),
                                  prev->coords[Graph::Y] + (initChangeYDelta * ratio),
                                  prev->coords[Graph::X], prev->coords[Graph::Y],
                                  prev->to, prev->from, &surface);
            NodeChange nextChange(next,
                                  next->coords[Graph::X] + (initChangeXDelta * ratio),
                                  next->coords[Graph::Y] + (initChangeYDelta * ratio),
                                  next->coords[Graph::X], next->coords[Graph::Y],
                                  next->to, next->from, &surface);
            // Previous and next nodes are also going to be altered as a matter of where they are in relation to their inner correspondents
            toReturn.insert(prevChange);
            toReturn.insert(nextChange);
        }
        return toReturn;
    }

    std::map<Node *, NodeChange2>
    smoothAdjacentNodes2(Surface &surface, Node *changedNode, NodeChange2 initialChange, int smoothness,
                         double (*f)(double, double)) {
        std::map<Node *, NodeChange2> toReturn{
                {changedNode, initialChange}
        };

        Node *prev, *next;
        prev = next = changedNode;

        for (int c = 1; c <= smoothness; c++) {
            prev = prev->from;
            next = next->to;

            double ratio = (*f)(smoothness, c);
            auto initChangeXDelta = initialChange.newX - initialChange.prevX;
            auto initChangeYDelta = initialChange.newY - initialChange.prevY;
            NodeChange2 prevChange(prev->coords[Graph::X] + (initChangeXDelta * ratio),
                                   prev->coords[Graph::Y] + (initChangeYDelta * ratio),
                                   prev->coords[Graph::X], prev->coords[Graph::Y],
                                   &surface);
            NodeChange2 nextChange(next->coords[Graph::X] + (initChangeXDelta * ratio),
                                   next->coords[Graph::Y] + (initChangeYDelta * ratio),
                                   next->coords[Graph::X], next->coords[Graph::Y],
                                   &surface);
            // Previous and next nodes are also going to be altered as a matter of where they are in relation to their inner correspondents
            toReturn.insert(std::make_pair(next, nextChange));
            toReturn.insert(std::make_pair(prev, prevChange));
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

    double surfacePerimeter(const Surface &surface) {
        Node *it = surface.nodes[0];
        double acc = 0.0;
        do {
            acc += MathGeometry::findNorm2d(it->coords[Graph::X] - it->to->coords[Graph::X],
                                            it->coords[Graph::Y] - it->to->coords[Graph::Y]);
            it = it->to;
        } while (it != surface.nodes[0]);

        return acc;
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
    changesetForNodes(Surface &surface, const std::vector<Node *> &nodesToPush,
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
                                           (*it)->coords[Graph::Y],
                                           (*it)->to, (*it)->from,
                                           &surface);
            toReturn = smoothAdjacentNodes(surface, randomChange, smoothness, f);
        }
        return toReturn;
    }

    std::map<Node *, NodeChange2>
    changesetForNodes2(Surface &surface, const std::vector<Node *> &nodesToPush,
                       double forceOffsetRange, int smoothness,
                       double (*f)(double, double)) {
        std::map<Node *, NodeChange2> toReturn;
        for (auto it = nodesToPush.begin(); it != nodesToPush.end(); it++) {
            // Choose a random offset, bounded by optimizer params
            double offsetX, offsetY;
            offsetX = Util::getRandomRange(-forceOffsetRange, forceOffsetRange);
            offsetY = Util::getRandomRange(-forceOffsetRange, forceOffsetRange);

            // Collect change to outer node's position.
            auto randomChange = NodeChange2((*it)->coords[Graph::X] + offsetX,
                                            (*it)->coords[Graph::Y] + offsetY,
                                            (*it)->coords[Graph::X],
                                            (*it)->coords[Graph::Y],
                                            &surface);
            toReturn = smoothAdjacentNodes2(surface, (*it), randomChange, smoothness, f);
        }
        return toReturn;
    }

/* This will assume the original changeset is in the outer surface, which means the changeset returned is
 * based on "new" coordinates of the outer surface. */
    std::set<NodeChange> innerChangesetFromOuterChangeset(ThickSurface &thickSurface,
                                                          const std::set<NodeChange> &outerChanges,
                                                          double compression) {
        std::set<NodeChange> toReturn;
        for (auto it = outerChanges.begin(); it != outerChanges.end(); it++) {
            // Changed nodes have their correspondents updated one at a time
            Node *fnode = (*it).node;
            // Just to not have to reimplement findDirectionVector
            MathGeometry::point_t pfrom(fnode->from->coords[Graph::X], fnode->from->coords[Graph::Y]);
            MathGeometry::point_t pto(fnode->to->coords[Graph::X], fnode->to->coords[Graph::Y]);
            // A little less obscene, but still in need of some cleaning up.
            auto distance = MathGeometry::findNorm2d(
                    fnode->coords[Graph::X] - (*fnode->correspondents.begin())->coords[Graph::X],
                    fnode->coords[Graph::Y] - (*fnode->correspondents.begin())->coords[Graph::Y]);
            MathGeometry::point_t vd2 = MathGeometry::findDirectionVector2(
                    MathGeometry::point_t(fnode->coords[Graph::X],
                                          fnode->coords[Graph::Y]),
                    pto,
                    pfrom);
            auto delta =
                    MathGeometry::point_t(it->newX, it->newY) // current new outer position
                    + (vd2 * distance) // plus direction (unit vector) times original distance = new position
                    - MathGeometry::point_t((*fnode->correspondents.begin())->coords[Graph::X],
                                            (*fnode->correspondents.begin())->coords[Graph::Y]); // because this will be added, subtract current position
            // TODO: Sum below for each correspondent. Damn dis shit good
            for (auto summin = fnode->correspondents.begin(); summin != fnode->correspondents.end(); summin++) {
                toReturn.insert(
                        NodeChange(
                                (*summin),
                                (*summin)->coords[Graph::X] + delta.x * compression,
                                (*summin)->coords[Graph::Y] + delta.y * compression,
                                (*summin)->coords[Graph::X],
                                (*summin)->coords[Graph::Y],
                                (*summin)->to,
                                (*summin)->from,
                                &thickSurface.layers[Graph::INNER]
                        )
                );
            }
        }
        return toReturn;
    }

    MathGeometry::point_t innerDelta(Node *n, NodeChange2 change) {
        MathGeometry::point_t pfrom(n->from->coords[Graph::X], n->from->coords[Graph::Y]);
        MathGeometry::point_t pto(n->to->coords[Graph::X], n->to->coords[Graph::Y]);
        // A little less obscene, but still in need of some cleaning up.
        auto distance = MathGeometry::findNorm2d(
                n->coords[Graph::X] - (*n->correspondents.begin())->coords[Graph::X],
                n->coords[Graph::Y] - (*n->correspondents.begin())->coords[Graph::Y]);
        MathGeometry::point_t vd2 = MathGeometry::findDirectionVector2(
                MathGeometry::point_t(n->coords[Graph::X],
                                      n->coords[Graph::Y]),
                pto,
                pfrom);
        return
                MathGeometry::point_t(change.newX, change.newY) // current new outer position
                + (vd2 * distance) // plus direction (unit vector) times original distance = new position
                - MathGeometry::point_t((*n->correspondents.begin())->coords[Graph::X],
                                        (*n->correspondents.begin())->coords[Graph::Y]); // because this will be added, subtract current position
    }

    std::map<Node *, std::set<NodeChange2>> innerChangesetFromOuterChangeset2(ThickSurface &thickSurface,
                                                                              const std::map<Node *, NodeChange2> &outerChanges,
                                                                              double compression) {
        std::map<Node *, std::set<NodeChange2>> toReturn;
        for (auto change = outerChanges.begin(); change != outerChanges.end(); change++) {
            // Changed nodes have their correspondents updated one at a time
            Node *fnode = change->first;
            auto delta = innerDelta(fnode, change->second);
            auto newChange = NodeChange2(
                    fnode->coords[Graph::X] + delta.x * compression,
                    fnode->coords[Graph::Y] + delta.y * compression,
                    fnode->coords[Graph::X],
                    fnode->coords[Graph::Y],
                    &thickSurface.layers[Graph::INNER]
            );
            for (auto eachCorr = change->first->correspondents.begin();
                 eachCorr != change->first->correspondents.end(); eachCorr++) {
                auto itN = toReturn.find(*eachCorr);
                if (itN != toReturn.end())
                    itN->second.insert(newChange);
                else
                    toReturn.insert(std::make_pair(fnode, std::set<NodeChange2>{newChange}));
            }
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
    generateTotalChangesetFromPushedOuterNodes(ThickSurface &thickSurface,
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

    std::map<Node *, std::set<NodeChange2>>
    generateTotalChangesetFromPushedOuterNodes2(ThickSurface &thickSurface,
                                                const std::vector<Node *> &outerNodes, double compression,
                                                double forceOffsetRange,
                                                double multiProb, int smoothness,
                                                double (*f)(double, double)) {
        auto toReturn = changesetForNodes2(thickSurface.layers[Graph::OUTER], outerNodes, forceOffsetRange, smoothness, f);
        auto innerChangeset = innerChangesetFromOuterChangeset2(thickSurface, toReturn, compression);
        for (auto it = toReturn.begin(); it != toReturn.end(); it++) {
            innerChangeset.insert(std::make_pair(it->first, std::set<NodeChange2> { it->second }));
        }
        return innerChangeset;
    }

}