//
// Created by nima on 04/11/2020.
//

#include "Cache.h"

Cache::Cache(int c_size, int asso, int b_size) {
    this->cache_size = c_size;
    this->associativity = asso;
    this->block_size = b_size;
    initialize_cache(cache_size, associativity, block_size);
    N = (int)ceil(log2(block_size/4));
    M = (int)ceil(log2(cache_size/(block_size*associativity)));
    printf("N = %d, M = %d", N, M);
}

int Cache::loadAddress(uint address) {
    //TODO: check ??
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);
    //Check if exists in cache
    if(cache_content[index].find(tag) != cache_content[index].end()){  //Present
        //TODO: state ?
    }
    else{ //Not present
        if(cache_content[index].size() < associativity){ //Cache is not full

        }
        else{  //Cache is full
            //TODO: cache coherence
            //Delete front one
            cache_block to_remove = cache[index].front();
            cache[index].pop_front();
            cache_content[index].erase(to_remove.tag);

            //Insertion
            cache_content[index].insert(tag);
            cache_block to_add;
            to_add.tag = tag;
            to_add.state = 0;
            cache[index].push_back(to_add);
        }
    }
    return 0;
}

int Cache::writeAddress(long address) {
    return 0;
}

int Cache::initialize_cache(int cache_size, int associativity, int block_size) {
    nb_cache_blocs = cache_size / (block_size * associativity);
    for(int i=0; i<nb_cache_blocs; i++){
        cache.push_back(list<cache_block>());
        cache_content.push_back(unordered_set<uint>());
    }
    cache_block a;
    a.state=1;
    a.tag=4;
    cache[1].push_back(a);
    return 0;
}
