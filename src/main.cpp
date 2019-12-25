// Surface2D 2.0.cpp : Defines the entry point for the console application.
//
#include <nanogui/nanogui.h>

#include <fstream>
#include <unordered_map>
#include <GlobalState.hpp>
#include <MathGeometry.hpp>

#include "Renderer.hpp"
#include "Optimizer.hpp"
#include "ThickSurface.hpp"
#include "IO.hpp"


int main(int argc, char **argv)
{
    /* My sweet playground*/
    nanogui::init();

    Optimizer myOpt;

    std::unordered_map <std::string, std::string> inputMap;
    IO::sillyMapReader("../input.txt", inputMap);
    InitSaParams theseParams;
    IO::parseInputToParams(inputMap, &theseParams);
    std::cout << theseParams;
    GlobalState::initParameterObjects();
    GlobalState::setSurfaceParameters(theseParams.radius, theseParams.thickness, 0.0, 0.0, theseParams.points);
    GlobalState::initThickSurface();
    double initialGrayMatter = []{
        auto outer = Graph::surfaceAreaAndPerimeter(GlobalState::thickSurface.layers[Graph::OUTER]);
        auto inner = Graph::surfaceAreaAndPerimeter(GlobalState::thickSurface.layers[Graph::INNER]);
        return outer[0] - inner[0];
    }();
    GlobalState::setOptimizerParameters(initialGrayMatter, theseParams.smooth, theseParams.diffMul, theseParams.diffPow,
                                        theseParams.areaMul, theseParams.areaPow, theseParams.multiProb,
                                        theseParams.tempProb, theseParams.forceOffsetRange, theseParams.compression,
                                        MathGeometry::linearSmooth, 0);

    // Declarations and instantiations:
    // -----------------------------------------
    nanogui::ref <Renderer> myRenderer = new Renderer();
    // -----------------------------------------

    myRenderer->setVisible(true);
    myRenderer->thickSurface2 = &GlobalState::thickSurface;
    myRenderer->uploadIndices2();


    while (!glfwWindowShouldClose(myRenderer->glfwWindow())) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        myRenderer->drawContents2();

        myRenderer->uploadSurface2();
        glfwSwapBuffers(myRenderer->glfwWindow());
        glfwPollEvents();
    }
//    nanogui::init();
//
//    Optimizer myOpt;
//
//    std::unordered_map <std::string, std::string> inputMap;
//    IO::sillyMapReader("../input.txt", inputMap);
//    InitSaParams theseParams;
//    IO::parseInputToParams(inputMap, &theseParams);
//    std::cout << theseParams;
//    Graph::ThickSurface2 mySurface = Graph::generateCircularThicksurface(0, 0, 1, 0.1, 200);
//    auto changeSet = Graph::neighborOuterChangeset(mySurface.layers[Graph::OUTER], 0.0, 0.2, 8,
//                                                   MathGeometry::sineSmooth);
//    auto innerChangeset = Graph::innerChangesetFromOuterChangeset(mySurface, changeSet, 1.0);
//    for (auto it =  changeSet.begin(); it != changeSet.end(); it++){
//        it->node->coords[Graph::X] += it->changeX;
//        it->node->coords[Graph::Y] += it->changeY;
//    }
//    for (auto it =  innerChangeset.begin(); it != innerChangeset.end(); it++){
//        it->node->coords[Graph::X] += it->changeX;
//        it->node->coords[Graph::Y] += it->changeY;
//    }
//
//    // Declarations and instantiations:
//    // -----------------------------------------
//    nanogui::ref<Renderer> myRenderer = new Renderer();
//    // -----------------------------------------
//
//    myRenderer->setVisible(true);
//    myRenderer->thickSurface2 = &mySurface;
//    myRenderer->uploadIndices2();
//    // Awkward a0 generation
//    theseParams.a0 = [mySurface]{
//        auto out = Graph::surfaceAreaAndPerimeter(mySurface.layers[Graph::OUTER]);
//        auto inn = Graph::surfaceAreaAndPerimeter(mySurface.layers[Graph::INNER]);
//        return out[0] - inn[0];
//    }();
//
//    printf("a0: %.3f\n", theseParams.a0);
//
//    while (!glfwWindowShouldClose(myRenderer->glfwWindow()))
//    {
//        glClearColor(0,0,0,1);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//        myRenderer->drawContents2();
//
//        myRenderer->uploadSurface2();
//        glfwSwapBuffers(myRenderer->glfwWindow());
//        glfwPollEvents();
//    }
}
