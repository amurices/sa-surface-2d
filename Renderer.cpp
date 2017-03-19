//
//  Renderer.cpp
//  Surface
//
//  Created by André Muricy on 8/3/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Renderer.hpp"

const int NUM_POINTS = 20;

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------


int Renderer::init()
{
    if (!glfwInit())
    {
        return -1;
    }
    
    window = glfwCreateWindow(640, 480, "Hi, im trying", NULL, NULL);
    if (!window)
    {
        return -1;
    }
    
    if (!glewInit())
    {
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Inicialização de artefatos relacionados a texturas
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(uniform_tex, 0);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glEnableVertexAttribArray(attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
    
    return 1;
}

void Renderer::handle(int code)
{
    switch (code) {
        case -1:
            glfwTerminate();
            exit(EXIT_FAILURE);
        default:
            return;
    }
}

void Renderer::render_text(const string text, point_t pos)
{
   
}

// Desenha todas as arestas da superfície
void Renderer::render_surface(const SurfaceData_t &surf)
{
    for (ListDigraph::ArcIt e(surf.graph); e != INVALID; ++e )
    {
        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex2f((*surf.coords)[ surf.graph.source(e) ].x, (*surf.coords)[ surf.graph.source(e) ].y);
        
        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex2f((*surf.coords)[ surf.graph.target(e) ].x , (*surf.coords)[ surf.graph.target(e) ].y);
    }
}

// Desenha os eixos X e Y, cinzentos
void Renderer::render_axes()
{
    glColor3f(0.5f, 0.5f, .5f);
    glVertex2f(0, 1);
    
    glColor3f(0.5f, 0.5f, .5f);
    glVertex2f(0, -1);
    
    
    glColor3f(0.5f, 0.5f, .5f);
    glVertex2f(1, 0);
    
    glColor3f(0.5f, 0.5f, .5f);
    glVertex2f(-1, 0);
    
}


// Renderiza interseções com um pequeno "x" vermelho no lugar
void Renderer::render_intersections(const SurfaceData_t& surf)
{
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

/* void Renderer::render_text(const string text, point_t pos)
{
    
} */

