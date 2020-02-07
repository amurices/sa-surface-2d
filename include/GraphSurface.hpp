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
        Node *a;
        Node *b;
        enum LineType{
            INTERLAYER,
            INTRALAYER
        };
        LineType type;
    };

    typedef std::vector<Line *> Intersectables;

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

    struct NodeChange {
        Node *node;
        double newX;
        double newY;
        double prevX;
        double prevY;
        bool existed;
        bool willExist;
        Node *newTo;
        Node *newFrom;
        Node *prevTo;
        Node *prevFrom;
        Surface *whoHasIt; // only useful for add/delete operations

        NodeChange (Node *n, double nX, double nY, double pX, double pY, Node *nt, Node *nf, Surface* whi, Node* pt = nullptr, Node *pf = nullptr, bool e = true, bool we = true){
            node = n; newX = nX; newY = nY; prevX = pX; prevY = pY; newTo = nt; newFrom = nf; whoHasIt = whi; prevTo = pt; prevFrom = pf; existed = e; willExist = we;
        }
        bool operator<(const struct NodeChange &otherChange) const {
            return (node < otherChange.node |
                    newX < otherChange.newX |
                    newY < otherChange.newY);
        }

    };
    std::ostream &operator<<(std::ostream &os, const NodeChange &nc);

    Surface generateCircularGraph(double centerX, double centerY, double radius, int pts);

    std::set<NodeChange> smoothAdjacentNodes(Surface &surface, NodeChange initialChange, int smoothness,
                                             double (*f)(double, double));

    std::set<NodeChange> changesetForNodes(Surface &surface, const std::vector<Graph::Node *> &nodesToPush,
                                           double forceOffsetRange, int smoothness,
                                           double (*f)(double, double));

    std::set<NodeChange>
    innerChangesetFromOuterChangeset(ThickSurface &thickSurface, const std::set<NodeChange> &outerChanges,
                                     double compression);

    std::vector<Graph::Node *> randomNodes(const Graph::Surface &surface, double multiProb);

    std::set<NodeChange> generateTotalChangesetFromPushedOuterNodes(ThickSurface &thickSurface,
                                                                    const std::vector<Graph::Node *> &outerNodes,
                                                                    double compression,
                                                                    double forceOffsetRange, double multiProb,
                                                                    int smoothness,
                                                                    double (*f)(double, double));

    double surfaceArea(const Surface &surface);
    double surfacePerimeter(const Surface &surface);

    ThickSurface
    generateCircularThicksurface(double centerX, double centerY, double outerRadius, double initialThickness, int pts);
}

#endif //SA_SURFACE_2D_GRAPHSURFACE_HPP
