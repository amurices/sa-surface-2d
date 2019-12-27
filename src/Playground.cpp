//
// Created by Andre Muricy on 2019-12-22.
//

int main(int argc, char **argv) {
    auto myChangeset = Graph::smoothAdjacentNodes(GlobalState::thickSurface.layers[Graph::OUTER], Graph::NodeChange(GlobalState::thickSurface.layers[Graph::OUTER].nodes[0], -0.05, 0.0), 10, MathGeometry::linearSmooth);
    auto myInnerChangeset = Graph::innerChangesetFromOuterChangeset(GlobalState::thickSurface, myChangeset, 1.0);
    for (auto it = myChangeset.begin(); it != myChangeset.end(); it++){
        it->node->coords[Graph::X] += it->changeX;
        it->node->coords[Graph::Y] += it->changeY;
    }
    for (auto it = myInnerChangeset.begin(); it != myInnerChangeset.end(); it++){
        it->node->coords[Graph::X] += it->changeX;
        it->node->coords[Graph::Y] += it->changeY;
    }
}