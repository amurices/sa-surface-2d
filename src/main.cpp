// Surface2D 2.0.cpp : Defines the entry point for the console application.
//
#include <nanogui/nanogui.h>

#include "Renderer.hpp"
#include "Optimizer.hpp"
#include "ThickSurface.hpp"

int main(int argc, char **argv)
{
	nanogui::init();
	// Declarations and instantiations:
	// -----------------------------------------
	nanogui::ref<Renderer> myRenderer = new Renderer();
	// -----------------------------------------

	ThickSurface mySurface;
	mySurface.generateCircularThickSurface(1, 20, true, 0.024, 0.024, point_t(0.0, 0.0));
	double perim;

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

	myRenderer->setVisible(true);
	myRenderer->thickSurface = &mySurface;
	myRenderer->optimizer = &myOpt;
	double temperature = 0;
	myRenderer->uploadIndices();
	while (!glfwWindowShouldClose(myRenderer->glfwWindow()))
    {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        myRenderer->drawContents();
        myRenderer->drawWidgets();
        myOpt.changed = true;
		if (myRenderer->shouldStep){
	        myOpt.step_saV2(mySurface, &temperature, a0);
		}
        myRenderer->uploadSurface();
        glfwSwapBuffers(myRenderer->glfwWindow());
        glfwPollEvents();
	}
	return 0;
}
