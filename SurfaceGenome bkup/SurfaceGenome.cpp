//
//  SurfaceGenome.cpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "SurfaceGenome.hpp"



// old objective
float Objective(GAGenome& g) {
    SurfaceGenome&genomaTeste = (SurfaceGenome &) g;
    surface offsetSurface;
    
    copySurface(&offsetSurface, &mySurf);
    
    float fitness = 1000;
    float m1, c1, m2, c2, intersection_X, intersection_Y;
    
    for (size_t i = 0; i < offsetSurface.v.size(); i++)
    {
        offsetSurface.v[i].x += genomaTeste.gene((unsigned int)i);
        offsetSurface.v[i].y += genomaTeste.gene((unsigned int)i);
    }
    
    for (size_t i = 0; i < offsetSurface.e.size(); i++)
    {
        for (int j = (int)offsetSurface.e.size() - 1; j > i; j--)
        {
            link_g a, b;
            a = offsetSurface.e[i];
            b = offsetSurface.e[j];
            if (intersect(a, b))
                fitness--;
            /*  fitness -= LineSegmentIntersection(offsetSurface.e[i].first->x, offsetSurface.e[i].first->y,
             offsetSurface.e[i].second->x, offsetSurface.e[i].second->x,
             offsetSurface.e[j].first->x, offsetSurface.e[j].first->y,
             offsetSurface.e[j].second->x, offsetSurface.e[j].second->x,
             &m1, &c1, &m2, &c2,
             &intersection_X, &intersection_Y); */
        }
    }
    fitness = powf(2.0, -fitness);
    
    return fitness;
    
    /*   */
}


// old main
int main(int argc, const char * argv[]) {
    GLFWwindow *window;
    
    
    if (!glfwInit())
    {
        return -1;
    }
    
    window = glfwCreateWindow(640, 480, "Hi, im trying", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    
    if (!glewInit())
    {
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    // Criar um genoma (pra testar)
    GAAlleleSet<float> alelosGenomaTeste;
    for (int i = 0; i < 10000; i++) // <- Coisa mais imbecil do mundo; framework n tá ajudando
    {
        alelosGenomaTeste.add(-.5f + ((float)i) / 10000);
    }
    SurfaceGenome genomaTeste(NUM_POINTS * 2, alelosGenomaTeste, Objective);
    
    // Criar um GA pra testar essa porra;
    GASimpleGA gaTeste(genomaTeste);
    
    int counter = 0;
    while (!glfwWindowShouldClose(window))
    {
        counter++;
        gaTeste.step();
        //  std::cout << "gen " << counter << "'s best individual:\n";
        // std::cout << gaTeste.statistics().bestIndividual() << std::endl;
        
        surface toRender;
        copySurface(&toRender, &mySurf);
        
        std::vector<float> jarilson;
        std::stringstream buf;
        buf << gaTeste.statistics().bestIndividual();
        jarilson =  convertStringVectortoDoubleVector( split(buf.str(), ' ') );
        
        for (size_t i = 0; i < toRender.v.size(); i++)
        {
            toRender.v[i].x += jarilson[i];
            toRender.v[i].y += jarilson[i + toRender.v.size()];
        }
        
        std::cout << std::endl;
        render(toRender);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
    
    
    // insert code here...
    
    for (size_t i = 0; i < mySurf.v.size(); i++)
    {
        std::cout << "V" << i << "; X: " << mySurf.v[i].x << ", Y: " << mySurf.v[i].y <<
        std::endl;
    }
    
    return 0;
}
