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
    int fill_instruction_buffer();
    int next_cycle();
    int prRd(uint address);
    int prWr(uint address);
    int cacheSnoop();
    int cacheSnoopResponse();
    void dumpCache();
    Cache l1_cache;
private:
    int core_number;  //To load the correct file
    string program{"test"};
    bool blocked{0};
    int cycles_to_wait{0};
    bool work_done{0};
    Bus &main_bus;
    Bus &response_bus;
    queue<Operation> instruction_buffer;
    bool snoopingPhaseRequired{false};
    //Statistics
    int store_counter{0};
    int load_counter{0};
    int computing_counter{0};
    int idle_counter{0};


};


#endif //MULTICORE_CORE_H
