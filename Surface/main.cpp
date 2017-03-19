//
//  main.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include <iostream>


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <lemon/list_graph.h>

#include "Interfacer.hpp"
#include "Renderer.hpp"


const int NUM_POINTS = 40;


using namespace lemon;
using namespace std;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    ThickSurface_t myThickSurf;
    Interfacer::generate_random(myThickSurf.outer, 2.0, NUM_POINTS);
    Interfacer::generate_inner(myThickSurf.inner, myThickSurf.outer);

  //  Interfacer::get_from_matlab(mySurf, "matlab.mat");
    

    Renderer myRenderer;
    int init = myRenderer.initWindow();
    myRenderer.handle(init);
    
    while (!glfwWindowShouldClose(myRenderer.window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBegin(GL_LINES);

        // Calls to objects to be drawn go here
        Renderer::render_axes();
        Renderer::render_surface(myThickSurf.outer);
        Renderer::render_surface(myThickSurf.inner);
        // ---- 
        glEnd();
        glfwSwapBuffers(myRenderer.window);
        glfwPollEvents();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
    
    return 0;
}
