# Introduction

This repository contains the "transitional code" from a version that readily compiles in Visual Studio 2017 on Windows 10, to a portable version that also compiles on Unix platforms given minor adjustments.

## Installation

Most of the packages that need to be installed can be obtained via [Homebrew](https://brew.sh/) on Mac OS (Xcode command line tools obviously need to be installed as well). Out of the following packages, the only one that is not more easily installed with Homebrew's `brew` command is LEMON:

_Packages_:
* [LEMON](https://lemon.cs.elte.hu/trac/lemon/wiki/InstallLinux) (depends on [CMake](https://cmake.org/))
* [GLFW](https://www.glfw.org/)
* [GLEW](http://glew.sourceforge.net/)
* [FTGL](http://ftgl.sourceforge.net/docs/html/ftgl-tutorial.html)

_Libraries_:
* [CGAL](https://www.cgal.org/) # For the way faster (n lg n) sweep-line algorithm for intersection detection
* [nanogui](https://github.com/wjakob/nanogui) # For cross-platform GUIs rendered with OpenGL (QT would be overkill)

OpenGL (version string `2.1 INTEL-10.36.19`) should come with Mac OS.

After that, just running `make surface` should be enough to produce an executable.
