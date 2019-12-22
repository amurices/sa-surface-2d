//
// Contains every reference to any global state.
//
#include <GlobalState.hpp>
#include <vector>
#include <MathGeometry.hpp>

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

std::set<Graph::NodeChange> Graph::smoothAdjacentNodes(const Graph::Surface &surface, Graph::NodeChange initialChange,  int smoothness, double (*f)(double idk, double idk2)){
    std::set<Graph::NodeChange> toReturn;
    toReturn.insert(initialChange);

    Graph::Node *prev, *next;
    prev = next = initialChange.node;

    for (int c = 1; c <= smoothness; c++)
    {
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

std::vector<double> Graph::surfaceAreaAndPerimeter(const Graph::Surface &surface){
    std::vector<double> toReturn(2, 0);

    Graph::Node *prev, *next, *no;
    no = surface.nodes[0];
    do
    {
        prev = no->from;
        next = no->to;

        // Perimeter calculation: -----------------------------------------------------
        double perimVectorX = no->coords[Graph::X] - prev->coords[Graph::X];
        double perimVectorY = no->coords[Graph::Y] - prev->coords[Graph::Y];
        double perimVectorNorm = MathGeometry::findNorm2d(perimVectorX, perimVectorY);
        toReturn[1] += perimVectorNorm;
        // End of perimeter calculation -----------------------------------------------

        toReturn[0] += no->coords[Graph::X] * (next->coords[Graph::Y] - prev->coords[Graph::Y]);
        no = next;
    } while (no != surface.nodes[0]);

    toReturn[0] /= 2;

    if (toReturn[0] < 0)
        toReturn[0] = 0;

    return toReturn;
}

Graph::ThickSurface2 Graph::generateCircularThicksurface(double centerX, double centerY, double outerRadius, double initialThickness, int pts){
    Graph::ThickSurface2 toReturn;
    toReturn.layers[Graph::OUTER] = Graph::generateCircularGraph(centerX, centerY, outerRadius, pts);
    toReturn.layers[Graph::INNER] = Graph::generateCircularGraph(centerX, centerY, outerRadius - initialThickness, pts);

    /* One of the only places we can depend on nodes' indices */
    for (int i = 0; i < toReturn.layers[Graph::OUTER].nodes.size(); i++){
        toReturn.layers[Graph::OUTER].nodes[i]->correspondents.push_back(
                toReturn.layers[Graph::INNER].nodes[i]
        );
        toReturn.layers[Graph::INNER].nodes[i]->correspondents.push_back(
                toReturn.layers[Graph::OUTER].nodes[i]
        );
    }
    return toReturn;
}