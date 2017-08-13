
//
//  main.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include <iostream>
#include <sys/time.h>
#include <cstdio>

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

// Interesting hyperparams to us:
// Number of points;
// Thickness;
// Smoothness ratio
// Perimeter
// -----
/// Tests we'll want to run (and plot (x, y)):
/// (free nrg, thickness)               - smoothness fixed on N values: N plots
/// (free nrg, smoothness)              - thickness fixed on N values: N plots
/// (free nrg, gray area / perimeter)   - smoothness fixed on N values: N x N plots

const int NUM_POINTS = 340;
const int NUM_GENS = 10000;
const int NUM_SMOOTH = 5;

using namespace lemon;
using namespace std;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    // Rendering loop variable declarations below:
    // -----------------------------------------
    ThickSurface_t myThickSurf; // the surface itself. ThickSurface_t is a wrapper around SurfaceData
    std::vector<point_t> is;    // container to hold intersections
    Interfacer::generate_circle(myThickSurf, 0.75, 0.025, NUM_POINTS, is);
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

    Optimizer opt(myThickSurf);
    opt.init_SA(0.1, NUM_SMOOTH, 1, 2); // Why does increasing Pow decrease pS?
    ThickSurface_t nghbr;
    
    opt.neighbor(myThickSurf, nghbr);
    
 //   opt.evolve_sa(NUM_GENS);
    
    // Get intersections of solution and put them in is
    opt.find_intersections(is);
    // Rendering loop below:
    // -----------------------------------------
    int count = 0;
    bool wasDown = false; bool pause = false;
    while (!glfwWindowShouldClose(myRenderer.window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
      //  glMatrixMode(GL_PROJECTION);
      //  glLoadIdentity();
      //  glOrtho(0.0, 0, 0, 0.0, -1.0, 1.0);
      //  glMatrixMode(GL_MODELVIEW);
      //  glLoadIdentity();

        if (!pause && count != NUM_GENS)
        {
            opt.step_sa();
            count++;
        }
        // Compile these later into a hyperparam struct. We want:
        char gen[16];   // Gen number
        char nrg[16];   // free energy (outer area + (diffMul * stretch ^ diffPow))
        char wht[16];   // white matter (inner area)
        char gry[16];   // gray matter (outer area - inner area)
        char dif[16];   // stretch factor (initial gray matter - current gray matter)
        char prm[16];   // perimeter (sum of all distances)
        std::sprintf(gen, "gen %d", count);
        std::sprintf(nrg, "nrg %.7f", opt.energy);
        std::sprintf(wht, "wht %.7f", opt.white);
        std::sprintf(gry, "gry %.7f", opt.gray);
        std::sprintf(dif, "dif %.7f", opt.stretch);
        std::sprintf(prm, "prm %.7f", opt.perimeter);

        myRenderer.render_text(fonti, gen, point_t(-1, 0.76), 34);
        myRenderer.render_text(fonti, nrg, point_t(-1, 0.71), 34);
        myRenderer.render_text(fonti, wht, point_t(-1, 0.66), 34);
        myRenderer.render_text(fonti, gry, point_t(-1, 0.61), 34);
        myRenderer.render_text(fonti, dif, point_t(-1, 0.56), 34);
        myRenderer.render_text(fonti, prm, point_t(-1, 0.51), 34);
        myRenderer.render_axes(fonti);
        myRenderer.render_surface(opt.org->outer, triple_t (1.0, 0.0, 1.0));
        myRenderer.render_surface(opt.org->inner, triple_t (0.7, 0.7, 0.3));
        myRenderer.render_surface(opt.org->bridges, triple_t (0.7, 0.3, 0.7));
        
///   myRenderer.render_surface(nghbr.outer, triple_t (0.5, 0.5, 1.0));
   ///  myRenderer.render_surface(nghbr.inner, triple_t (0.4, 0.4, 0.7));
     ///   myRenderer.render_surface(nghbr.bridges, triple_t (0.3, 0.7, 0.7));
        
        myRenderer.render_intersections(is);
        
        // ----------------------------------------------------
        // Input processing and Conditional rendering below:
        // ----------------------------------------------------
        glfwGetCursorPos(myRenderer.window, &mousePos.x, &mousePos.y);
        mousePos.x = mousePos.x * (2.0 / myRenderer.wWidth) - 1;
        mousePos.y = myRenderer.wHeight - mousePos.y;
        mousePos.y = mousePos.y * (2.0 / myRenderer.wHeight) - 1;
        
        int space_state = glfwGetKey(myRenderer.window, GLFW_KEY_SPACE);
    
        if (space_state == GLFW_PRESS)
        {
            if (count == NUM_GENS)
                count++;
            if (!wasDown && pause)
            {
                std::cout << "\n\n\n\n\n\nUNPAUSING\n\n\n\n\n\n";
                pause = false;
            }
            else if (!wasDown && !pause || count == 100)
            {
                std::cout << "\n\n\n\n\n\nPAUSING\n\n\n\n\n\n";
                pause = true;
            }
            if (count != 100)
                wasDown = true;
        }
        else
        {
            wasDown = false;
        }
        // ----------------------------------------------------
        // ----------------------------------------------------
        // ----------------------------------------------------

        
        for (ListDigraph::NodeIt no(opt.org->outer.graph); no != INVALID; ++no)
        {
            int rendering = 0;
            if (dist(mousePos, (*opt.org->outer.coords)[ no ]) < 0.05 && rendering < 1)
            {
                rendering++;
                char nodeinfo[32];
                sprintf(nodeinfo, "%d: %.3f,%.3f",
                        opt.org->outer.graph.id(no),
                        (*opt.org->outer.coords)[no].x,
                        (*opt.org->outer.coords)[no].y);
                myRenderer.render_node(no, opt.org->outer, triple_t (1.0, 0.0, 1.0));
                myRenderer.render_text(fonti, nodeinfo, (*opt.org->outer.coords)[no] + point_t(0.05, 0.05), 18);
                
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
