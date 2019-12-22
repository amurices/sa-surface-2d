//
// Created by Andre Muricy on 2019-12-22.
//

int main(){
    /* My sweet playground*/
    nanogui::init();

    Optimizer myOpt;

    std::unordered_map <std::string, std::string> inputMap;
    IO::sillyMapReader("../input.txt", inputMap);
    InitSaParams theseParams;
    IO::parseInputToParams(inputMap, &theseParams);
    std::cout << theseParams;
    myOpt.params = &theseParams;

    Graph::ThickSurface2 mySurface = Graph::generateCircularThicksurface(0.0, 0.0, 1.0, 0.2, 200);

    // Declarations and instantiations:
    // -----------------------------------------
    nanogui::ref<Renderer> myRenderer = new Renderer();
    // -----------------------------------------

    myRenderer->setVisible(true);
    myRenderer->thickSurface2 = &mySurface;
    myRenderer->uploadIndices2();
    // Awkward a0 generation
    theseParams.a0 = [mySurface]{
        auto out = Graph::surfaceAreaAndPerimeter(mySurface.layers[Graph::OUTER]);
        auto inn = Graph::surfaceAreaAndPerimeter(mySurface.layers[Graph::INNER]);
        return out[0] - inn[0];
    }();

    printf("a0: %.3f\n", theseParams.a0);

    while (!glfwWindowShouldClose(myRenderer->glfwWindow()))
    {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        myRenderer->drawContents2();
        myOpt.changed = true;

        myRenderer->uploadSurface2();
        glfwSwapBuffers(myRenderer->glfwWindow());
        glfwPollEvents();
    }
}