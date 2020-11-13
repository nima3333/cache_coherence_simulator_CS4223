//============================================================================
// Name        : multicore_coherency.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>

#include "Bus.h"
#include "Cache.h"
#include "Core.h"
#include "CoreCluster.h"

using namespace std;

int main(int argc, char* argv[]) {

    if(argc != 6 and argc != 3) {
        cout << "Usage : coherence <protocol> <input_file> (<cache_size> <associativity> <block_size>) ";
        return -1;
    }

    //Get args
    string protocol = argv[1];
    string input_file = argv[2];
    int cache_size, associativity, block_size;

    if(argc==6){  //if cache specs are supplied
        cache_size = atoi(argv[3]);
        associativity = atoi(argv[4]);
        block_size = atoi(argv[5]);
    }
    else{  //Default values
        cache_size = 4096; //in bytes
        associativity = 2;
        block_size = 32; //in bytes
    }

    int nb_cores = 4;

    CoreCluster cl = CoreCluster(nb_cores, input_file, cache_size, associativity, block_size);

    int j;
    do {
        j = cl.nextCycle();
        cl.cacheSnoop();
        cl.cacheSnoopResponse();
        cl.clearBuses();
    } while (j != nb_cores);

    cl.debug();

    return 0;
}
