//
// Created by nima on 11/11/2020.
//

#ifndef MULTICORE_CORECLUSTER_H
#define MULTICORE_CORECLUSTER_H


#include "Core.h"

class CoreCluster {
public:
    explicit CoreCluster(int nb_cores);
    CoreCluster(int nb_cores, string program_name);
    int nextCycle();
    void cacheSnoop();
    void cacheSnoopResponse();
    void clearBuses();
    void debug();
private:
    int nb_cores{2};
    string program{"bodytrack"};
    vector<Core> cores;
    Bus main_bus{Bus()};
    Bus response_bus{Bus()};
};


#endif //MULTICORE_CORECLUSTER_H
