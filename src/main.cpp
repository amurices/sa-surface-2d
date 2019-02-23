// Surface2D 2.0.cpp : Defines the entry point for the console application.
//
#include <nanogui/nanogui.h>

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
	mySurface.generateCircularThickSurface(1, 200, true, 0.12, 0.12, point_t(0.0, 0.0));
	double perim;
	ThickSurface *theNeighbor = &mySurface;

	std::set<SNode> changed;
	mySurface.inner->updateInnerSurface(*mySurface.outer, changed, mySurface.thicknesses);
	int ticks = 0;
	Optimizer myOpt;

	double scale = 1.0;				 // Index that will adjust some hyperparameters of evolution
	double a0 = mySurface.outer->findSurfaceAreaAndPerimeter(perim) - mySurface.inner->findSurfaceAreaAndPerimeter(perim);
	int smooth = 10;				 // Whether neighbors should be calculated using smoothing depressions or not
	double diffPow = 1.0;			 // Power to raise difference btwn A0 and AS
	double diffMul = 1.0;			 // Scalar ^
	double areaPow = 1.0;			 // Analogous to
	double areaMul = 1.0;			 // previous two
	double multiProb = 0.0;			 // Probability of forcing another vertex after the first one
	double tempProb = 1.0;			 // How much weight does temperature hold
	double forceOffsetRange = 0.066; // How much can points be shifted every iteration
	double compression = 1.0;		 // How much should cortex be compressed by force
	InitSaParams theseParams(scale, a0, smooth, diffPow, diffMul, areaPow, areaMul, multiProb, tempProb, forceOffsetRange, compression);
	myOpt.params = &theseParams;

	// Loop is testing copying, deleting, and modification
	double temperature = 0;

	while (!glfwWindowShouldClose(myRenderer.window))
	{

		myRenderer.preLoopGL();
		myRenderer.render_axes(); // Swap this for a RenderObject list
		if (ticks == 0)
		{
			myOpt.step_saV2(mySurface, &temperature, a0);
		}

		ticks++;

		if (ticks < 6)
		{
			myRenderer.render_surface(*mySurface.outer, triple_t(0.1, 0.2, 0.3), false);
			myRenderer.render_surface(*mySurface.inner, triple_t(0.1, 0.2, 0.45), false);
		}
		else
		{
			ticks = 0;
		}
		myRenderer.postLoopGL();
	}
	glfwTerminate();

	return 0;
}
