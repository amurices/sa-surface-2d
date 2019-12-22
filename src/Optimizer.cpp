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
        SNode randomNodeOutgoing = thickSurface->outer->graph->target(ListDigraph::OutArcIt(*thickSurface->outer->graph, randomNode));
        SNode randomInnerNode = thickSurface->inner->graph->nodeFromId(thickSurface->outer->correspondence[randomIndex]);

        // Choose a random offset, bounded by optimizer params
        double offsetX, offsetY;
        offsetX = Util::getRandomRange(- this->params->forceOffsetRange, this->params->forceOffsetRange);
        offsetY = Util::getRandomRange(- this->params->forceOffsetRange, this->params->forceOffsetRange);
        
        point_t directionOfChange(offsetX, offsetY);

        point_t newPositionByAddingChange = (*thickSurface->outer->coords)[randomNode] + directionOfChange;
        point_t newPositionBySubtractingChange = (*thickSurface->outer->coords)[randomNode] - directionOfChange;

        // Collect change to outer node's position.
        neighborChanges->insert(NodeChange_t(randomNode, randomNodeOutgoing, directionOfChange, thickSurface->outer->graph));

        // We now have the offset of the neighbor in relation to the current state. If we want to know whether
        // this particular force has stretched or compressed the surface, which will determine whether we multiply
        // thickness by a value greater or less than than 1, we have to know if the offset point in the outer
        // surface is closer to or further away from the original's corresponding inner point. This would be easily
        // determined by looking at both polygons' areas, but since we're not in that realm yet (it belongs to the
        // probability function) we can just compare the distance of the offset point if the offset were in the opposite
        // direction.

        double distPdir = Util::pointsDistance(newPositionByAddingChange,
                                               (*thickSurface->inner->coords)[randomInnerNode]);
        double distMdir = Util::pointsDistance(newPositionBySubtractingChange,
                                               (*thickSurface->inner->coords)[randomInnerNode]);

        // If the distance to the inner node is now larger, then the surface will be stretched, otherwise itll be compressed
        double thicknessDiff;
        thicknessDiff = thickSurface->thicknesses[randomIndex] * (distPdir > distMdir ? this->params->compression : 1 / this->params->compression);
        thicknessDiff -= thickSurface->thicknesses[randomIndex];

        neighborThicknessChanges->insert(ThicknessChange_t(randomIndex, thicknessDiff));
        thickSurface->smoothAdjacentThicknesses(thicknessDiff, this->params->smooth, randomNode, neighborThicknessChanges, &MathGeometry::linearSmooth);
        thickSurface->outer->smoothAdjacentNodesV2(randomNode, directionOfChange, this->params->smooth, neighborChanges, &MathGeometry::linearSmooth);
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
    double potentialResult = exp((eS - eN) / t);
    std::cout << "t is " << t <<  "; potentialResult (should be undefined) is: " << potentialResult << std::endl;
    if (eN < eS)
        return 1;
    else
        return potentialResult;
}
/*
1. Perimetros, pontos e areas no output
2. Tratar os três casos de T:
  - Zero: continua retornando sempre o estado de menor energia
  - Finita: Continua retornando o estado da fórmula
  - Infinita: retorna qualquer coisa
3. Temperatura constante
 t = t0 . exp (-t /tal) onde tal = escala de tempo
4. densidade de pontos nao pode ser mto grande
5. fazer lista de correspondência entre pontos
6. STL?
7. Arquivo separado com energia e um só com pontos <- isso faz sentido
*/
void Optimizer::stepSimulatedAnnealing(ThickSurface *thickSurface, double *temperature, double a0)
{
    this->neighborChanges.clear();
    this->thicknessChanges.clear();
    findNeighbor(thickSurface, &this->neighborChanges, &this->thicknessChanges);
    double eS = findEnergy(thickSurface, a0);
    applyChanges(thickSurface, this->neighborChanges, this->thicknessChanges);
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
        revertChanges(thickSurface, this->neighborChanges, this->thicknessChanges);
    }
    if (singleStep)
    {
        singleStep = false;
        shouldStep = false;
    }

    if (this->recording){
        commitToOutputFile(eS, eN, *temperature, 0, 0, changed);
    }
    // TODO: Pedreirar menos aqui
    //*temperature -= 0.01;
}

void Optimizer::commitToOutputFile(double eS, double eN, double t, double whiteS, double whiteN, bool changed)
{
    this->outputFile << "eS " << eS << " eN " << eN << " T " << t << " wS " << whiteS << " wN " << whiteN << " moved? " << changed << std::endl;
}

void Optimizer::reset(){
    this->shouldStep = false;
    this->singleStep = false;
    this->changed = false;
}