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


int Renderer::initWindow()
{
    if (!glfwInit())
    {
        return -1;
    }

    window = glfwCreateWindow(wWidth, wHeight, "Hi, im trying", NULL, NULL);

    if (!window)
    {
        return -1;
    }

    if (!glewInit())
    {
        return -1;
    }
    glfwMakeContextCurrent(window);

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


void Renderer::render_text(FTGLPixmapFont &font, const char* text, point_t pos, int faceSize)
{
    double mulX = pos.x + 1;
    double mulY = pos.y + 1;

    font.FaceSize(faceSize);
    font.Render(text, -1, FTPoint((double) wWidth * mulX, (double) wHeight * mulY, 0.0));
}

// Draw individual node as triangle
void Renderer::render_node(SNode no, const SurfaceData_t &surf, const triple_t color)
{
    glBegin(GL_TRIANGLES);

    point_t pos = (*surf.coords)[ no ];
    glColor3f(color._1, color._2, color._3);
    glVertex2f(pos.x, pos.y - 0.02);

    glColor3f(color._1, color._2, color._3);
    glVertex2f(pos.x - 0.02, pos.y + 0.02);

    glColor3f(color._1, color._2, color._3);
    glVertex2f(pos.x + 0.02, pos.y + 0.02);

    glEnd();
}


// Draws the entire surface
void Renderer::render_surface(const SurfaceData_t &surf, const triple_t color)
{
    // Rasterise edges as lines
    glBegin(GL_LINES);
    int count = 0;
    for (ListDigraph::ArcIt e(surf.graph); e != INVALID; ++e )
    {
        count++;
        glColor3f(color._1, color._2, color._3);
        glVertex2f((*surf.coords)[ surf.graph.source(e) ].x, (*surf.coords)[ surf.graph.source(e) ].y);
        glColor3f(color._1, color._2, color._3);
        glVertex2f((*surf.coords)[ surf.graph.target(e) ].x , (*surf.coords)[ surf.graph.target(e) ].y);
    }
 //   std::cout << "Rendered: " << count << std::endl;
    glEnd();

    // Rasterise nodes as squares
  /*  for (ListDigraph::NodeIt no(surf.graph); no != INVALID; ++no)
    {
        glBegin(GL_TRIANGLE_STRIP); // Ganho de eficiência se comparado a Quads (dois triângulos = 6 vértices)

        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex2f((*surf.coords)[ no ].x - 0.007f, (*surf.coords)[ no ].y + 0.007f);

        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex2f((*surf.coords)[ no ].x - 0.007f, (*surf.coords)[ no ].y - 0.007f);

        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex2f((*surf.coords)[ no ].x + 0.007f, (*surf.coords)[ no ].y + 0.007f);

        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex2f((*surf.coords)[ no ].x + 0.007f, (*surf.coords)[ no ].y - 0.007f);

        glEnd();
    } */

}

// Desenha os eixos X e Y, cinzentos, e demarca cada 0.1 (segundo a escala) caminhado
void Renderer::render_axes(FTGLPixmapFont &font)
{
    glBegin(GL_LINES);

    glColor3f(0.5f, 0.5f, .5f);
    glVertex2f(0, 1);

    glColor3f(0.5f, 0.5f, .5f);
    glVertex2f(0, -1);


    glColor3f(0.5f, 0.5f, .5f);
    glVertex2f(1, 0);

    glColor3f(0.5f, 0.5f, .5f);
    glVertex2f(-1, 0);
    glEnd();

    for (double marker = -1.0; marker < 1.0; marker += 0.1)
    {
        char stringForm[10];
        sprintf(stringForm, "%.1f", marker);
        // Small lines are drawn at the marker's position
        glBegin(GL_LINES);

        // X-axis:
        glColor3f(0.5f, 0.5f, .5f);
        glVertex2f(marker, -0.009);

        glColor3f(0.5f, 0.5f, .5f);
        glVertex2f(marker, 0);

        // Y-axis:
        if (marker <= 0.01 && marker >= -0.01) // We can skip the Y axis if it's the origin
        {
            glEnd();                            // But before we do that, we have to glEnd()
            render_text(font, stringForm, point_t(marker, -0.02), 13); // and render our current text iteration ofc
            continue;
        }
        glColor3f(0.5f, 0.5f, .5f);
        glVertex2f(-0.009, marker);

        glColor3f(0.5f, 0.5f, .5f);
        glVertex2f(0, marker);
        glEnd();

       render_text(font, stringForm, point_t(marker, -0.02), 13);
       render_text(font, stringForm, point_t (0.0, marker - 0.003), 13);

    }

}


// Renderiza interseções com um pequeno "x" vermelho no lugar
void Renderer::render_intersections(std::vector<point_t> intersections)
{
    glBegin(GL_LINES);
    for (size_t j = 0; j < intersections.size(); j++)
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(intersections[j].x - 0.005, intersections[j].y -0.005);

        glColor3f(1.0f, 0.0f, .0f);
        glVertex2f(intersections[j].x + 0.005, intersections[j].y +0.005);


        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(intersections[j].x + 0.005, intersections[j].y -0.005);

        glColor3f(1.0f, 0.0f, .0f);
        glVertex2f(intersections[j].x - 0.005, intersections[j].y +0.005);
    }
    glEnd();
}
