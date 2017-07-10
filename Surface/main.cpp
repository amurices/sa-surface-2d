
//
//  main.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include <iostream>
#include <sys/time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <lemon/list_graph.h>
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Sweep_line_2_algorithms.h>

#include "Interfacer.hpp"
#include "Auxiliares.hpp"
#include "Renderer.hpp"
#include "Optimizer.hpp"


const int NUM_POINTS = 60;
const int NUM_GENS = 10000;

using namespace lemon;
using namespace std;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    // Rendering loop variable declarations below:
    // -----------------------------------------
    ThickSurface_t myThickSurf;         // the surface itself. ThickSurface_t is a wrapper around SurfaceData_t
    std::vector<point_t> is;
    Interfacer::generate_circle(myThickSurf, 0.75, NUM_POINTS, is);
    FTGLPixmapFont fonti("/Library/Fonts/Arial.ttf");      // Load .ttf file to memory
    
    point_t mousePos;                                      // Mouse position for (minimal) input handling
    
    Renderer        myRenderer; // Renderer instance (so it can store some loop-related stuff transparently)
    int init =      myRenderer.initWindow();    // glfw window wrapper
    // -----------------------------------------
    
    
    // Error-handling calls:
    // -----------------------------------------
    myRenderer.handle(init);                // Window initialization handling
    if(fonti.Error()) exit(EXIT_FAILURE);   // Font loading handling
    // -----------------------------------------

    Optimizer opt(myThickSurf, 0.3);
    
    opt.evolve_sa(NUM_GENS);
    
    // Get intersections of solution and put them in is
    opt.find_intersections(is);
    // Rendering loop below:
    // -----------------------------------------
    while (!glfwWindowShouldClose(myRenderer.window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
      //  glMatrixMode(GL_PROJECTION);
      //  glLoadIdentity();
      //  glOrtho(0.0, 0, 0, 0.0, -1.0, 1.0);
      //  glMatrixMode(GL_MODELVIEW);
      //  glLoadIdentity();

        myRenderer.render_text(fonti, "yo", point_t(-1, 0.76), 72);
        myRenderer.render_axes(fonti);
        myRenderer.render_surface(opt.org->outer, triple_t (1.0, 0.0, 1.0));
        myRenderer.render_surface(opt.org->inner, triple_t (0.7, 0.7, 0.3));
        myRenderer.render_surface(opt.org->bridges, triple_t (0.7, 0.3, 0.7));
        
   //     myRenderer.render_surface(nghbr.outer, triple_t (0.5, 0.5, 1.0));
   //     myRenderer.render_surface(nghbr.inner, triple_t (0.4, 0.4, 0.7));
   //     myRenderer.render_surface(nghbr.bridges, triple_t (0.3, 0.7, 0.7));
        
        myRenderer.render_intersections(is);
        
        // ----
        // Conditional rendering below:
        // ----
        glfwGetCursorPos(myRenderer.window, &mousePos.x, &mousePos.y);
        mousePos.x = mousePos.x * (2.0 / myRenderer.wWidth) - 1;
        mousePos.y = myRenderer.wHeight - mousePos.y;
        mousePos.y = mousePos.y * (2.0 / myRenderer.wHeight) - 1;
        
        for (ListDigraph::NodeIt no(opt.org->outer.graph); no != INVALID; ++no)
        {
            if (dist(mousePos, (*opt.org->outer.coords)[ no ]) < 0.05)
            {
                myRenderer.render_node(no, opt.org->outer, triple_t (1.0, 0.0, 1.0));
                myRenderer.render_node(opt.org->inner.graph.nodeFromId(opt.org->outer.correspondence[opt.org->outer.graph.id(no)]), opt.org->inner, triple_t (0.7, 0.7, 0.3));
            }
        }
        glfwSwapBuffers(myRenderer.window);
        glfwPollEvents();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
   
    return 0;
}
