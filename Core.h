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

    Core(int core_id, int cache_size, int associativity, int block_size, Bus &main_bus, Bus &resp_bus,
         string program_name, string protocol);

    int fill_instruction_buffer();

    [[nodiscard]] int next_cycle();

    [[nodiscard]] int prRd(uint address);

    [[nodiscard]] int prWr(uint address);

    void cacheSnoop();

    int cacheSnoopResponse();

    void dumpCache();

    Cache l1_cache;

    [[nodiscard]] float getCacheMissRate() const;

    [[nodiscard]] long long int getCacheMiss() const;

    [[nodiscard]] long long int getCacheHit() const;

private:
    int core_number;  //To load the correct file
    string program{"bodytrack"};
    string protocol{"mesi"};
    bool blocked{false};
    int cycles_to_wait{0};
    bool work_done{false};
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
