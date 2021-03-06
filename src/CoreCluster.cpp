//
// Created by nima on 11/11/2020.
//

#include "CoreCluster.h"

CoreCluster::CoreCluster(int nb_cores, const string& protocol, const string& program_name, int cache_size, int associativity, int block_size)
        : nb_cores(nb_cores), block_size(block_size), cache_size(cache_size), associativity(associativity),
          main_bus(Bus(block_size)), response_bus(Bus(block_size)), protocol(protocol) {
    for (int i = 0; i < nb_cores; i++) {
        cores.emplace_back(i, cache_size, associativity, block_size, this->main_bus, this->response_bus, program_name, protocol);
    }
}

int CoreCluster::nextCycle() {
    int return_value = 0;
    for (int i = 0; i < nb_cores; i++) {
        int result = cores[i].next_cycle();
        return_value += result;
    }
    return return_value;
}

void CoreCluster::cacheSnoop() {
    for (int i = 0; i < nb_cores; i++) {
        cores[i].cacheSnoop();
    }
};

void CoreCluster::cacheSnoopResponse() {
    for (int i = 0; i < nb_cores; i++) {
        cores[i].cacheSnoopResponse();
    }
};

void CoreCluster::clearBuses() {
    main_bus.clearBus();
    response_bus.clearBus();
}

void CoreCluster::debug() {
    cores[0].dumpCache();
    cores[1].dumpCache();
    cout << cores[0].getCacheMissRate() << "  " << cores[0].getCacheMiss() << "  " << cores[0].getCacheHit() << endl;

}

void CoreCluster::displayStatistics(string protocol){
    long long exec1 = 0;
    for (int i = 0; i < nb_cores; i++) {
        exec1 = max(cores[i].getOverallExecCycles(), exec1);
    }
    cout << "Overall execution cycles = " << exec1 << endl;
    cout << "Core Execution Compute Load Store MissRate Private Shared" << endl;
    for (int i = 0; i < nb_cores; i++) {
        cores[i].displayStatistics();
    }
    main_bus.getStatisticsInvalidationsUpdates(protocol);
    response_bus.getStatisticsDataTraffic();
}
