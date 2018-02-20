//
//  Renderer.hpp
//  Surface
//
//  Created by André Muricy on 8/3/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <stdio.h>
#include <iostream>
#include <string>


#include <GLFW/glfw3.h>
#include <lemon/list_graph.h>
#include <FTGL/ftgl.h>

#include "Interfacer.hpp"


class Renderer{
public:
    GLFWwindow *window;
    int wWidth = 1200;
    int wHeight = 1200;
    
	Renderer(int n);

    int     initWindow();
    
    void    handle(int code);
    
    void        render_axes(FTGLPixmapFont &font);
    
	void        render_text(FTGLPixmapFont &font, const char* text, point_t pos, int faceSize);

	void        render_text(FTGLTextureFont &font, const char* text, point_t pos, int faceSize);

    void        render_node(SNode no, const SurfaceData_t &surf, const triple_t color);
    
    static void render_surface(const SurfaceData_t &surf, const triple_t color);
    
    static void render_intersections(std::vector<point_t> intersections);
};


#endif /* Renderer_hpp */
