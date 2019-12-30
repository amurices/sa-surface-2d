// Surface2D 2.0.cpp : Defines the entry point for the console application.
//
#include <nanogui/nanogui.h>

#include <fstream>
#include <unordered_map>
#include <GlobalState.hpp>
#include <MathGeometry.hpp>

#include "Renderer.hpp"
#include "Optimizer.hpp"
#include "Optimizer2.hpp"
#include "ThickSurface.hpp"
#include "IO.hpp"


int main(int argc, char **argv)
{
    /* My sweet playground*/
    nanogui::init();

    std::unordered_map <std::string, std::string> inputMap;
    IO::sillyMapReader("../input.txt", inputMap);
    InitSaParams theseParams;
    IO::parseInputToParams(inputMap, &theseParams);
    std::cout << theseParams;
    GlobalState::setSurfaceParameters(theseParams.radius, theseParams.thickness, 0.0, 0.0, theseParams.points);
    GlobalState::initThickSurface();
    double initialGrayMatter =
            Graph::surfaceArea(GlobalState::thickSurface.layers[Graph::OUTER]) -
            Graph::surfaceArea(GlobalState::thickSurface.layers[Graph::INNER]);
    GlobalState::setOptimizerParameters(initialGrayMatter, theseParams.smooth, theseParams.diffMul, theseParams.diffPow,
                                        theseParams.areaMul, theseParams.areaPow, theseParams.multiProb,
                                        theseParams.tempProb, theseParams.forceOffsetRange, theseParams.compression,
                                        MathGeometry::linearSmooth, 0);

 //   auto myChangeset = Graph::smoothAdjacentNodes(GlobalState::thickSurface.layers[Graph::OUTER], Graph::NodeChange(GlobalState::thickSurface.layers[Graph::OUTER].nodes[100], 0, -0.35), 10, MathGeometry::linearSmooth);
 //   auto myInnerChangeset = Graph::innerChangesetFromOuterChangeset(GlobalState::thickSurface, myChangeset, 1.0);
 //   Graph::applyNodeChanges(myChangeset);
 //   Graph::applyNodeChanges(myInnerChangeset);
    // Nanogui renderer setup:
    nanogui::ref <Renderer> myRenderer = new Renderer();
    myRenderer->setVisible(true);
    myRenderer->thickSurface2 = &GlobalState::thickSurface;
    myRenderer->uploadIndices2();
    // -----------------------------------------

    int count = 0;
    while (!glfwWindowShouldClose(myRenderer->glfwWindow())) {
        count++;
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        Optimizer2::stepSimulatedAnnealing();

        myRenderer->drawContents2();

        myRenderer->uploadSurface2();
        glfwSwapBuffers(myRenderer->glfwWindow());
        glfwPollEvents();
    }
}
