//
//  SurfaceDecoder.cpp
//  Surface
//
//  Created by André Muricy on 1/4/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "SurfaceDecoder.hpp"


double SurfaceDecoder::decode(const std::vector< double >& chromosome) const {
    ThickSurface_t thickGen;
    SurfaceData_t outerGen, innerGen, bridges;
    std::vector<point_t> inters;
    copy_surface(org->outer, thickGen.outer);
    int count = 0;
    for (ListDigraph::NodeIt no(thickGen.outer.graph); no != INVALID; ++no)
    {
     //   std::cout << "from org: " << (*org->outer.coords)[org->outer.graph.nodeFromId(count)] << std::endl;
     //   std::cout << "from thickGen: " << (*thickGen.outer.coords)[thickGen.outer.graph.nodeFromId(count)] << std::endl;

        (*thickGen.outer.coords)[no].x += chromosome[count];
        (*thickGen.outer.coords)[no].y += chromosome[count + chromosome.size() * 0.5];
        count++;
    }
    // OTIMIZAÇÃO A SE FAZER AQUI
    // Os offsets podem ser adicionados e logo em seguida utilizados p/ calcular a inner
    Interfacer::generate_inner_s(thickGen.inner, thickGen.outer, org->thickness);
    
    Interfacer::generate_bridges(thickGen);
    
    double perim;
    double res = calculate_surface_area(thickGen.outer, perim);
    
    std::vector<SurfaceData_t*> surfaces;
    surfaces.push_back(&thickGen.outer);
    surfaces.push_back(&thickGen.inner);
    
    int intsOuter = find_surface_intersections(surfaces, inters);
    res += 100000 * intsOuter;
    return res;
}
