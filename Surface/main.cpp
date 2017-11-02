
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
/*
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Sweep_line_2_algorithms.h>
*/
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

const int NUM_POINTS = 200;
const int NUM_GENS = 6500;
const int NUM_SMOOTH = 10;

const int numTests = 20;
const int numFixed = 3;
const int numParams = 2;

const int smoothVar = 8;
const double thicknessVar = 0.1;
const double RADIUS = 0.75;
const double THICKNESS = 0.05;

using namespace lemon;
using namespace std;

void init_optimizers(Optimizer *opt,        std::vector<Optimizer*> &opts,
                     ThickSurface_t *thick, std::vector<ThickSurface_t> &thicks,
                     InitSaParams params, bool multi, std::vector<point_t> &is,
                     ThickSurface_t *innerCircle);


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main(int argc, char** args) {
    // Rendering loop variable declarations below:
    // -----------------------------------------
    ThickSurface_t innerCircle; // the surface itself. ThickSurface_t is a wrapper around SurfaceData
    ThickSurface_t nghbr;

    SurfaceData_t hull; // To hold the "exposed" surface and eventually calculate foldedness
    
    ofstream output;
    ofstream output_t;
    ifstream input; // File holding simulation parameters
    input.open("input.txt");
    std::string inputReader;
    
    InitSaParams params;
    getline(input, inputReader);
    params.radius           = atof(inputReader.c_str());
    getline(input, inputReader);
    params.thickness        = atof(inputReader.c_str());
    getline(input, inputReader);
    params.scale            = atof(inputReader.c_str());
    getline(input, inputReader);
    params.smooth           = atoi(inputReader.c_str());
    getline(input, inputReader);
    params.diffMul          = atof(inputReader.c_str());
    getline(input, inputReader);
    params.diffPow          = atof(inputReader.c_str());
    getline(input, inputReader);
    params.areaMul          = atof(inputReader.c_str());
    getline(input, inputReader);
    params.areaPow          = atof(inputReader.c_str());
    getline(input, inputReader);
    params.multiProb        = atof(inputReader.c_str());
    getline(input, inputReader);
    params.forceOffsetRange = atof(inputReader.c_str());
    getline(input, inputReader);
    params.temperature      = atof(inputReader.c_str());
    getline(input, inputReader);
    params.compression      = atof(inputReader.c_str());
    
    bool multiTests = false;
    
    vector<ThickSurface_t> myThickSurfaces(numTests * numFixed * numParams);
    vector<Optimizer*> optimizers(numTests * numFixed * numParams);
    
    
    std::vector<point_t> is;    // container to hold intersections
    /*
    InitSaParams params;
    params.radius           = RADIUS;
    params.thickness        = THICKNESS;
    params.scale            = 0.1;
    params.smooth           = NUM_SMOOTH;
    params.diffMul          = 1.0;
    params.diffPow          = 1.0;
    params.areaPow          = 1.0;
    params.areaMul          = 1.0;
    params.multiProb        = 0.4;
    params.forceOffsetRange = 0.166;
    params.temperature      = 0;
    params.compression      = 1;
    */
    init_optimizers(NULL, optimizers, NULL, myThickSurfaces, params, true, is, &innerCircle);

    Interfacer::generate_circle(innerCircle, RADIUS * 0.45, 0.13, NUM_POINTS, is);
    
    
    // Testing variable thickness, fixed smoothness
 
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
    
    double Ae = 0;
    double At = 0;
    
    // Rendering loop below:
    // -----------------------------------------
    int count = 0; int currentThick = 0;
    bool wasDown = false; bool pause = true;
    bool jWasDown = false; bool slowMo = false;

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

        if (!pause && count < NUM_GENS)
        {
       //     opt.step_sa();
            optimizers[currentThick]->gen++;
            optimizers[currentThick]->step_sa();
            count++;
        }
        
        else if (count >= NUM_GENS)
        {
            Interfacer::generate_hull(hull, optimizers[currentThick]->org->outer);
            calculate_surface_area(hull, Ae);
            calculate_surface_area(optimizers[currentThick]->org->outer, At);
         //   sqrt(optimizers[currentThick]->org->thickness) * At;
        }
  //      T1/2AG = kAE1.25,
        if (count > NUM_GENS)
        {
            count++;
            myRenderer.render_surface(hull, triple_t (0.6, 0.6, 1.0));
        }
        
        // Compile these later into a hyperparam struct. We want:
        char gen[16];   // Gen number
        char nrg[16];   // free energy (outer area + (diffMul * stretch ^ diffPow))
        char nrgt[10];  // use it for nrg/time plot
        char wht[16];   // white matter (inner area)
        char gry[16];   // gray matter (outer area - inner area)
        char dif[16];   // stretch factor (initial gray matter - current gray matter)
        char prm[16];   // perimeter (sum of all distances)
        char exp[16];   // exposed perimeter (sum of all distances)
        char thk[16];   // thickness of this solution
        char smt[16];   // thickness of this solution
        std::sprintf(gen, "gen %d", count);
        std::sprintf(nrg, "nrg %.7f", optimizers[currentThick]->energy);
        std::sprintf(wht, "wht %.7f", optimizers[currentThick]->white);
        std::sprintf(gry, "gry %.7f", optimizers[currentThick]->gray);
        std::sprintf(dif, "dif %.7f", optimizers[currentThick]->stretch);
        std::sprintf(prm, "prm %.7f", optimizers[currentThick]->perimeter);
        std::sprintf(thk, "thk %.7f", (optimizers[currentThick]->org->thickness)[0]);
        std::sprintf(smt, "smt %d", optimizers[currentThick]->smooth);
     
     /*   std::sprintf(gen, "gen %d", count);
        std::sprintf(nrg, "nrg %.7f", opt.energy);
        std::sprintf(wht, "wht %.7f", opt.white);
        std::sprintf(gry, "gry %.7f", opt.gray);
        std::sprintf(dif, "dif %.7f", opt.stretch);
        std::sprintf(prm, "prm %.7f", opt.perimeter);
        std::sprintf(thk, "thk %.7f", opt.org->thickness);
        std::sprintf(smt, "smt %d", opt.smooth);
      */
        
        output_t << gen << endl;
        output_t << nrg << endl;
        output_t << wht << endl;
        output_t << gry << endl;
        output_t << dif << endl;
        output_t << prm << endl;
        output_t << thk << endl;
        output_t << smt << endl;

        if (count >= NUM_GENS)
        {
            Interfacer::generate_hull(hull, optimizers[currentThick]->org->outer);
            calculate_surface_area(hull, Ae);

            std::sprintf(exp, "exp %.7f", Ae);

            count = 0;
            output << nrg << endl;
            output << wht << endl;
            output << gry << endl;
            output << dif << endl;
            output << prm << endl;
            output << exp << endl;
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
        Interfacer::generate_hull(hull, optimizers[currentThick]->org->outer);
        myRenderer.render_surface(hull, triple_t (1.0, 1.0, 1.0));
        myRenderer.render_surface(optimizers[currentThick]->org->outer, triple_t (1.0, 0.0, 1.0));
        myRenderer.render_surface(optimizers[currentThick]->org->inner, triple_t (0.7, 0.7, 0.3));
     //   myRenderer.render_surface(optimizers[currentThick]->org->bridges, triple_t (0.7, 0.3, 0.7));
        myRenderer.render_surface(optimizers[currentThick]->innerCircle->outer, triple_t(0.5, 0.3, 0.2));

        myRenderer.render_axes(fonti);

     /*   myRenderer.render_surface(opt.org->outer, triple_t (1.0, 0.0, 1.0));
        myRenderer.render_surface(opt.org->inner, triple_t (0.7, 0.7, 0.3));
        myRenderer.render_surface(opt.org->bridges, triple_t (0.7, 0.3, 0.7));
      */
    //    myRenderer.render_surface(nghbr.outer, triple_t (0.0, .5, 1.0));
     //   myRenderer.render_surface(nghbr.inner, triple_t (0.7, 0.9, 0.0));
    //    myRenderer.render_surface(nghbr.bridges, triple_t (0.3, 0.3, 0.9));
        
        
    //    myRenderer.render_intersections(is);
        
        // ----------------------------------------------------
        // Input processing and Conditional rendering below:
        // ----------------------------------------------------
        if (slowMo)
            if (pause == false) pause = true;
        
        glfwGetCursorPos(myRenderer.window, &mousePos.x, &mousePos.y);
        mousePos.x = mousePos.x * (2.0 / myRenderer.wWidth) - 1;
        mousePos.y = myRenderer.wHeight - mousePos.y;
        mousePos.y = mousePos.y * (2.0 / myRenderer.wHeight) - 1;
        
        int space_state = glfwGetKey(myRenderer.window, GLFW_KEY_SPACE);
        int j_state = glfwGetKey(myRenderer.window, GLFW_KEY_J);
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
        
        // Press J key to enter slow-mo mode; one iteration at a time
        if (j_state == GLFW_PRESS)
        {
            if (!jWasDown)
            {
                jWasDown = true;
                if (!pause)
                    pause = true; // if unpaused, pause to start slowMo mode
                slowMo = !slowMo; // flip the state
            }
        }
        else
            jWasDown = false;
        
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


void init_optimizers(Optimizer *opt,        std::vector<Optimizer*> &opts,
                     ThickSurface_t *thick, std::vector<ThickSurface_t> &thicks,
                     InitSaParams params, bool multi, std::vector<point_t> &is,
                     ThickSurface_t *innerCircle)
{
    // Testing variable thickness, fixed smoothness
    for (int i = 0; i < numTests; i++)
    {
        Interfacer::generate_circle(thicks[i], params.radius, thicknessVar * (i + 1), NUM_POINTS, is);
        opts[i] = new Optimizer(thicks[i]);
        opts[i]->init_SA(params.scale, NUM_SMOOTH,
                         params.diffPow, params.diffMul,
                         params.areaPow, params.areaMul,
                         params.multiProb, params.forceOffsetRange, params.temperature, params.compression);
        opts[i]->innerCircle = innerCircle;
     //   opts[i]->init_SA(0.1, NUM_SMOOTH, 1, 2); // Why does increasing Pow decrease pS?
        
        Interfacer::generate_circle(thicks[i+numTests], params.radius, thicknessVar * (i + 1), NUM_POINTS, is);
        opts[i+numTests] = new Optimizer(thicks[i+numTests]);
        opts[i+numTests]->init_SA(params.scale, NUM_SMOOTH + smoothVar,
                         params.diffPow, params.diffMul,
                         params.areaPow, params.areaMul,
                         params.multiProb, params.forceOffsetRange, params.temperature, params.compression);
        opts[i+numTests]->innerCircle = innerCircle;

    //    opts[i+numTests]->init_SA(0.1, NUM_SMOOTH + smoothVar, 1, 2); // Why does increasing Pow decrease pS?
        
        Interfacer::generate_circle(thicks[i+2*numTests], params.radius, thicknessVar * (i + 1), NUM_POINTS, is);
        opts[i+2*numTests] = new Optimizer(thicks[i+2*numTests]);
        opts[i+2*numTests]->init_SA(params.scale, NUM_SMOOTH + 2*smoothVar,
                         params.diffPow, params.diffMul,
                         params.areaPow, params.areaMul,
                         params.multiProb, params.forceOffsetRange, params.temperature, params.compression);
        opts[i+2*numTests]->innerCircle = innerCircle;

    //    opts[i+2*numTests]->init_SA(0.1, NUM_SMOOTH + 2*smoothVar, 1, 2); // Why does increasing Pow decrease pS?
    }
    
    // Testing variable smoothness, fixed thickness
    for (int i = numTests * numFixed; i < numTests * (numFixed + 1); i++)
    {
        Interfacer::generate_circle(thicks[i], params.radius, 0.05, NUM_POINTS, is);
        opts[i] = new Optimizer(thicks[i]);
        opts[i]->init_SA(params.scale, NUM_SMOOTH - 2 + i - (numTests * numFixed),
                         params.diffPow, params.diffMul,
                         params.areaPow, params.areaMul,
                         params.multiProb, params.forceOffsetRange,
                         params.temperature, params.compression);
        opts[i]->innerCircle = innerCircle;
    //   opts[i]->init_SA(0.1, NUM_SMOOTH - 2 + i - (numTests * numFixed), 1, 2); // Why does increasing Pow decrease pS?
        
        Interfacer::generate_circle(thicks[i+numTests], params.radius, 0.15, NUM_POINTS, is);
        opts[i+numTests] = new Optimizer(thicks[i+numTests]);
        opts[i+numTests]->init_SA(params.scale, NUM_SMOOTH - 2 + i - (numTests * numFixed),
                         params.diffPow, params.diffMul,
                         params.areaPow, params.areaMul,
                         params.multiProb, params.forceOffsetRange, params.temperature, params.compression);
        opts[i+numTests]->innerCircle = innerCircle;

     //   opts[i+numTests]->init_SA(0.1, NUM_SMOOTH - 2 + i - (numTests * numFixed), 1, 2); // Why does increasing Pow decrease pS?
        
        Interfacer::generate_circle(thicks[i+2*numTests], params.radius, 0.25, NUM_POINTS, is);
        opts[i+2*numTests] = new Optimizer(thicks[i+2*numTests]);
        opts[i+2*numTests]->init_SA(params.scale, NUM_SMOOTH - 2 + i - (numTests * numFixed),
                                  params.diffPow, params.diffMul,
                                  params.areaPow, params.areaMul,
                                  params.multiProb, params.forceOffsetRange, params.temperature, params.compression);
        opts[i+2*numTests]->innerCircle = innerCircle;

     //   opts[i+2*numTests]->init_SA(0.1, NUM_SMOOTH - 2 + i - (numTests * numFixed), 1, 2); // Why does increasing Pow decrease pS?
    }
    
}

/*
 Optimizer opt(myThickSurf);
 opt.init_SA(0.1, NUM_SMOOTH, 1, 2);
 opt.neighbor(myThickSurf, nghbr);
 */
/*
 const unsigned n = 2 * NUM_POINTS;        // size of chromosomes
 const unsigned p = 400;     // size of population
 const double pe = 0.20;        // fraction of population to be the elite-set
 const double pm = 0.10;        // fraction of population to be replaced by mutants
 const double rhoe = 0.70;    // probability that offspring inherit an allele from elite parent
 const unsigned K = 3;        // number of independent populations
 const unsigned MAXT = 1;    // number of threads for parallel decoding
 
 SurfaceDecoder decoder;            // initialize the decoder
 
 const long unsigned rngSeed = 0;    // seed to the random number generator
 MTRand rng(rngSeed);                // initialize the random number generator
 
 
 decoder.org = &myThickSurf;
 Optimizer opt(myThickSurf, rng, decoder);
 opt.init_GA(p, pe, pm, rhoe, K, MAXT, 100, 2, 800);
 opt.evolve_ga();
 opt.update_surface_ga(opt.bestSolution);
 */
