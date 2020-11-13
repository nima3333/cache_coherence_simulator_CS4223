//
// Created by nima on 11/11/2020.
//

#ifndef MULTICORE_CORECLUSTER_H
#define MULTICORE_CORECLUSTER_H


#include "Core.h"

class CoreCluster {
public:
    CoreCluster(int nb_cores, const string& program_name, int cache_size, int associativity, int block_size);

    int nextCycle();

    void cacheSnoop();

    void cacheSnoopResponse();

    void clearBuses();

    void debug();

private:
    int nb_cores{2};
    string program{"bodytrack"};
    vector<Core> cores;
    int block_size;
    int cache_size;
    int associativity;
    Bus main_bus;
    Bus response_bus;
};


#endif //MULTICORE_CORECLUSTER_H
