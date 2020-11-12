//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_CORE_H
#define MULTICORE_CORE_H

#include "Bus.h"
#include "Cache.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>

using namespace std;

typedef unsigned int uint;
typedef pair<int, uint> Operation;


class Core {
public:
    Core(int core_id, Bus &main_bus, Bus &resp_bus);

    Core(int core_id, Bus &main_bus, Bus &resp_bus, string program);

    int fill_instruction_buffer();

    int next_cycle();

    int prRd(uint address);

    int prWr(uint address);

    int cacheSnoop();

    int cacheSnoopResponse();

    void dumpCache();

    Cache l1_cache;

    float getCacheMissRate() const;

    long long int getCacheMiss() const;

    long long int getCacheHit() const;

private:
    int core_number;  //To load the correct file
    string program{"bodytrack"};
    bool blocked{0};
    int cycles_to_wait{0};
    bool work_done{0};
    Bus &main_bus;
    Bus &response_bus;
    queue<Operation> instruction_buffer;
    bool snoopingPhaseRequired{false};
    //Statistics
    long long store_counter{0};
    long long load_counter{0};
    long long computing_counter{0};
    long long idle_counter{0};

};


#endif //MULTICORE_CORE_H
