/*
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

#include "MTRand.h"
#include "BRKGA.h"


class SampleDecoder {
public:
    SampleDecoder(){
    }
    ~SampleDecoder()
    {
    }
    
    void decode(const std::vector< double >& chromosome) const {
        std::vector< std::pair< double, unsigned > > ranking(chromosome.size());
        
        for(unsigned i = 0; i < chromosome.size(); ++i) {
            ranking[i] = std::pair< double, unsigned >(chromosome[i], i);
        }
        
        // Here we sort 'permutation', which will then produce a permutation of [n] in pair::second:
        std::sort(ranking.begin(), ranking.end());
        
        // permutation[i].second is in {0, ..., n - 1}; a permutation can be obtained as follows
        std::list< unsigned > permutation;
        for(std::vector< std::pair< double, unsigned > >::const_iterator i = ranking.begin();
            i != ranking.end(); ++i) {
            permutation.push_back(i->second);
        }
    }
private:
};


int main(int argc, char* argv[]) {
    const unsigned n = 100;		// size of chromosomes
    const unsigned p = 1000;	// size of population
    const double pe = 0.20;		// fraction of population to be the elite-set
    const double pm = 0.10;		// fraction of population to be replaced by mutants
    const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
    const unsigned K = 3;		// number of independent populations
    const unsigned MAXT = 1;	// number of threads for parallel decoding
    
    SampleDecoder decoder;			// initialize the decoder
    
    const long unsigned rngSeed = 0;	// seed to the random number generator
    MTRand rng(rngSeed);				// initialize the random number generator
    
    // initialize the BRKGA-based heuristic
    BRKGA< SampleDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
    
    unsigned generation = 0;		// current generation
    const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
    const unsigned X_NUMBER = 2;	// exchange top 2 best
    const unsigned MAX_GENS = 100;	// run for 100 gens
    printf("Aqui o caralho\n");
    do {
        printf("Aqui o caralho %d\n", generation);
        
        algorithm.evolve();	// evolve the population for one generation
        
        if((++generation) % X_INTVL == 0) {
            algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
        }
    } while (generation < MAX_GENS);
    
    std::cout << "Best solution found has objective value = "
	 		<< algorithm.getBestFitness() << std::endl;
    
    return 0;
} */
