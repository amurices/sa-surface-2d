//
//  SurfaceDecoder.hpp
//  Surface
//
//  Created by André Muricy on 1/4/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#ifndef SurfaceDecoder_hpp
#define SurfaceDecoder_hpp

#include <iostream>
#include <vector>
#include <list>
#include "defs.h"

class SurfaceDecoder {
public:
    SurfaceDecoder(){
    }
    ~SurfaceDecoder()
    {
    }
    
    
    
    void decode(const std::vector< double >& chromosome) const;

private:
};


#endif /* SurfaceDecoder_hpp */
