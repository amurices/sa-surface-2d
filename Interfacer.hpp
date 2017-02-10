//
//  Interfacer.hpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef Interfacer_hpp
#define Interfacer_hpp

#include <stdio.h>
#include "defs.h"
#include "Auxiliares.hpp"
class Interfacer{
public:
    Interfacer();
    static surface generate_random(double perim, int pts = 500);
    surface get_from_matlab();
};

#endif /* Interfacer_hpp */
