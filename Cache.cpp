//
// Created by nima on 04/11/2020.
//

#include "Cache.h"

Cache::Cache(int cache_size, int associativity, int block_size, Bus &main_bus, Bus &response_bus_arg, int attached_core)
:main_bus(main_bus), response_bus(response_bus_arg)
{
    this->cache_size = cache_size;
    this->associativity = associativity;
    this->block_size = block_size;
    this->attached_core = attached_core;
    initialize_cache(cache_size, associativity, block_size);
    this->N = (int)ceil(log2(block_size/4));
    this->M = (int)ceil(log2(cache_size/(block_size*associativity)));
    printf("N = %d, M = %d", N, M);
}

int Cache::initialize_cache(int cache_size, int associativity, int block_size) {
    nb_cache_blocs = cache_size / (block_size * associativity);
    for(int i=0; i<nb_cache_blocs; i++){
        cache.emplace_back();
        cache_content.emplace_back();
    }
    CacheBlock a = CacheBlock(1, 4);
    cache[1].push_back(a);
    return 0;
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
        //TO_TEST
        //for (list<CacheBlock>::iterator it=cache[index].begin(); it != cache[index].end(); ++it){
        for (auto & it : cache[index]){
            uint iter_tag = it.tag;
            if(iter_tag == tag){
                hit = it;
                found = true;
            }
        }
        //TODO: to put in a function
        if(!found) throw invalid_argument("load addr pb 1");

        //Check state
        if(hit.state == 0){  //Invalid block, will have to get it
            //TODO: get it, factorize
            //Bus transaction
            if(!main_bus.isEmpty()){  //Bus occupied, cannot proceeds
                //TODO: handle error (-1) in Core
                return -1;
            }
            BusMessage transaction = BusMessage(BusRd, this->attached_core, address);
            main_bus.setMessage(transaction);

        }
        else{  //Cache Hit
            //Apply LRU rule
            putLastUsed(address);
            //Number of cycles to wait
            return timeConstants::cache_hit;
        }
    }
    else{ //Not present
        //Bus transaction
        if(!main_bus.isEmpty()){  //Bus occupied, cannot proceeds
            printf("bus not empty");
            //TODO: handle error (-1) in Core
            return -1;
        }
        BusMessage transaction = BusMessage(BusRd, this->attached_core, address);
        main_bus.setMessage(transaction);
        //FIXME: -2 is "waiting for snoop result"
        return -2;
    }
    return 0;
}

int Cache::snoopMainBus() {
    BusMessage on_bus = main_bus.getMessage();
    if(on_bus.senderId == attached_core){  //it's his own message, no reaction
        return 0;
    }
    else{  // Take into account the message
        MessageType message_type = on_bus.type;
        uint address = on_bus.address;
        //Check if address is in cache
        int inCache = isInCache(address);
        if(inCache){
            //Get state
            int state = getCacheBlockTag(address);
            switch (state) {
                case 0: //If cache is invalid, do nothing
                    return 0;
                case 1: //If cache in Exclusive
                    if(message_type==BusRd){
                        BusMessage response = BusMessage(FlushOpt, attached_core, address);
                        response_bus.setMessageIfEmpty(response);
                        changeCacheBlockState(address, 2); //Transition to Shared
                    }
                    else if(message_type==BusRdX){
                        BusMessage response = BusMessage(FlushOpt, attached_core, address);
                        response_bus.setMessageIfEmpty(response);

                        changeCacheBlockState(address, 0); //Transition to Invalid
                    }
                    break;
                case 2: //If cache in Shared
                    if(message_type==BusRd){
                        BusMessage response = BusMessage(FlushOpt, attached_core, address);
                        response_bus.setMessageIfEmpty(response);

                    }
                    else if(message_type==BusRdX){
                        BusMessage response = BusMessage(FlushOpt, attached_core, address);
                        response_bus.setMessageIfEmpty(response);

                        changeCacheBlockState(address, 0); //Transition to Invalid
                    }
                    break;
                case 3: //If cache in Modified
                    if(message_type==BusRd){
                        BusMessage response = BusMessage(FlushOpt, attached_core, address);
                        response_bus.setMessageIfEmpty(response);
                        changeCacheBlockState(address, 2); //Transition to Shared
                    }
                    else if(message_type==BusRdX){
                        BusMessage response = BusMessage(FlushOpt, attached_core, address);
                        response_bus.setMessageIfEmpty(response);
                        changeCacheBlockState(address, 0); //Transition to Invalid
                    }
                    break;
            }
        }
        else { ;
        }
    }

    return 0;
}

int Cache::snoopResponseBus(int current_instruction, uint current_address){

    //For read operations
    if(current_instruction==0){
        if(response_bus.isEmpty()){ //Transition to Exclusive
            changeCacheBlockState(current_address, 1);
            //TODO: put value in a file
            return timeConstants::main_memory_fetch;  //Get from main memory
        }
        else{  //Transition to Share
            BusMessage response = response_bus.getMessage(); //Stub data transfer between cache
            changeCacheBlockState(current_address, 2);
            //TODO: put value in a file
            return timeConstants::cache_to_cache; //Get from other cache
        }

    }
    //TODO: For write operations
    else if(current_instruction==1){
        return 0;
    }
    else{
        //Something's wrong
        throw invalid_argument("load addr pb 2");
        return 0;
    }
}

int Cache::writeAddress(uint address) {
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
    if(!found) throw invalid_argument("load addr pb 3");

    //Delete
    cache[index].remove(hit);

    //Re-Insertion
    cache[index].push_back(hit);

    return 0;
}

int Cache::addBlock(uint address, State state){
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);
    if(cache[index].size()==associativity){
        int tag_to_delete = cache[index].front().tag;
        cache[index].pop_front();
        cache_content[index].erase(tag_to_delete);
    }
    cache[index].emplace_back(state, tag);
    cache_content[index].emplace(tag);
    return 0;
}

int Cache::changeCacheBlockState(uint address, State state){
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
    if(!found){
        addBlock(address, state);
    }
    else{
        hit.changeState(state);
    }
    return 0;

}
/*
 * //TODO: put this elsewhere, after every core reported for the transaction
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
 */