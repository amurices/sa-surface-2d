# Introduction

This project is a renderer, using *[nanogui](https://github.com/wjakob/nanogui)*, of a particular Simulated Annealing algorithm that calculates the folding of the cerebellum (yes, the actual part of the brain) in 2 dimensions (which can lead to valid extrapolations since the cerebellum folds uniformly across dimensions) based on an energy function that is an estimate of [a surface's Free Energy](https://en.wikipedia.org/wiki/Surface_energy).

## Installation

First, clone this repository with `git clone --recursive`. This will automatically include any git submodules it depends on.

Most of the packages that need to be installed can be obtained via [Homebrew](https://brew.sh/) on Mac OS (Xcode command line tools obviously need to be installed as well). Out of the following packages, the only one that is not more easily installed with Homebrew's `brew` command is LEMON:

_Packages_:
* [LEMON](https://lemon.cs.elte.hu/trac/lemon/wiki/InstallLinux) (depends on [CMake](https://cmake.org/))
* [GLFW](https://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [FTGL](http://ftgl.sourceforge.net/docs/html/ftgl-tutorial.html)

_Libraries_:
* [CGAL](https://www.cgal.org/) # For the way faster (n lg n) sweep-line algorithm for intersection detection
* [nanogui](https://github.com/wjakob/nanogui) # For cross-platform GUIs rendered with OpenGL (QT would be overkill)

### Note on Nanogui
Nanogui is included as a submodule. This means that cloning this repository recursively will also obtain nanogui's code, but it still has to be built from source. Thankfully, the process is straightforward. The steps to build nanogui are as follows, after cloning this repo:
1. `cd nanogui`
2. `mkdir build` (this is normally standard for CMake-based projects and libraries)
3. `cd build`
4. `cmake ..`
5. `make`

After that, change directories back to the repo's root with `cd ../..` and run `make` and that should be enough to produce an executable.
