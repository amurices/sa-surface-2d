//
// Created by Andre Muricy on 2019-12-22.
//

int main(int argc, char **argv) {
    /* My sweet playground*/
    nanogui::init();

    Optimizer myOpt;

    std::unordered_map <std::string, std::string> inputMap;
    IO::sillyMapReader("../input.txt", inputMap);
    InitSaParams theseParams;
    IO::parseInputToParams(inputMap, &theseParams);
    std::cout << theseParams;
    GlobalState::setSurfaceParameters(theseParams.radius, theseParams.radius, 0.0, 0.0, theseParams.points);
    GlobalState::initThickSurface();
    double initialGrayMatter = Graph::surfaceAreaAndPerimeter(GlobalState::thickSurface.layers[Graph::OUTER])[0] -
                               Graph::surfaceAreaAndPerimeter(GlobalState::thickSurface.layers[Graph::INNER])[0];
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

    //printf("a0: %.3f\n", initialGrayMatter);

    while (!glfwWindowShouldClose(myRenderer->glfwWindow())) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        myRenderer->drawContents2();

        myRenderer->uploadSurface2();
        glfwSwapBuffers(myRenderer->glfwWindow());
        glfwPollEvents();
    }
}