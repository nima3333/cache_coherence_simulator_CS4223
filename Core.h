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
    Core(int core_id, Bus main_bus);
    int fill_instruction_buffer();
    int next_cycle();
    int prRd(uint address);
    int prWr(uint address);
    int cacheSnoop();
    int cacheSnoopResponse();

private:
    int core_number;  //To load the correct file
    string program{"test"};
    int store_counter{0}; //Store the number of stores
    int load_counter{0};
    int computing_counter{0};
    bool blocked{0};
    int cycles_to_wait{0};
    bool work_done{0};
    Bus main_bus{Bus()};
    Bus response_bus{Bus()};
    //FIXME: check that, nothing properly passed
    Cache l1_cache{Cache(1024, 2, 16, main_bus, response_bus, this->core_number)};
    queue<Operation> instruction_buffer;
    bool snoopingPhaseRequired;

};


#endif //MULTICORE_CORE_H
