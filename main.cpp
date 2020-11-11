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

	int nb_cores = 2;
	CoreCluster cl = CoreCluster(nb_cores);
    int j;

    do{
        j = cl.nextCycle();
        cl.cacheSnoop();
        cl.cacheSnoopResponse();
        cl.clearBuses();
    } while(j != nb_cores);
    cl.debug();
    return 0;
}
