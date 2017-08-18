
//
//  main.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include <iostream>
#include <fstream>
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

const int NUM_POINTS = 350;
const int NUM_GENS = 6500;
const int NUM_SMOOTH = 3;

const int numTests = 30;
const int numFixed = 3;
const int numParams = 2;

const int smoothVar = 5;
const double thicknessVar = 0.012;

using namespace lemon;
using namespace std;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    // Rendering loop variable declarations below:
    // -----------------------------------------
    // ThickSurface_t myThickSurf; // the surface itself. ThickSurface_t is a wrapper around SurfaceData
    
    ofstream output;
    ofstream output_t;
    
    std::vector<point_t> is;    // container to hold intersections
    // Interfacer::generate_circle(myThickSurf, 0.75, 0.025, NUM_POINTS, is);

    vector<ThickSurface_t> myThickSurfaces(numTests * numFixed * numParams);
    vector<Optimizer*> optimizers(numTests * numFixed * numParams);
    
    // Testing variable thickness, fixed smoothness
    for (int i = 0; i < numTests; i++)
    {
        Interfacer::generate_circle(myThickSurfaces[i], 1, thicknessVar * (i + 1), NUM_POINTS, is);
        optimizers[i] = new Optimizer(myThickSurfaces[i]);
        optimizers[i]->init_SA(0.1, NUM_SMOOTH, 1, 2); // Why does increasing Pow decrease pS?

        Interfacer::generate_circle(myThickSurfaces[i+numTests], 1, thicknessVar * (i + 1), NUM_POINTS, is);
        optimizers[i+numTests] = new Optimizer(myThickSurfaces[i+numTests]);
        optimizers[i+numTests]->init_SA(0.1, NUM_SMOOTH + smoothVar, 1, 2); // Why does increasing Pow decrease pS?

        Interfacer::generate_circle(myThickSurfaces[i+2*numTests], 1, thicknessVar * (i + 1), NUM_POINTS, is);
        optimizers[i+2*numTests] = new Optimizer(myThickSurfaces[i+2*numTests]);
        optimizers[i+2*numTests]->init_SA(0.1, NUM_SMOOTH + 2*smoothVar, 1, 2); // Why does increasing Pow decrease pS?
    }
    
    // Testing variable smoothness, fixed thickness
    for (int i = numTests * numFixed; i < numTests * (numFixed + 1); i++)
    {
        Interfacer::generate_circle(myThickSurfaces[i], 1, 0.05, NUM_POINTS, is);
        optimizers[i] = new Optimizer(myThickSurfaces[i]);
        optimizers[i]->init_SA(0.1, NUM_SMOOTH - 2 + i - (numTests * numFixed), 1, 2); // Why does increasing Pow decrease pS?

        Interfacer::generate_circle(myThickSurfaces[i+numTests], 1, 0.15, NUM_POINTS, is);
        optimizers[i+numTests] = new Optimizer(myThickSurfaces[i+numTests]);
        optimizers[i+numTests]->init_SA(0.1, NUM_SMOOTH - 2 + i - (numTests * numFixed), 1, 2); // Why does increasing Pow decrease pS?

        Interfacer::generate_circle(myThickSurfaces[i+2*numTests], 1, 0.25, NUM_POINTS, is);
        optimizers[i+2*numTests] = new Optimizer(myThickSurfaces[i+2*numTests]);
        optimizers[i+2*numTests]->init_SA(0.1, NUM_SMOOTH - 2 + i - (numTests * numFixed), 1, 2); // Why does increasing Pow decrease pS?
        cout << i << endl;


    }
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
    
    // Rendering loop below:
    // -----------------------------------------
    int count = 0; int currentThick = 0;
    bool wasDown = false; bool pause = false;
    output.open ("output.txt");
    output_t.open("output_t.txt");
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
            optimizers[currentThick]->step_sa();
            count++;
        }
        
        
        
        // Compile these later into a hyperparam struct. We want:
        char gen[16];   // Gen number
        char nrg[16];   // free energy (outer area + (diffMul * stretch ^ diffPow))
        char nrgt[10];  // use it for nrg/time plot
        char wht[16];   // white matter (inner area)
        char gry[16];   // gray matter (outer area - inner area)
        char dif[16];   // stretch factor (initial gray matter - current gray matter)
        char prm[16];   // perimeter (sum of all distances)
        char thk[16];   // thickness of this solution
        char smt[16];   // thickness of this solution
        std::sprintf(gen, "gen %d", count);
        std::sprintf(nrg, "nrg %.7f", optimizers[currentThick]->energy);
        std::sprintf(wht, "wht %.7f", optimizers[currentThick]->white);
        std::sprintf(gry, "gry %.7f", optimizers[currentThick]->gray);
        std::sprintf(dif, "dif %.7f", optimizers[currentThick]->stretch);
        std::sprintf(prm, "prm %.7f", optimizers[currentThick]->perimeter);
        std::sprintf(thk, "thk %.7f", optimizers[currentThick]->org->thickness);
        std::sprintf(smt, "smt %d", optimizers[currentThick]->smooth);
        
        output_t << gen << endl;
        output_t << nrg << endl;
        output_t << wht << endl;
        output_t << gry << endl;
        output_t << dif << endl;
        output_t << prm << endl;
        output_t << thk << endl;
        output_t << smt << endl;

        if (count == NUM_GENS)
        {
            count = 0;
            output << nrg << endl;
            output << wht << endl;
            output << gry << endl;
            output << dif << endl;
            output << prm << endl;
            output << thk << endl;
            output << smt << endl << endl;
            output_t << endl;
            currentThick++;
            if (currentThick >= optimizers.size())
                break;
        }
        
        
        myRenderer.render_text(fonti, gen, point_t(-1, 0.76), 34);
        myRenderer.render_text(fonti, nrg, point_t(-1, 0.71), 34);
        myRenderer.render_text(fonti, wht, point_t(-1, 0.66), 34);
        myRenderer.render_text(fonti, gry, point_t(-1, 0.61), 34);
        myRenderer.render_text(fonti, dif, point_t(-1, 0.56), 34);
        myRenderer.render_text(fonti, prm, point_t(-1, 0.51), 34);
        myRenderer.render_text(fonti, thk, point_t(-1, 0.46), 34);
        myRenderer.render_text(fonti, smt, point_t(-1, 0.41), 34);
        myRenderer.render_axes(fonti);
        myRenderer.render_surface(optimizers[currentThick]->org->outer, triple_t (1.0, 0.0, 1.0));
        myRenderer.render_surface(optimizers[currentThick]->org->inner, triple_t (0.7, 0.7, 0.3));
        myRenderer.render_surface(optimizers[currentThick]->org->bridges, triple_t (0.7, 0.3, 0.7));
        

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

        /*
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
        */
        glfwSwapBuffers(myRenderer.window);
        glfwPollEvents();
    }
    output.close();
    output_t.close();
    glfwTerminate();
    exit(EXIT_SUCCESS);
   
    return 0;
}
