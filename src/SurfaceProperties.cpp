#include <SurfaceProperties.hpp>
#include <MathGeometry.hpp>
#include <Optimizer.hpp>

namespace SurfaceProperties {
    namespace {
        double whiteMatter(const Graph::ThickSurface &thickSurface){
            return Graph::surfaceArea(thickSurface.layers[Graph::INNER]);
        }
        double grayMatter(const Graph::ThickSurface &thickSurface){
            return MathGeometry::absol(
                    Graph::surfaceArea(thickSurface.layers[Graph::OUTER]) -
                    Graph::surfaceArea(thickSurface.layers[Graph::INNER])
            );
        }
        double perimeter(const Graph::ThickSurface &thickSurface){
            return 0.0;
        }
        double energy(const Graph::ThickSurface &thickSurface){
            return Optimizer::findEnergy();
        }
    }
    std::map<std::string, std::function<double(const Graph::ThickSurface&)>> fns =
            {{"whiteMatter", whiteMatter},
             {"grayMatter", grayMatter},
             {"perimeter", perimeter},
             {"energy", energy}};
}
