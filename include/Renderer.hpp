#pragma once

#include <stdio.h>
#include <iostream>
#include <string>

#include "defs.hpp"
#include "Util.hpp"
#include "GLFW/glfw3.h"
#include "_2DSurface.hpp"

// #include <lemon/list_graph.h>
// #include <FTGL/ftgl.h>

#if defined(NANOGUI_GLAD)
    #if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
        #define GLAD_GLAPI_EXPORT
    #endif

    #include <glad/glad.h>
#else
    #if defined(__APPLE__)
        #define GLFW_INCLUDE_GLCOREARB
    #else
        #define GL_GLEXT_PROTOTYPES
    #endif
#endif

#include <nanogui/nanogui.h>

/* Renderer class which abstracts away the drawing of elements. */
class Renderer
{
  public:
	GLFWwindow *window;
	int wWidth = 1280;
	int wHeight = 920;

	Renderer();
	~Renderer();
	// TODO: Document graphical methods
	int initWindow();

	/* */
	void preLoopGL();

	void postLoopGL();

	void handle(int code);

	/* 
	 * Draws vertical and horizontal axes with step markers.

	 * @param font the FTGL font object which carries the glyphs with which to draw text. 
	 */
	void render_axes(/*FTGLPixmapFont &font*/);

	/*
	* Draws a surface.

	* @param surf the _2DSurface object to rasterise.
	* @param color the color with which to draw.
	* @param nodes optional boolean to determine whether to rasterise nodes or not.
	*/
	void render_surface(_2DSurface &surf, const triple_t color, bool nodes = true);

	/*
	void        render_text(FTGLPixmapFont &font, const char* text, point_t pos, int faceSize);

	void        render_text(FTGLTextureFont &font, const char* text, point_t pos, int faceSize);

	void        render_node(SNode no, const SurfaceData_t &surf, const triple_t color);

	static void render_intersections(std::vector<point_t> intersections); */
};
