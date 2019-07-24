#include "Optimizer.hpp"
#include "MathGeometry.hpp"

Optimizer::Optimizer()
{
}

Optimizer::~Optimizer()
{
}

void Optimizer::applyChanges(ThickSurface *thickSurface, std::set<NodeChange_t> &changes, std::set<ThicknessChange_t> &thicknessChanges)
{
    // TODO: Apply changes has to see what updateInnerSurfaceV2 has done. So the call to updateInnerSurfaceV2 should be done inside here,
    // with the changes being simply added to changes and thicknessChanges. Then after a call to applyChanges has been made, all the changes
    // are done AND will be visible within the sets, which means they can be reversed.
    for (auto it = changes.begin(); it != changes.end(); it++)
    {
        if (it->graph == thickSurface->outer->graph)
        {
            (*thickSurface->outer->coords)[it->node] += it->change;
        }
    }

    for (auto it = thicknessChanges.begin(); it != thicknessChanges.end(); it++)
    {
        thickSurface->thicknesses[it->nodeIndex] += it->change;
    }
    std::set<SNode> changedIDK;
    thickSurface->updateInnerSurfaceV2(&changes);
}

void Optimizer::revertChanges(ThickSurface *thickSurface, std::set<NodeChange_t> &changes, std::set<ThicknessChange_t> &thicknessChanges)
{
    // TODO: Apply changes has to see what updateInnerSurfaceV2 has done. So the call to updateInnerSurfaceV2 should be done inside here,
    // with the changes being simply added to changes and thicknessChanges. Then after a call to applyChanges has been made, all the changes
    // are done AND will be visible within the sets, which means they can be reversed.
    for (auto it = changes.begin(); it != changes.end(); it++)
    {
        if (it->graph == thickSurface->outer->graph)
        {
            (*thickSurface->outer->coords)[it->node] -= it->change;
        }
        else if (it->graph == thickSurface->inner->graph)
        {
            (*thickSurface->inner->coords)[it->node] -= it->change;
        }
    }

    for (auto it = thicknessChanges.begin(); it != thicknessChanges.end(); it++)
    {
        thickSurface->thicknesses[it->nodeIndex] -= it->change;
    }
}

void Optimizer::findNeighbor(ThickSurface *thickSurface, std::set<NodeChange_t> *neighborChanges,
                             std::set<ThicknessChange_t> *neighborThicknessChanges)
{
    std::vector<int> randomIndexes;
    // Use a set of changes for node and thickness changes

    // First, choose indices that will be modified by random neighbor search
    double coinFlip = 0.0;
    do
    { // do {} while guarantees that at least one node is modified
        int randomIndex = rand() % thickSurface->outer->nNodes;
        randomIndexes.push_back(randomIndex);
        coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    } while (coinFlip < this->params->multiProb);

    for (size_t i = 0; i < randomIndexes.size(); i++)
    {
        int randomIndex = randomIndexes[i];

        SNode randomNode = thickSurface->outer->graph->nodeFromId(randomIndex);
        SNode randomInnerNode = thickSurface->inner->graph->nodeFromId(thickSurface->outer->correspondence[randomIndex]);

        // Choose a random offset, bounded by optimizer params
        double offsetX, offsetY;
        offsetX = Util::getRandomRange(- this->params->forceOffsetRange, this->params->forceOffsetRange);
        offsetY = Util::getRandomRange(- this->params->forceOffsetRange, this->params->forceOffsetRange);
        
        point_t dir(offsetX, offsetY);

        point_t pdir = (*thickSurface->outer->coords)[randomNode] + dir;
        point_t mdir = (*thickSurface->outer->coords)[randomNode] - dir;

        // Collect change to outer node's position.
        neighborChanges->insert(NodeChange_t(randomNode, dir, thickSurface->outer->graph));

        // We now have the offset of the neighbor in relation to the current state. If we want to know whether
        // this particular force has stretched or compressed the surface, which will determine whether we multiply
        // thickness by a value greater or less than than 1, we have to know if the offset point in the outer
        // surface is closer to or further away from the original's corresponding inner point. This would be easily
        // determined by looking at both polygons' areas, but since we're not in that realm yet (it belongs to the
        // probability function) we can just compare the distance of the offset point if the offset were in the opposite
        // direction.

        double distPdir = Util::dist(pdir, (*thickSurface->inner->coords)[randomInnerNode]);
        double distMdir = Util::dist(mdir, (*thickSurface->inner->coords)[randomInnerNode]);

        // If the distance to the inner node is now larger, then the surface will be stretched, otherwise itll be compressed
        double thicknessDiff;
        thicknessDiff = thickSurface->thicknesses[randomIndex] * (distPdir > distMdir ? this->params->compression : 1 / this->params->compression);
        thicknessDiff -= thickSurface->thicknesses[randomIndex];

        neighborThicknessChanges->insert(ThicknessChange_t(randomIndex, thicknessDiff));
        thickSurface->smoothAdjacentThicknesses(thicknessDiff, this->params->smooth, randomNode, neighborThicknessChanges, &MathGeometry::linearSmooth);
        // TODO: Pass changes to smoothAdjacentNodesV2 so that based on it, it does its thing and adds
        // the appropriate shit to changes. Add smoothAdjacentNodesV2 prototype so it compiles.
        thickSurface->outer->smoothAdjacentNodesV2(randomNode, dir, this->params->smooth, neighborChanges, &MathGeometry::linearSmooth);
    }
}

double Optimizer::findEnergy(const ThickSurface *thickSurface, double a0)
{
    double res;

    double whiteMatterArea, whiteMatterPerimeter;
    double grayMatterArea, grayMatterPerimeter;

    // White matter area is just the inner surface's area
    whiteMatterArea = thickSurface->inner->findSurfaceAreaAndPerimeter(whiteMatterPerimeter);
    // Gray matter area is the difference between the outer surface and the inner surface
    grayMatterArea = thickSurface->outer->findSurfaceAreaAndPerimeter(grayMatterPerimeter) - whiteMatterArea;
    // Area must be non-negative
    grayMatterArea = MathGeometry::absol(grayMatterArea);

    // Difference between initial and current gray matter area.
    double grayMatterStretch = MathGeometry::absol(a0 - grayMatterArea);

    double p1, p2;
    p1 = pow(whiteMatterArea, this->params->areaPow);
    p2 = pow(grayMatterStretch + 1, this->params->diffPow); // Stretch is the difference; therefore p2 is raised to the right power here
                                                    // We add 1 before raising to ensure the growth is not sublinear from the get-go

    // Simplified energy equation of 2D closed surface
    res = params->areaMul * p1 + params->diffMul * p2;
    return res;
}

double Optimizer::findProbability(double eS, double eN, double t)
{
    if (eN < eS)
        return 1;

    else
        return exp((eS - eN) / t);
}

void Optimizer::stepSimulatedAnnealing(ThickSurface *thickSurface, double *temperature, double a0)
{
    neighborChanges.clear();
    thicknessChanges.clear();
    findNeighbor(thickSurface, &neighborChanges, &thicknessChanges);
    double eS = findEnergy(thickSurface, a0);
    applyChanges(thickSurface, neighborChanges, thicknessChanges);
    double eN = findEnergy(thickSurface, a0);

    double prob = findProbability(eS, eN, *temperature);
    double coinFlip = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);

    std::vector<_2DSurface*> surfaces;
    std::vector<point_t> potentialIntersections;

    surfaces.push_back(thickSurface->outer);
    surfaces.push_back(thickSurface->inner);
    int numInts = MathGeometry::findSurfaceIntersections(surfaces, potentialIntersections);
    if (numInts > 0) {
        prob = 0; // We'll always want to revert changes
    }
    changed = true;
    if (coinFlip >= prob)
    {
        changed = false;
        revertChanges(thickSurface, neighborChanges, thicknessChanges);
    }
    if (singleStep)
    {
        singleStep = false;
        shouldStep = false;
    }
    // TODO: Pedreirar menos aqui
    //*temperature -= 0.01;
}