
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


const int NUM_POINTS = 300;


using namespace lemon;
using namespace std;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    // Rendering loop variable declarations below:
    // -----------------------------------------
    ThickSurface_t myThickSurf;         // the surface itself. ThickSurface_t is a wrapper around SurfaceData_t
    Interfacer::generate_random(myThickSurf, 2.0, NUM_POINTS);
    //Interfacer::get_from_matlab(mySurf, "matlab.mat");

    FTGLPixmapFont fonti("/Library/Fonts/Arial.ttf");                   // Load .ttf file to memory
    
    point_t mousePos;                                           // Mouse position for (minimal) input handling
    
    Renderer        myRenderer; // Renderer instance (so it can store some loop-related stuff transparently)
    int init =      myRenderer.initWindow();    // glfw window wrapper
    // -----------------------------------------
    
    
    // Error-handling calls:
    // -----------------------------------------
    myRenderer.handle(init);                    // Window initialization handling
    if(fonti.Error()) exit(EXIT_FAILURE);       // Font loading handling
    // -----------------------------------------

    
    
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
        myRenderer.render_surface(myThickSurf.outer);
        myRenderer.render_surface(myThickSurf.inner);
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
                myRenderer.render_node(no, myThickSurf.outer);
        }
       
        
        // Calls to objects to be drawn go here
        
        
        
        glfwSwapBuffers(myRenderer.window);
        glfwPollEvents();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
    
    return 0;
}

/*
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

#include "MTRand.h"
#include "BRKGA.h"


class SampleDecoder {
public:
    SampleDecoder(){
    }
    ~SampleDecoder()
    {
    }
    
    double decode(const std::vector< double >& chromosome) const {
        std::vector< std::pair< double, unsigned > > ranking(chromosome.size());
        
        for(unsigned i = 0; i < chromosome.size(); ++i) {
            ranking[i] = std::pair< double, unsigned >(chromosome[i], i);
        }
        
        // Here we sort 'permutation', which will then produce a permutation of [n] in pair::second:
        std::sort(ranking.begin(), ranking.end());
        
        // permutation[i].second is in {0, ..., n - 1}; a permutation can be obtained as follows
        std::list< unsigned > permutation;
        for(std::vector< std::pair< double, unsigned > >::const_iterator i = ranking.begin();
            i != ranking.end(); ++i) {
            permutation.push_back(i->second);
        }
        // sample fitness is the first allele
        return chromosome.front();
    }
private:
};


int main(int argc, char* argv[]) {
    const unsigned n = 100;		// size of chromosomes
    const unsigned p = 1000;	// size of population
    const double pe = 0.20;		// fraction of population to be the elite-set
    const double pm = 0.10;		// fraction of population to be replaced by mutants
    const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
    const unsigned K = 3;		// number of independent populations
    const unsigned MAXT = 1;	// number of threads for parallel decoding
    
    SampleDecoder decoder;			// initialize the decoder
    
    const long unsigned rngSeed = 0;	// seed to the random number generator
    MTRand rng(rngSeed);				// initialize the random number generator
    
    // initialize the BRKGA-based heuristic
    BRKGA< SampleDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
    
    unsigned generation = 0;		// current generation
    const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
    const unsigned X_NUMBER = 2;	// exchange top 2 best
    const unsigned MAX_GENS = 100;	// run for 100 gens
    printf("Aqui o caralho\n");
    do {
        printf("Aqui o caralho %d\n", generation);
        
        algorithm.evolve();	// evolve the population for one generation
        
        if((++generation) % X_INTVL == 0) {
            algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
        }
    } while (generation < MAX_GENS);
    
    std::cout << "Best solution found has objective value = "
	 		<< algorithm.getBestFitness() << std::endl;
    
    return 0;
} */
