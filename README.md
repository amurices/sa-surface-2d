# Introduction

This project is a renderer, using *[nanogui](https://github.com/wjakob/nanogui)*, of a particular Simulated Annealing algorithm that calculates the folding of the cerebellum (yes, the actual part of the brain) in 2 dimensions (which can lead to valid extrapolations since the cerebellum folds uniformly across dimensions) based on an energy function that is an estimate of [a surface's Free Energy](https://en.wikipedia.org/wiki/Surface_energy).

## Installation

### On Linux (tested on Ubuntu 18.04)
1. Install git: `sudo apt install git`
2. Install cmake: `sudo apt install cmake`
3. Install OpenGL headers: `sudo apt install libgl1-mesa-dev`
4. Install this package for some reason: `sudo apt install xorg-dev`
5. Clone recursively: `git clone https://github.com/amurices/sa-surface-2d.git --recursive`
6. cd into nanogui’s folder: `cd sa-surface-2d/nanogui`
7. make a build directory: `mkdir build`
8. change directory into it: `cd build`
9. use CMake to write build files to this directory by doing: `cmake ..` (in Unix, .. means “the directory above”. So if your current path is /usr/bin/local, doing cd .. will take you  to /usr/bin. CMake takes a directory as a parameter to find a CMakeLists.txt file, which in this case, is the one directly above it)
10. run `make` to compile the generated code
11. Go back to sa-surface-2d’s directory: `cd ../..`
12. Finally, do the same we did for nanogui; first create a build directory: `mkdir build`
13. cd into that folder: `cd build`
14. use CMake to write build files to it: `cmake ..`
15. finally, run `make`

There, just 14 steps lol. Now you can run the program by being in the build directory and just doing `./sa_surface_2d`

If `make` complains at the end, try replacing the `dylib` in CMakeLists.txt with `so`:
```cmake
${CMAKE_SOURCE_DIR}/nanogui/build/libnanogui.dylib -> ${CMAKE_SOURCE_DIR}/nanogui/build/libnanogui.so
```
This is due to the fact that building with cmake on Mac generates .dylib  by default, while Linux generates .so. Functionally they’re the same thing: libraries, which is just a collection of compiled code and symbols that map on to that code (which is why we have to include the nanogui headers; we have to tell our code which parts of the compiled nanogui library we’re going to use).

## Overview of the model

### Simulated Annealing
The model is based on *Two polygons with the same amount of sides*, one internal and one external. It follows the
Simulated Annealing algorithm by iteratively performing a step function, called `Optimizer::stepSimulatedAnnealing`. Before
describing what an iteration of this instance of the SA algorithm does, here are a few definitions:
* A ***surface*** is represented by a mere polygon. It is essentially a circular graph.
* A ***thick surface*** is represented by *two* surfaces and *one* ***thickness vector***. The latter is an array of `double`s, where
`thicknesses[n]` represents the distance between `outerSurfaceNodes[n]` and `innerSurfaceNodes[n]`; each node in a surface is therefore identified by a
number in the range `[0..amountOfPoints-1]`.

The function works as follows:

1. First, find a *neighbor* of the current *state*. Here, a state is just the current configuration of
the thick surface under optimization. A state's neighbor is defined by choosing a random node on the state's
*outer* surface and *altering it* in a random direction. More details on how this alteration can be performed later.
2. Find the *energy* of both the neighbor and the current *state*. The energy function is defined as follows (pseudocode):
```
let whiteMatter = area(innerSurface)
    grayMatter = area(outerSurface) - area(innerSurface)
    grayMatterStretch = absolute(g0 - grayMatter) // a0 is initial (relaxed) state's  gray matter area; Must be non-negative
in
   am * whiteMatter^ap + dm * (grayMatter + 1)^dp // where `am`, `ap`, `dm` and `dp` are *given* constants defined by an input file
```
3. Find the *probability* of moving into a new state. The probability function is defined as follows (actual C++ implementation):
```c++
double Optimizer::findProbability(double eS, double eN, double t){
    if (eN < eS)
        return 1;
    else
        return exp((eS - eN) / t); // t being 0 doesn't break this, interestingly
}
```
4. *If* the neighbor contains no intersections between the set of edges comprising the inner and outer polygons,
consider it the new state with the probability found in 3.

5. Currently, **don't** decrement temperature and just repeat. Decrementing temperature by just subtracting an
absolute value from it was giving weird results.

### Altering a node and optional parameters

As noted in the description above, altering a node is an operation that can be configured by an input file or
via the *nanogui*-powered front end GUI. The parameters are:
1. **Smoothness**; Arguably the most important parameter, it's more easily explained visually. The drawing
below showcases what happens with a randomly selected node in a given iteration.
![](resources/how-smoothness-works-in-sad.png)
A node's neighbors are pushed in the same direction it was, with decreasing intensity based on how far from it
they are. Smoothness is an integer number that determines how many nodes will be "smoothed"; so the example in the picture
shows a simulation with smoothness set to 2. lower smoothness values tend to lead to more "gyrified" but more jagged and "spiky" simulations.
2. **forceOffsetRange**; Defines the numerical range in the XY plane a node can be pushed in. Example: if set to
0.066, the maximum alteration to a node's coordinates is (0.033, 0.033) and the minimum (-0.033, -0.033).
3. **thickness**; the initial thickness of the surface. Every element in the `thicknesses` array described above is
set to this value in the beginning of the simulation. The number means *multiplication of radius*, so a thick surface with
thickness 1.0 has no inner surface area.
4. **points**; Number of nodes in the outer and inner polygons. Higher numbers mean smoother-looking simulations, but
also slower ones.
5. **diffPow**, **diffMul**, **areaPow** and **areaMul**; the variables described in the energy function's pseudocode above.
They are dimensionless and are all set to 1.0 by default.
6. **multiProb**; the probability, after selecting a random node, that another random node will be selected to be
altered *in the same iteration*. So if this is set to 0.5, the probability that 3 random nodes will be picked in the same
iteration to be altered and considered part of the same neighbor is 1 * 0.5 * 0.5 (1 first because at least one node is always selected). Set
to 0 by default.
7. **tempProb**; unused.
8. **Last but not least, compression**; An important parameter that describes how the surface stretches as it's pulled
and pushed. If a node is pulled away from its inner correspondent (the first node altered is always the outer one),
then its thickness is *multiplied* by this value. If it's pushed towards its inner correspondent, it is *divided* by this value. 