//
// Created by Andre Muricy on 2019-12-30.
//

#include <GraphSurface.hpp>
#include <MathGeometry.hpp>
#include <Util.hpp>

Graph::Surface Graph::generateCircularGraph(double centerX, double centerY, double radius, int pts) {
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

std::set<Graph::NodeChange>
Graph::smoothAdjacentNodes(const Graph::Surface &surface, Graph::NodeChange initialChange, int smoothness,
                           double (*f)(double idk, double idk2)) {
    std::set<Graph::NodeChange> toReturn;
    toReturn.insert(initialChange);

    Graph::Node *prev, *next;
    prev = next = initialChange.node;

    for (int c = 1; c <= smoothness; c++) {
        prev = prev->from;
        next = next->to;

        double ratio = (*f)(smoothness, c);
        Graph::NodeChange prevChange{prev, initialChange.changeX * ratio, initialChange.changeY * ratio};
        Graph::NodeChange nextChange{next, initialChange.changeX * ratio, initialChange.changeY * ratio};

        // Previous and next nodes are also going to be altered as a matter of where they are in relation to their inner correspondents
        toReturn.insert(prevChange);
        toReturn.insert(nextChange);
    }
    return toReturn;
}

double Graph::surfaceArea(const Graph::Surface &surface) {
    double toReturn = 0;
    Graph::Node *prev, *next, *no;
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

Graph::ThickSurface2
Graph::generateCircularThicksurface(double centerX, double centerY, double outerRadius, double initialThickness,
                                    int pts) {
    Graph::ThickSurface2 toReturn;
    toReturn.layers[Graph::OUTER] = Graph::generateCircularGraph(centerX, centerY, outerRadius, pts);
    toReturn.layers[Graph::INNER] = Graph::generateCircularGraph(centerX, centerY, outerRadius - initialThickness, pts);

    /* One of the only places we can depend on nodes' indices */
    for (int i = 0; i < toReturn.layers[Graph::OUTER].nodes.size(); i++) {
        toReturn.layers[Graph::OUTER].nodes[i]->correspondents.push_back(
                toReturn.layers[Graph::INNER].nodes[i]
        );
        toReturn.layers[Graph::INNER].nodes[i]->correspondents.push_back(
                toReturn.layers[Graph::OUTER].nodes[i]
        );
    }
    return toReturn;
}

std::set<Graph::NodeChange>
Graph::changesetForNodes(const Graph::Surface &surface, const std::vector<Graph::Node *> &nodesToPush,
                         double forceOffsetRange, int smoothness,
                         double (*f)(double, double)) {
    std::set<Graph::NodeChange> toReturn;
    for (auto it = nodesToPush.begin(); it != nodesToPush.end(); it++) {
        // Choose a random offset, bounded by optimizer params
        double offsetX, offsetY;
        offsetX = Util::getRandomRange(-forceOffsetRange, forceOffsetRange);
        offsetY = Util::getRandomRange(-forceOffsetRange, forceOffsetRange);

        // Collect change to outer node's position.
        auto randomChange = Graph::NodeChange(*it, offsetX, offsetY);
        toReturn = Graph::smoothAdjacentNodes(surface, randomChange, smoothness, f);
    }
    return toReturn;
}

void Graph::applyNodeChanges(std::set <Graph::NodeChange> &changes){
    for (auto it = changes.begin(); it != changes.end(); it++){
        it->node->coords[Graph::X] += it->changeX;
        it->node->coords[Graph::Y] += it->changeY;
    }
}

/* This will assume the original changeset is in the outer surface, which means the changeset returned is
 * based on "new" coordinates of the outer surface. */
std::set<Graph::NodeChange> Graph::innerChangesetFromOuterChangeset(const Graph::ThickSurface2 &thickSurface,
                                                                    const std::set<Graph::NodeChange> &outerChanges,
                                                                    double compression) {
    std::set<Graph::NodeChange> toReturn;
    for (auto it = outerChanges.begin(); it != outerChanges.end(); it++) {
        // Changed nodes have their correspondents updated one at a time
        Graph::Node *fnode = (*it).node;
        double nextX = fnode->to->coords[Graph::X];
        double nextY = fnode->to->coords[Graph::Y];
        double prevX = fnode->from->coords[Graph::X];
        double prevY = fnode->from->coords[Graph::Y];
        // Just to not have to reimplement findDirectionVector
        point_t pPrev(prevX, prevY);
        point_t pNext(nextX, nextY);
        // A little less obscene, but still in need of some cleaning up.
        auto distance = MathGeometry::findNorm(
                point_t(fnode->coords[Graph::X] - fnode->correspondents[0]->coords[Graph::X],
                        fnode->coords[Graph::Y] - fnode->correspondents[0]->coords[Graph::Y]));
        point_t vd = MathGeometry::findDirectionVector(pPrev, pNext,
                                                       point_t(fnode->coords[Graph::X], fnode->coords[Graph::Y]),
                                                       MathGeometry::MEDIAN_ANGLE); // Get directional vector btwn inner & outer
        auto delta =
                point_t(fnode->coords[Graph::X] + it->changeX, fnode->coords[Graph::Y] + it->changeY) // current new outer position
                - (vd * distance) // plus direction (unit vector) times original distance = new position
                - point_t(fnode->correspondents[0]->coords[Graph::X], fnode->correspondents[0]->coords[Graph::Y]); // because this will be added, subtract current position
        toReturn.insert(Graph::NodeChange(fnode->correspondents[0], delta.x * compression, delta.y * compression));
    }
    return toReturn;
}

std::vector<Graph::Node *> Graph::randomNodes(const Graph::Surface &surface, double multiProb) {
    std::vector<Graph::Node *> toReturn;
    double coinFlip;
    do {
        int randomIndex = rand() % surface.nodes.size();
        toReturn.push_back(surface.nodes[randomIndex]);
        coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    } while (coinFlip < multiProb);
    return toReturn;
}

std::set<Graph::NodeChange>
Graph::generateChangesetForOuterNodes(const Graph::ThickSurface2 &thickSurface, const std::vector<Graph::Node*> &outerNodes, double compression,
                                      double forceOffsetRange,
                                      double multiProb, int smoothness, double (*f)(double idk1, double idk2)) {
    std::set<Graph::NodeChange> toReturn;
    toReturn = changesetForNodes(thickSurface.layers[Graph::OUTER], outerNodes, forceOffsetRange, smoothness, f);
    auto innerChangeset = innerChangesetFromOuterChangeset(thickSurface, toReturn, compression);
    for (auto it = innerChangeset.begin(); it != innerChangeset.end(); it++) {
        toReturn.insert(*it);
    }
    return toReturn;
}
