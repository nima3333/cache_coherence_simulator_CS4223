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

int main() {
    cout << "Hello World!!!" << endl; // prints !!!Hello World!!!

    //Get protocol
    string protocol = _argv[1];

    int nb_cores = 4;
    CoreCluster cl = CoreCluster(4, "bodytrack", 1024, 2, 16);
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
