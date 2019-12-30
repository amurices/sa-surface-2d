//
// Created by Andre Muricy on 2019-12-30.
//

#ifndef SA_SURFACE_2D_GRAPHSURFACE_HPP
#define SA_SURFACE_2D_GRAPHSURFACE_HPP

#include <vector>
#include <set>

namespace Graph {
    const int DEFAULT_DIMENSIONS = 2;
    const int X = 0;
    const int Y = 1;

    struct Node {
        std::vector<double> coords; // this is a vector of length `number_of_dimensions`
        std::vector<Node *> correspondents; // This will be a vector of length 1 for now
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
        Node *node;
        double changeX;
        double changeY;

        NodeChange(Node *n, double x, double y) {
            node = n;
            changeX = x;
            changeY = y;
        };

        bool operator<(const struct NodeChange &otherChange) const {
            return (node < otherChange.node |
                    changeX < otherChange.changeX |
                    changeY < otherChange.changeY);
        }
    };


    struct Surface {
        std::vector<Node *> nodes;
    };

    const int DEFAULT_LAYERS = 2;
    const int OUTER = 0;
    const int INNER = 1;

    struct ThickSurface2 {
        std::vector<Surface> layers;

        ThickSurface2() {
            layers.resize(DEFAULT_LAYERS);
        }
    };

    Surface generateCircularGraph(double centerX, double centerY, double radius, int pts);

    std::set<NodeChange> smoothAdjacentNodes(const Surface &surface, NodeChange initialChange, int smoothness,
                                             double (*f)(double idk, double idk2));

    std::set<NodeChange> changesetForNodes(const Surface &surface, const std::vector<Graph::Node*> &nodesToPush,
                                           double forceOffsetRange, int smoothness,
                                           double (*f)(double idk1, double idk2));

    std::set<NodeChange> innerChangesetFromOuterChangeset(const ThickSurface2 &thickSurface, const std::set<NodeChange> &outerChanges,
                                                          double compression);

    std::vector<Graph::Node*> randomNodes(const Graph::Surface& surface, double multiProb);

    std::set<NodeChange> generateChangesetForOuterNodes(const ThickSurface2 &thickSurface, const std::vector<Graph::Node*> &outerNodes, double compression,
                                                        double forceOffsetRange, double multiProb,
                                                        int smoothness, double (*f)(double idk1, double idk2));

    void applyNodeChanges(std::set <Graph::NodeChange> &changes);

    double surfaceArea(const Surface &surface);

    ThickSurface2 generateCircularThicksurface(double centerX, double centerY, double outerRadius, double initialThickness, int pts);
}

#endif //SA_SURFACE_2D_GRAPHSURFACE_HPP
