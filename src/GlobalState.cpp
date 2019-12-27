//
// Contains every reference to any global state.
//
#include <GlobalState.hpp>
#include <vector>
#include <MathGeometry.hpp>
#include <Util.hpp>

Graph::ThickSurface2 GlobalState::thickSurface;
GlobalState::SurfaceParameters GlobalState::surfaceParameters;
GlobalState::OptimizerParameters GlobalState::optimizerParameters;

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

std::set<Graph::NodeChange> Graph::neighborOuterChangeset(const Graph::Surface &surface, double multiProb,
                                                          double forceOffsetRange, int smoothness,
                                                          double (*f)(double idk1, double idk2)) {
    std::set<Graph::NodeChange> toReturn;
    std::vector<int> randomIndexes;
    // First, choose indices that will be modified by random neighbor search
    double coinFlip;
    do {
        int randomIndex = rand() % surface.nodes.size();
        randomIndexes.push_back(randomIndex);
        coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    } while (coinFlip < multiProb);

    for (int i = 0; i < randomIndexes.size(); i++) {
        Graph::Node *randomNode = surface.nodes[randomIndexes[i]];

        // Choose a random offset, bounded by optimizer params
        double offsetX, offsetY;
        offsetX = Util::getRandomRange(-forceOffsetRange, forceOffsetRange);
        offsetY = Util::getRandomRange(-forceOffsetRange, forceOffsetRange);

        // Collect change to outer node's position.
        auto randomChange = Graph::NodeChange(randomNode, offsetX, offsetY);
        toReturn = Graph::smoothAdjacentNodes(surface, randomChange, smoothness, f);
    }
    return toReturn;
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
        // Dont know if the below is right, but basically, we find the direction we should push an inner node in via the old method,
        // then push it in that direction preserving the distance to the outer node, multiplied by an additional parameter compression, which is basically softness
        // ÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆ
        auto distance = MathGeometry::findNorm(point_t(fnode->coords[Graph::X] - fnode->correspondents[0]->coords[Graph::X],
                                                       fnode->coords[Graph::Y] - fnode->correspondents[0]->coords[Graph::Y]));
        auto distanceWithChange = MathGeometry::findNorm(point_t(fnode->coords[Graph::X] + it->changeX - fnode->correspondents[0]->coords[Graph::X],
                                                                 fnode->coords[Graph::Y] + it->changeY - fnode->correspondents[0]->coords[Graph::Y]));
        point_t vd = MathGeometry::findDirectionVector(pPrev, pNext,
                                                       point_t(fnode->coords[Graph::X], fnode->coords[Graph::Y]),
                                                       MathGeometry::MEDIAN_ANGLE); // Get directional vector btwn inner & outer
        double changeNorm = MathGeometry::findNorm(point_t(it->changeX, it->changeY));
        toReturn.insert(Graph::NodeChange(fnode->correspondents[0], - vd.x * changeNorm * compression,
                                          - vd.y * changeNorm * compression));
    }
    return toReturn;
}

std::set<Graph::NodeChange>
Graph::neighborChangeset(const Graph::ThickSurface2 &thickSurface, double compression, double forceOffsetRange,
                         double multiProb, int smoothness, double (*f)(double idk1, double idk2)) {
    std::set<Graph::NodeChange> toReturn;
    toReturn = neighborOuterChangeset(thickSurface.layers[Graph::OUTER], multiProb, forceOffsetRange, smoothness, f);
    auto innerChangeset = innerChangesetFromOuterChangeset(thickSurface, toReturn, compression);
    for (auto it = innerChangeset.begin(); it != innerChangeset.end(); it++) {
        toReturn.insert(*it);
    }
    return toReturn;
}


void GlobalState::setSurfaceParameters(double radius, double thickness, double centerX, double centerY, int points) {
    GlobalState::surfaceParameters.radius = radius;
    GlobalState::surfaceParameters.thickness = thickness;
    GlobalState::surfaceParameters.centerX = centerX;
    GlobalState::surfaceParameters.centerY = centerY;
    GlobalState::surfaceParameters.points = points;
}

void GlobalState::setOptimizerParameters(double initialGrayMatter, int smoothness, double diffMul, double diffPow,
                                         double areaMul, double areaPow, double multiProb, double tempProb,
                                         double forceOffsetRange, double compression,
                                         double (*smoothnessFunction)(double, double), double temperature) {
    GlobalState::optimizerParameters.initialGrayMatter = initialGrayMatter;
    GlobalState::optimizerParameters.smoothness = smoothness;
    GlobalState::optimizerParameters.diffMul = diffMul;
    GlobalState::optimizerParameters.diffPow = diffPow;
    GlobalState::optimizerParameters.areaMul = areaMul;
    GlobalState::optimizerParameters.areaPow = areaPow;
    GlobalState::optimizerParameters.multiProb = multiProb;
    GlobalState::optimizerParameters.tempProb = tempProb;
    GlobalState::optimizerParameters.forceOffsetRange = forceOffsetRange;
    GlobalState::optimizerParameters.compression = compression;
    GlobalState::optimizerParameters.smoothnessFunction = smoothnessFunction;
    GlobalState::optimizerParameters.temperature = temperature;
}

void GlobalState::initThickSurface() {
    GlobalState::thickSurface = Graph::generateCircularThicksurface(surfaceParameters.centerX,
                                                                    surfaceParameters.centerY, surfaceParameters.radius,
                                                                    surfaceParameters.thickness,
                                                                    surfaceParameters.points);
}
