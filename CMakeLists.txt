cmake_minimum_required(VERSION 3.14)
project(sa_surface_2d)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(nanogui)
include_directories(nanogui/include)
include_directories(nanogui/include/nanogui)
include_directories(nanogui/ext)
include_directories(nanogui/ext/eigen)
include_directories(nanogui/ext/nanovg/src)

add_executable(sa_surface_2d
        include/_2DSurface.hpp
        include/defs.hpp
        include/IO.hpp
        include/MathGeometry.hpp
        include/Optimizer.hpp
        include/Renderer.hpp
        include/ThickSurface.hpp
        include/Util.hpp
        src/_2DSurface.cpp
        src/IO.cpp
        src/main.cpp
        src/MathGeometry.cpp
        src/Optimizer.cpp
        src/Renderer.cpp
        src/ThickSurface.cpp
        src/Util.cpp src/GlobalState.cpp include/GlobalState.hpp)
find_package(OpenGL)
target_link_libraries(sa_surface_2d
        /usr/local/lib/libnanogui.dylib
        ${OPENGL_gl_LIBRARY})
