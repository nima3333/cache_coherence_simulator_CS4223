//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_CACHE_H
#define MULTICORE_CACHE_H

#include "Bus.h"
#include "CacheBlock.h"
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

class Cache {
public:
    //FIXME: passed by ref
    Cache(int c_size, int asso, int b_size, Bus main_bus, Bus reponse_bus, int attached_core);
    int loadAddress(uint address);
    int writeAddress(uint address);
    int snoopBus(uint address);

private:
    vector<list<CacheBlock>> cache;
    vector<unordered_set<uint>> cache_content;
    int cache_size;
    int associativity;
    int block_size;
    uint N;
    uint M;
    int nb_cache_blocs;
    int initialize_cache(int cache_size, int associativity, int block_size);
    Bus main_bus;
    Bus response_bus;

    int attached_core;

    int isInCache(uint address);

    int getCacheBlockTag(uint address);

    int putLastUsed(uint address);
};

#endif //MULTICORE_CACHE_H
