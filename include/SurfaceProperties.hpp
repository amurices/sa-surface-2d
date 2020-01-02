#pragma once

#include <map>
#include <functional>
#include <GraphSurface.hpp>

namespace SurfaceProperties{
    extern std::map<std::string, std::function<double(const Graph::ThickSurface&)>> fns;
}


