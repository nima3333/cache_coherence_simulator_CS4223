//
// Created by nima on 04/11/2020.
//

#include "Cache.h"

Cache::Cache(int cache_size, int associativity, int block_size, Bus main_bus, Bus response_bus, int attached_core)
{
    this->cache_size = cache_size;
    this->associativity = associativity;
    this->block_size = block_size;
    this->main_bus = main_bus;
    this->response_bus = response_bus;
    this->attached_core = attached_core;
    initialize_cache(cache_size, associativity, block_size);
    this->N = (int)ceil(log2(block_size/4));
    this->M = (int)ceil(log2(cache_size/(block_size*associativity)));
    printf("N = %d, M = %d", N, M);
}

int Cache::loadAddress(uint address) {
    //Tested with one value
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);

    //Check if exists in cache
    if(cache_content[index].find(tag) != cache_content[index].end()){  //Present
        //Cache hit ? Check state

        //Get cache block
        bool found = false;
        CacheBlock hit;
        for (list<CacheBlock>::iterator it=cache[index].begin(); it != cache[index].end(); ++it){
            uint iter_tag = it->tag;
            if(iter_tag == tag){
                hit = *it;
                found = true;
            }
        }
        //TODO: to put in a function
        if(!found) throw "problem";

        //Check state
        if(hit.state == 0){  //Invalid block, will have to get it
            //TODO: get it, factorize

        }
        else{  //Cache Hit
            //Apply LRU rule
            putLastUsed(address);
            //Number of cycles to wait TODO: define it in a file
            return 1;
        }
    }
    else{ //Not present
        //Bus transaction
        if(!main_bus.isEmpty()){  //Bus occupied, cannot proceeds
            //TODO: handle error (-1) in Core
            return -1;
        }
        BusMessage transaction = BusMessage(BusRd, this->attached_core);
        main_bus.setMessage(transaction);
        //FIXME: -2 is "waiting for snoop result"
        return -2;
    }
    return 0;
}

int Cache::snoopBus(uint address) {
    BusMessage on_bus = main_bus.getMessage();
    if(on_bus.senderId == attached_core){  //it's his own message, no reaction
        return 0;
    }
    else{  // Take into account the message
        //TODO: Need to take into account the message
        MessageType message_type = on_bus.type;
        //Check if address is in cache
        int inCache = isInCache(on_bus.address);
        if(inCache){
            //Get state
            int state = getCacheBlockTag(on_bus.address);
            switch (state) {
                case 0: //If cache is invalid, do nothing
                    return 0;
                case 1: //If cache in Exclusive
                    if(message_type==BusRd){
                        BusMessage response = BusMessage(FlushOpt, attached_core, on_bus.address);
                        response_bus.setMessage(response);
                    }
                    else if(message_type==BudRdX){

                    }
                    break;
                case 2: //If cache in Shared
                    if(message_type==BusRd){

                    }
                    else if(message_type==BudRdX){

                    }
                    break;
                case 3: //If cache in Modified
                    if(message_type==BusRd){

                    }
                    else if(message_type==BudRdX){

                    }
                    break;
            }
        }
        else{
            return 0;
        }
    }

    return 0;
}


int Cache::writeAddress(uint address) {
    return 0;
}

int Cache::initialize_cache(int cache_size, int associativity, int block_size) {
    nb_cache_blocs = cache_size / (block_size * associativity);
    for(int i=0; i<nb_cache_blocs; i++){
        cache.push_back(list<CacheBlock>());
        cache_content.push_back(unordered_set<uint>());
    }
    CacheBlock a = CacheBlock(1, 4);
    cache[1].push_back(a);
    return 0;
}

int Cache::isInCache(uint address) {
    //Tested with one value
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);

    //Check if exists in cache
    if(cache_content[index].find(tag) != cache_content[index].end()) {  //Present
        return 1;
    }
    else{
        return 0;
    }
}

int Cache::getCacheBlockTag(uint address) {
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);

    bool found = false;
    CacheBlock hit;
    for (list<CacheBlock>::iterator it=cache[index].begin(); it != cache[index].end(); ++it){
        uint iter_tag = it->tag;
        if(iter_tag == tag){
            hit = *it;
            found = true;
        }
    }
    return hit.tag;
}

int Cache::putLastUsed(uint address){
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);

    //Find corresponding cache block
    bool found = false;
    CacheBlock hit;
    for (list<CacheBlock>::iterator it=cache[index].begin(); it != cache[index].end(); ++it){
        uint iter_tag = it->tag;
        if(iter_tag == tag){
            hit = *it;
            found = true;
        }
    }

    //Delete
    //FIXME: does not work
    cache[index].remove(hit);

    //Re-Insertion
    cache[index].push_back(hit);

    return 0;
}

/*
int Cache::snoopreact(){
    //TODO: put this elsewhere, after every core reported for the transaction
    if(cache_content[index].size() < associativity){ //Cache is not full
        //TODO: just add in cache
    }
    else{  //Cache is full
        //TODO: LRU replacement
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
    return 0;
}
 */