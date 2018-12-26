// Surface2D 2.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.hpp"
#include "Renderer.hpp"
#include "Optimizer.hpp"
#include "ThickSurface.hpp"

int main(int argc, char **argv)
{
	// Declarations and instantiations:
	// -----------------------------------------
	Renderer myRenderer;
	// -----------------------------------------

	// Initialization and error-handling calls:
	// -----------------------------------------
	int init = 0;
	init |= myRenderer.initWindow(); // init Renderer, a wrapper around glfw and opengl functionality
	myRenderer.handle(init);		 // Window initialization handling
	// -----------------------------------------

	ThickSurface mySurface;
	mySurface.generateCircularThickSurface(1, 300, true, 0.15, 0.15, point_t(0.0, 0.0));
	double perim, a0 = mySurface.outer->findSurfaceAreaAndPerimeter(perim) - mySurface.inner->findSurfaceAreaAndPerimeter(perim);

	ThickSurface *theNeighbor = &mySurface;

	std::set<SNode> changed;
	mySurface.inner->updateInnerSurface(*mySurface.outer, changed, mySurface.thicknesses);
	int ticks = 0;
	Optimizer myOpt;
	// Loop is testing copying, deleting, and modification
	double temperature = 100;

	while (!glfwWindowShouldClose(myRenderer.window))
	{

		myRenderer.preLoopGL();
		myRenderer.render_axes(); // Swap this for a RenderObject list
		if (ticks == 0)
		{
			temperature -= 1.0;
			myOpt.step_sa(mySurface, temperature, a0);
		}

		ticks++;

		if (ticks < 6)
		{
			myRenderer.render_surface(*mySurface.outer, triple_t(0.1, 0.2, 0.3), false);
			myRenderer.render_surface(*mySurface.inner, triple_t(0.1, 0.2, 0.45), false);
		}
		// else if (ticks < 12)
		// {
		// 	myRenderer.render_surface(*theNeighbor->outer, triple_t(0.75, 0.75, 0.5), false);
		// 	myRenderer.render_surface(*theNeighbor->inner, triple_t(0.75, 0.75, 0.5), false);
		// }
		else
		{
			ticks = 0;
		}
		myRenderer.postLoopGL();
	}
	glfwTerminate();

	return 0;
}
