// Surface2D 2.0.cpp : Defines the entry point for the console application.
//
#include <nanogui/nanogui.h>

#include <fstream>
#include <unordered_map>
#include <GlobalState.hpp>
#include <MathGeometry.hpp>
#include <GraphEffects.hpp>

#include "Renderer.hpp"
#include "Optimizer.hpp"
#include "IO.hpp"


int main(int argc, char **argv) {
    /* My sweet playground */
    nanogui::init();

    std::unordered_map<std::string, std::string> inputMap;
    IO::sillyMapReader("../input.txt", inputMap);
    IO::InitSaParams theseParams;
    IO::parseInputToParams(inputMap, &theseParams);
    std::cout << theseParams;
    GlobalState::setSurfaceParameters(theseParams.radius, theseParams.thickness, 0.0, 0.0, theseParams.points,
                                      theseParams.bothCorrsDist, theseParams.splitThreshold);
    GlobalState::initThickSurface();
    double initialGrayMatter =
            Graph::surfaceArea(GlobalState::thickSurface.layers[Graph::OUTER]) -
            Graph::surfaceArea(GlobalState::thickSurface.layers[Graph::INNER]);
    GlobalState::setOptimizerParameters(initialGrayMatter, theseParams.smooth, theseParams.diffMul, theseParams.diffPow,
                                        theseParams.areaMul, theseParams.areaPow, theseParams.multiProb,
                                        theseParams.tempProb, theseParams.forceOffsetRange, theseParams.compression,
                                        MathGeometry::linearSmooth, 0);

    Effects::addNode2(&GlobalState::thickSurface.layers[Graph::OUTER],
                      GlobalState::thickSurface.layers[Graph::OUTER].nodes[0],
                      GlobalState::thickSurface.layers[Graph::OUTER].nodes[1],
                      GlobalState::surfaceParameters.bothCorrsDist);

    // Nanogui renderer setup:
    nanogui::ref<Renderer> myRenderer = new Renderer();
    myRenderer->setVisible(true);
    myRenderer->makeInputForms(myRenderer->windows[0]);
    // -----------------------------------------

    /* Proximos passos;
     *
     * Triviais:
     * perimetro
     * temperatura nos parametros
     * Escala no zoom
     *
     * Semi-trivial:
     * Cranio (p inicial externo) (cranio devia ter numero primo de nós)
     *
     * Nada trivial:
     * Inserir e eliminar pontos
     * */

    while (!glfwWindowShouldClose(myRenderer->glfwWindow())) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (GlobalState::shouldStep) {
            Optimizer::stepSimulatedAnnealing();
            if (GlobalState::recording)
                IO::commitToOutputFile(GlobalState::thickSurface, GlobalState::recordedAttributes);
        }

        if (GlobalState::singleStep) {
            GlobalState::singleStep = false;
            GlobalState::shouldStep = false;
            Optimizer::stepSimulatedAnnealing();
            if (GlobalState::recording)
                IO::commitToOutputFile(GlobalState::thickSurface, GlobalState::recordedAttributes);
        }
        myRenderer->uploadIndices();
        myRenderer->uploadSurface();

        myRenderer->drawContents();
        myRenderer->drawWidgets();

        glfwSwapBuffers(myRenderer->glfwWindow());
        glfwPollEvents();
    }
}
