#include "stdafx.hpp"
#include "Renderer.hpp"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

int Renderer::initWindow()
{
	if (!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(wWidth, wHeight, "Surface2D", NULL, NULL);

	if (!window)
	{
		return -1;
	}

	// Glew initialization skipped (I think glfw takes care of that)

	glfwMakeContextCurrent(window);

	return 1;
}

void Renderer::preLoopGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 0, 0, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Renderer::postLoopGL()
{
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Renderer::handle(int code)
{
	switch (code)
	{
	case -1:
		glfwTerminate();
		exit(EXIT_FAILURE);
	default:
		return;
	}
}

void Renderer::render_axes()
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
		//	sprintf(stringForm, "%.1f", marker);
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
			glEnd(); // But before we do that, we have to glEnd()
					 //	glEnable(GL_BLEND);
					 //	render_text(font, stringForm, point_t(marker, -0.02), 13); // and render our current text iteration ofc
			continue;
		}
		glColor3f(0.5f, 0.5f, .5f);
		glVertex2f(-0.009, marker);

		glColor3f(0.5f, 0.5f, .5f);
		glVertex2f(0, marker);
		glEnd();

		glEnable(GL_BLEND);
		//	render_text(font, stringForm, point_t(marker, -0.02), 13);
		//	render_text(font, stringForm, point_t(0.0, marker - 0.003), 13);
	}
}

void Renderer::render_surface(const _2DSurface &surf, const triple_t color, bool nodes)
{
	// Rasterise edges as lines
	glBegin(GL_LINES);
	int count = 0;
	for (ListDigraph::ArcIt e(*surf.graph); e != INVALID; ++e)
	{
		count++;
		glColor3f(color._1, color._2, color._3);
		glVertex2f((*surf.coords)[surf.graph->source(e)].x, (*surf.coords)[surf.graph->source(e)].y);

		glColor3f(color._1, color._2, color._3);
		glVertex2f((*surf.coords)[surf.graph->target(e)].x, (*surf.coords)[surf.graph->target(e)].y);
	}
	glEnd();

	// Rasterise nodes as squares if nodes is true
	if (nodes)
	{
		for (ListDigraph::NodeIt no(*surf.graph); no != INVALID; ++no)
		{
			glBegin(GL_TRIANGLE_STRIP); // Performance bonus when compared to Quads (2 triangles = 6 vertices)

			glColor3f(color._1, color._2, color._3);
			glVertex2f((*surf.coords)[no].x - 0.007f, (*surf.coords)[no].y + 0.007f);

			glColor3f(color._1, color._2, color._3);
			glVertex2f((*surf.coords)[no].x - 0.007f, (*surf.coords)[no].y - 0.007f);

			glColor3f(color._1, color._2, color._3);
			glVertex2f((*surf.coords)[no].x + 0.007f, (*surf.coords)[no].y + 0.007f);

			glColor3f(color._1, color._2, color._3);
			glVertex2f((*surf.coords)[no].x + 0.007f, (*surf.coords)[no].y - 0.007f);

			glEnd();
		}
	}
}