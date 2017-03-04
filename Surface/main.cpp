//
//  main.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include <iostream>

#include "Interfacer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <lemon/list_graph.h>

const int NUM_POINTS = 20;


using namespace lemon;
using namespace std;

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------


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
    
       while (!glfwWindowShouldClose(window))
    {
        surface toRender;
        copySurface(&toRender, &mySurf);
        
        render(toRender);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
    
    
    
    return 0;
}
