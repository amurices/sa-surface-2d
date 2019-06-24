// Surface2D 2.0.cpp : Defines the entry point for the console application.
//
#include <nanogui/nanogui.h>

#include <fstream>
#include <unordered_map>

#include "Renderer.hpp"
#include "Optimizer.hpp"
#include "ThickSurface.hpp"
#include "IO.hpp"

int main(int argc, char **argv)
{
    nanogui::init();
    // Declarations and instantiations:
    // -----------------------------------------
    nanogui::ref<Renderer> myRenderer = new Renderer();
    // -----------------------------------------

    Optimizer myOpt;

    std::unordered_map <std::string, std::string> inputMap;
    IO::sillyMapReader("input.txt", inputMap);
    InitSaParams theseParams;
    IO::parseInputToParams(inputMap, &theseParams);
    std::cout << theseParams;
    myOpt.params = &theseParams;

    ThickSurface mySurface;
    mySurface.generateCircularThickSurface(theseParams.radius, theseParams.points, true, theseParams.thickness, theseParams.thickness, point_t(0.0, 0.0));

    myRenderer->setVisible(true);
    myRenderer->thickSurface = &mySurface;
    myRenderer->optimizer = &myOpt;
    double temperature = 0;
    myRenderer->uploadIndices();
    
    // Awkward a0 generation
    double perim;
    theseParams.a0 = mySurface.outer->findSurfaceAreaAndPerimeter(perim) - mySurface.inner->findSurfaceAreaAndPerimeter(perim);;

    while (!glfwWindowShouldClose(myRenderer->glfwWindow()))
    {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        myRenderer->drawContents();
        myRenderer->drawWidgets();
        myOpt.changed = true;
        if (myRenderer->shouldStep){
            myOpt.step_saV2(mySurface, &temperature, theseParams.a0);
        }
        myRenderer->uploadSurface();
        glfwSwapBuffers(myRenderer->glfwWindow());
        glfwPollEvents();
    }
    return 0;
}
