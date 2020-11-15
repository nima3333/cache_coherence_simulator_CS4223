//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_CACHE_H
#define MULTICORE_CACHE_H

#include "Bus.h"
#include "CacheBlock.h"
#include "constants.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <bitset>
#include <list>
#include <unordered_set>
#include <algorithm>

using namespace std;
using namespace timeConstants;

typedef unsigned int uint;

enum instruction {
    Load = 0, Store = 1, Computation = 2
};
typedef pair<int, uint> Operation;
typedef bitset<3> State;


class Cache{
public:
    Cache(int c_size, int asso, int b_size, Bus &main_bus, Bus &reponse_bus, int attached_core, string protocol);

    [[nodiscard]] int loadAddress(uint address);

    [[nodiscard]] int writeAddress(uint address);

    [[nodiscard]] int snoopMainBus();

    [[nodiscard]] int snoopResponseBus(int current_instruction, uint current_address);

    //Debug tool
    void dump();

    //Statistics
    [[nodiscard]] long long getCacheMissNumber() const;

    [[nodiscard]] long long getCacheHitNumber() const;

private:
    vector<list<CacheBlock>> cache;
    vector<unordered_set<uint>> cache_content;
    vector<uint> shared_line;
    int cache_size;
    int associativity;
    int block_size;
    uint N;
    uint M;
    int nb_cache_blocs;
    string protocol{"mesi"};
    int initialize_cache(int cache_size, int associativity, int block_size);

    Bus &main_bus;
    Bus &response_bus;
    int attached_core;

    int isInCache(uint address);

    int getCacheBlockTag(uint address);

    int putLastUsed(uint address);

    int changeCacheBlockState(uint address, State state);

    int addBlock(uint address, State state);

    void sharedLineRemove(uint address);

    int sharedLineAssertion(uint address);

    State getCacheBlockState(uint address);

    CacheBlock &getCacheBlock(uint address);

    //Statistics
    long long cache_miss{0};
    long long cache_hit{0};
    long long private_data{0};
    long long shared_data{0};
public:
    long long int getPrivateData() const;

    long long int getSharedData() const;

    int block_size_words;
};

#endif //MULTICORE_CACHE_H
