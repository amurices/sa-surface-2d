//
//  SurfaceGenome.hpp
//  Surface
//
//  Created by André Muricy on 16/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//
/*
#ifndef SurfaceGenome_hpp
#define SurfaceGenome_hpp

#include <iostream>
#include "Auxiliares.hpp"
#include <ga/ga.h>
#include <ga/GARealGenome.h>

float Objective(GAGenome &);

class SurfaceGenome : public GA1DArrayAlleleGenome<float> {
/* public:
    GADefineIdentity("SurfaceGenome", 203);
    static void Init(GAGenome&);
    static int  Mutate(GAGenome&, float);
    static float Compare (const GAGenome&, const GAGenome&);
    static float Evaluate(GAGenome&);
    static int Cross(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);
    
    
public:
    SurfaceGenome(): GAGenome(Init, Mutate, Compare){
        evaluator(Evaluate);
        crossover(Cross);
    }
    
    SurfaceGenome(const SurfaceGenome& orig) { copy (orig); }
    virtual ~SurfaceGenome() {}
    SurfaceGenome& operator=(const GAGenome& orig){
        if (&orig != this) copy (orig);
        return *this;
    }
    virtual GAGenome* clone(CloneMethod) const {return new SurfaceGenome(*this);}
    virtual void copy (const GAGenome&orig){
        GAGenome::copy(orig);
        
        // todo: copy outros atributos
    }
    */
public:
    surface* initialSurface;
    SurfaceGenome(unsigned int l, const GAAlleleSet<float> alleleset, GAGenome::Evaluator objective) : GA1DArrayAlleleGenome<float>(l, alleleset, objective)
    {
        
    };
    
    void setInitialSurface(surface* surf)
    {
        initialSurface = surf;
        std::cout << "Got a pointer to surf. Vertex ids: " << std::endl;
        for (size_t i = 0; i < initialSurface->v.size(); i++)
        {
            std::cout << initialSurface->v[i].pid << " ";
        }
        std::cout << std::endl;
    }
    
    surface* getInitialSurface() { return initialSurface; }
 
 /*
    void
    SurfaceGenome::Init(GAGenome&){
    // your initializer here
    }
 
    int
    SurfaceGenome::Mutate(GAGenome&, float){
    // your mutator here
    }
 
    float
    SurfaceGenome::Compare(const GAGenome&, const GAGenome&){
    // your comparison here
    }
 
    float
    SurfaceGenome::Evaluate(GAGenome&){
    // your evaluation here
    }
 
    int
    SurfaceGenome::Cross(const GAGenome& mom, const GAGenome& dad,
    GAGenome* sis, GAGenome* bro){
    // your crossover here
 }
 */
};

#endif /* SurfaceGenome_hpp */


/*public:
GADefineIdentity("MyGenome", 201);
static void Init(GAGenome&);
static int Mutate(GAGenome&, float);
static float Compare(const GAGenome&, const GAGenome&);
static float Evaluate(GAGenome&);
static int Cross(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);

public:
MyGenome() : GAGenome(Init, Mutate, Compare) {
    evaluator(Evaluate);
    crossover(Cross);
}
MyGenome(const MyGenome& orig) { copy(orig); }
virtual ~MyGenome() {}
MyGenome& operator=(const GAGenome& orig){
    if(&orig != this) copy(orig);
    return *this;
}

virtual GAGenome* clone(CloneMethod) const {return new MyGenome(*this);}
virtual void copy(const GAGenome& orig) {
    GAGenome::copy(orig);  // this copies all of the base genome parts
    // copy any parts of MyObject here
    // copy any parts of MyGenome here
}

// any data/member functions specific to this new class
};*/
