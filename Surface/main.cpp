
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
//#include "SurfaceDecoder.hpp"
//#include "BRKGA.h"

const int NUM_POINTS = 2000;


using namespace lemon;
using namespace std;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    // Rendering loop variable declarations below:
    // -----------------------------------------
    ThickSurface_t myThickSurf;         // the surface itself. ThickSurface_t is a wrapper around SurfaceData_t
    std::vector<point_t> is;
    Interfacer::generate_circle(myThickSurf, 0.5, NUM_POINTS, is);
    cout << "MyThick points: " << myThickSurf.outer.nNodes << endl;
    //Interfacer::get_from_matlab(mySurf, "matlab.mat");
    FTGLPixmapFont fonti("/Library/Fonts/Arial.ttf");      // Load .ttf file to memory
    
    point_t mousePos;                                      // Mouse position for (minimal) input handling
    
    Renderer        myRenderer; // Renderer instance (so it can store some loop-related stuff transparently)
    int init =      myRenderer.initWindow();    // glfw window wrapper
    // -----------------------------------------
    
    
    // Error-handling calls:
    // -----------------------------------------
    myRenderer.handle(init);                    // Window initialization handling
    if(fonti.Error()) exit(EXIT_FAILURE);       // Font loading handling
    // -----------------------------------------

 /*   // Optimizer initialization:
    // -----------------------------------------
    const unsigned n = myThickSurf.outer.nNodes * 2;	// size of chromosomes
    const unsigned p = 1000;                            // size of population
    const double pe = 0.20;		// fraction of population to be the elite-set
    const double pm = 0.10;		// fraction of population to be replaced by mutants
    const double rhoe = 0.50;	// probability that offspring inherit an allele from elite parent
    const unsigned K = 3;		// number of independent populations
    const unsigned MAXT = 2;	// number of threads for parallel decoding
    const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
    const unsigned X_NUMBER = 2;	// exchange top 2 best
    const unsigned MAX_GENS = 10;	// run for 100 gens

    SurfaceDecoder decoder;			// initialize the decoder
    decoder.org = &myThickSurf;

    const long unsigned rngSeed = 0;	// seed to the random number generator
    MTRand rng(rngSeed);				// initialize the random number generator
    
    // initialize the BRKGA-based heuristic
    struct timeval tvalBefore;  // removed comma
    time_b(tvalBefore);
    BRKGA< SurfaceDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
    printf("Time: %f seconds\n\n", time_a(tvalBefore));

    unsigned generation = 0;		// current generation
    do {
        time_b(tvalBefore);
        
        algorithm.evolve();	// evolve the population for one generation
        
        printf("gen %d:\n Time: %f seconds\n",generation, time_a(tvalBefore));
        
        if((++generation) % X_INTVL == 0) {
            algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
        }
    } while (generation < MAX_GENS);
  
    std::vector<double> lel = algorithm.getBestChromosome();
    
    int count = 0;
    point_t acc(0,0);
    for (ListDigraph::NodeIt no(myThickSurf.outer.graph); no != INVALID; ++no)
    {
        (*myThickSurf.outer.coords)[no].x += lel[count];
        (*myThickSurf.outer.coords)[no].y += lel[count + lel.size() * 0.5];
        count++;
        acc = acc + (*myThickSurf.outer.coords)[no];
    }
    acc = acc * (1/(double)myThickSurf.outer.nNodes);
    
    Interfacer::update_inner_s(myThickSurf.inner, myThickSurf.outer, myThickSurf.thicknesses);
    Interfacer::update_bridges(myThickSurf);
    cout << "best area: " << algorithm.getBestFitness() << std::endl;
    cout << "best chromosome: ";
    for (size_t i = 0; i < lel.size(); i++)
    {
        cout << lel[i] << " ";
    } cout << lel.size() << endl;
    std::vector<SurfaceData_t*> surfaces;
    surfaces.push_back(&myThickSurf.outer);
    surfaces.push_back(&myThickSurf.inner);
    surfaces.push_back(&myThickSurf.bridges);
    cout << "Num ints: " << find_surface_intersections(surfaces, is) << std::endl;
    
    for (ListDigraph::NodeIt no(myThickSurf.outer.graph); no != INVALID; ++no)
    {
        (*myThickSurf.outer.coords)[no].x -= acc.x;
        (*myThickSurf.outer.coords)[no].y -= acc.y;
        (*myThickSurf.inner.coords)[no].x -= acc.x;
        (*myThickSurf.inner.coords)[no].y -= acc.y;
    }
    for (ListDigraph::NodeIt no(myThickSurf.bridges.graph); no != INVALID; ++no)
    {
        (*myThickSurf.bridges.coords)[no].x -= acc.x;
        (*myThickSurf.bridges.coords)[no].y -= acc.y;
    }
    
    
    for (size_t i = 0; i < is.size(); i++)
    {
        is[i].x -= acc.x;
        is[i].y -= acc.y;
    }
    
    // Rendering loop below:
    // -----------------------------------------
    while (!glfwWindowShouldClose(myRenderer.window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


     //   std::cout << mousePos << std::endl;
        
      //  glMatrixMode(GL_PROJECTION);
      //  glLoadIdentity();
      //  glOrtho(0.0, 0, 0, 0.0, -1.0, 1.0);
      //  glMatrixMode(GL_MODELVIEW);
      //  glLoadIdentity();

        myRenderer.render_text(fonti, "yo", point_t(-1, 0.76), 72);
        myRenderer.render_axes(fonti);
        myRenderer.render_surface(myThickSurf.outer, triple_t (1.0, 0.0, 1.0));
        myRenderer.render_surface(myThickSurf.inner, triple_t (0.7, 0.7, 0.3));
        myRenderer.render_surface(myThickSurf.bridges, triple_t (0.7, 0.3, 0.7));
        myRenderer.render_intersections(is);
        
        // ----
        // Conditional rendering below:
        // ----
        glfwGetCursorPos(myRenderer.window, &mousePos.x, &mousePos.y);
        mousePos.x = mousePos.x * (2.0 / myRenderer.wWidth) - 1;
        mousePos.y = myRenderer.wHeight - mousePos.y;
        mousePos.y = mousePos.y * (2.0 / myRenderer.wHeight) - 1;
        
        for (ListDigraph::NodeIt no(myThickSurf.outer.graph); no != INVALID; ++no)
        {
            if (dist(mousePos, (*myThickSurf.outer.coords)[ no ]) < 0.05)
            {
                myRenderer.render_node(no, myThickSurf.outer, triple_t (1.0, 0.0, 1.0));
                myRenderer.render_node(myThickSurf.inner.graph.nodeFromId(myThickSurf.outer.correspondence[myThickSurf.outer.graph.id(no)]), myThickSurf.inner, triple_t (0.7, 0.7, 0.3));
            }
        }
        glfwSwapBuffers(myRenderer.window);
        glfwPollEvents();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
    */
    return 0;
}
