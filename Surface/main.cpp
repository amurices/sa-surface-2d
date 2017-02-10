//
//  main.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include <iostream>
#include "defs.h"
#include "Auxiliares.hpp"
#include "Interfacer.hpp"
#include "SurfaceGenome.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const int NUM_POINTS = 8;

void render(const surface &surf)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBegin(GL_LINES);
    {
        glColor3f(0.5f, 0.5f, .5f);
        glVertex2f(0, 1);
        
        glColor3f(0.5f, 0.5f, .5f);
        glVertex2f(0, -1);
        
        
        glColor3f(0.5f, 0.5f, .5f);
        glVertex2f(1, 0);
        
        glColor3f(0.5f, 0.5f, .5f);
        glVertex2f(-1, 0);
        
        for(size_t i = 0; i < surf.e.size(); i++)
        {
            glColor3f(1.0f, 0.0f, 1.0f);
            glVertex2f(surf.e[i].first->x, surf.e[i].first->y);
            
            glColor3f(1.0f, 0.0f, 1.0f);
            glVertex2f(surf.e[i].second->x, surf.e[i].second->y);
            
            std::vector<std::pair<float, float> > where;
            int cnti = countIntersections(surf, where);
            std::cout << "intersects of best: " << cnti;
            if (cnti > 0)
            {
                std::cout << ", at ";
                for (size_t j = 0; j < where.size(); j++)
                {
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glVertex2f(where[j].first - 0.005, where[j].second -0.005);
                    
                    glColor3f(1.0f, 0.0f, .0f);
                    glVertex2f(where[j].first + 0.005, where[j].second +0.005);
                    
                    
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glVertex2f(where[j].first + 0.005, where[j].second -0.005);
                    
                    glColor3f(1.0f, 0.0f, .0f);
                    glVertex2f(where[j].first - 0.005, where[j].second +0.005);
                }
            }
           
        }
    }
    glEnd();
}
surface mySurf = Interfacer::generate_random(2.0, NUM_POINTS);


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
            fitness -= LineSegmentIntersection(offsetSurface.e[i].first->x, offsetSurface.e[i].first->y,
                                               offsetSurface.e[i].second->x, offsetSurface.e[i].second->x,
                                               offsetSurface.e[j].first->x, offsetSurface.e[j].first->y,
                                               offsetSurface.e[j].second->x, offsetSurface.e[j].second->x,
                                               &m1, &c1, &m2, &c2,
                                               &intersection_X, &intersection_Y);
        }
    }
    fitness = powf(2.0, -fitness);
    
    return fitness;
    
    /*   */
}

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
