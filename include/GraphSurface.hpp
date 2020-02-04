//
// Created by Andre Muricy on 2019-12-30.
//

#ifndef SA_SURFACE_2D_GRAPHSURFACE_HPP
#define SA_SURFACE_2D_GRAPHSURFACE_HPP

#include <vector>
#include <set>
#include <iostream>

namespace Graph {
    const int DEFAULT_DIMENSIONS = 2;
    const int X = 0;
    const int Y = 1;

    struct Node {
        std::vector<double> coords; // this is a vector of length `number_of_dimensions`
        std::set<Node *> correspondents; // This will be a vector of length 1 for now
        Node *to = nullptr;
        Node *from = nullptr;

        Node() {
            coords.resize(DEFAULT_DIMENSIONS);
        }
    };

    struct Line {
        Node* a;
        Node* b;
    };

    typedef std::vector<Line*> Intersectables;

    struct NodeChange {
        // TODO: newNext, newPrev, prevNext, prevPrev (rsrs)
        Node *node;
        double newX;
        double newY;
        double prevX;
        double prevY;
        bool existed;
        bool willExist;
        NodeChange(Node* n, double nX, double nY, double pX, double pY, bool ex, bool wex){
            node = n; newX = nX; newY = nY; prevX = pX; prevY = pY; existed = ex; willExist = wex;
        }
        bool operator<(const struct NodeChange &otherChange) const {
            return (node < otherChange.node |
                    newX < otherChange.newX |
                    newY < otherChange.newY |
                    prevX < otherChange.prevX |
                    prevY < otherChange.prevY |
                    existed < otherChange.existed |
                    willExist < otherChange.willExist);
        }

    };

    std::ostream& operator<<(std::ostream& os, const NodeChange& nc);

    struct Surface {
        std::vector<Node *> nodes;
    };

    const int DEFAULT_LAYERS = 2;
    const int OUTER = 0;
    const int INNER = 1;

    struct ThickSurface {
        std::vector<Surface> layers;

        ThickSurface() {
            layers.resize(DEFAULT_LAYERS);
        }
    };

    Surface generateCircularGraph(double centerX, double centerY, double radius, int pts);

    std::set<NodeChange> smoothAdjacentNodes(const Surface &surface, NodeChange initialChange, int smoothness,
                                             double (*f)(double, double));

    std::set<NodeChange> changesetForNodes(const Surface &surface, const std::vector<Graph::Node*> &nodesToPush,
                                           double forceOffsetRange, int smoothness,
                                           double (*f)(double, double));

    std::set<NodeChange> innerChangesetFromOuterChangeset(const ThickSurface &thickSurface, const std::set<NodeChange> &outerChanges,
                                                          double compression);

    std::vector<Graph::Node*> randomNodes(const Graph::Surface& surface, double multiProb);

    std::set<NodeChange> generateTotalChangesetFromPushedOuterNodes(const ThickSurface &thickSurface,
                                                                    const std::vector<Graph::Node *> &outerNodes,
                                                                    double compression,
                                                                    double forceOffsetRange, double multiProb,
                                                                    int smoothness,
                                                                    double (*f)(double, double));

    void applyNodeChanges(std::set <Graph::NodeChange> &changes);

    double surfaceArea(const Surface &surface);

    ThickSurface generateCircularThicksurface(double centerX, double centerY, double outerRadius, double initialThickness, int pts);

    void mergeTwoNodes(Graph::Surface &belonging, Graph::Node* a, Graph::Node* b);

    // Add node between A and B
    void addNode(Graph::Surface &belonging, Graph::Node* a, Graph::Node* b);
}

#endif //SA_SURFACE_2D_GRAPHSURFACE_HPP
