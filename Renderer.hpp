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


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <lemon/list_graph.h>


#include "Interfacer.hpp"

class Renderer{
public:
    GLFWwindow *window;
    GLuint      tex;
    
    int     initWindow();
    void    handle(int code);
    
    static void render_axes();
    
//  TODO:  static void render_text(const string text, point_t pos);
    
    static void render_surface(const SurfaceData_t &surf);
    
    static void render_intersections(const SurfaceData_t& surf);
};


#endif /* Renderer_hpp */
