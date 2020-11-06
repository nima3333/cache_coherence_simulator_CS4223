//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_CACHE_H
#define MULTICORE_CACHE_H

#include "Bus.h"
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <bitset>
#include <list>
#include <unordered_set>
#include <algorithm>

using namespace std;


typedef unsigned int uint;

enum instruction {Load=0, Store=1, Computation=2};
typedef pair<int, uint> Operation;
typedef bitset<2> State;

struct cache_block {
    State state;
    uint tag{};
};


class Cache {
public:
    Cache(int c_size, int asso, int b_size, Bus main_bus);
    int loadAddress(uint address);
    int writeAddress(uint address);
    int snoopBus();

private:
    vector<list<cache_block>> cache;
    vector<unordered_set<uint>> cache_content;
    int cache_size;
    int associativity;
    int block_size;
    uint N;
    uint M;
    int nb_cache_blocs;
    int initialize_cache(int cache_size, int associativity, int block_size);
    Bus main_bus{Bus()};
};

#endif //MULTICORE_CACHE_H
