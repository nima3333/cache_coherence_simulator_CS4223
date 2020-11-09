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
            return -1;
        }
        BusMessage transaction = BusMessage(BusRd, this->attached_core, address);
        main_bus.setMessage(transaction);
        //TODO: doc for better readability
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

            int state = (int)getCacheBlockState(address).to_ulong();
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
            return timeConstants::main_memory_fetch;  //Get from main memory
        }
        else{  //Transition to Share
            BusMessage response = response_bus.getMessage(); //Stub data transfer between cache
            changeCacheBlockState(current_address, 2);
            return timeConstants::cache_to_cache; //Get from other cache
        }

    }
    //For write operations
    else if(current_instruction==1){
        if(response_bus.isEmpty()){  // fetch from main memory
            return timeConstants::main_memory_fetch;  //Get from main memory
        }
        else{  //fetch from cache
            return timeConstants::cache_to_cache; //Get from other cache
        }

        return 0;
    }
    else{
        //Something's wrong
        throw invalid_argument("load addr pb 2");
        return 0;
    }
}

int Cache::writeAddress(uint address) {
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);

    //Check if exists in cache
    if(cache_content[index].find(tag) != cache_content[index].end()){  //Present
        //Cache hit ? Check state
        State block_state = getCacheBlockState(address);

        //Check state
        if(block_state == 1){  //Exclusive block, just switch to Modified
            //No bus transaction
            changeCacheBlockState(address, 3);
            return timeConstants::cache_hit;
        }
        else if(block_state == 3){  //Modified block, nothing to do
            return timeConstants::cache_hit;
        }
        else if(block_state == 2){  //Shared block
            if(!main_bus.isEmpty()){  //Bus occupied, cannot proceeds
                return -1;
            }
            BusMessage transaction = BusMessage(BusUpgr, this->attached_core, address);
            main_bus.setMessage(transaction);
            changeCacheBlockState(address, 3);
            return timeConstants::cache_hit;
        }
        else if(block_state==0){  //Invalid block
            if(!main_bus.isEmpty()){  //Bus occupied, cannot proceeds
                return -1;
            }
            //Put BusRdX, next steps depend of the other caches
            BusMessage transaction = BusMessage(BusRdX, this->attached_core, address);
            main_bus.setMessage(transaction);
            changeCacheBlockState(address, 3);
            return -2;
        }
    }
    else{ //Not present
        if(!main_bus.isEmpty()){  //Bus occupied, cannot proceeds
            return -1;
        }
        //Put BusRdX, next steps depend of the other caches
        BusMessage transaction = BusMessage(BusRdX, this->attached_core, address);
        main_bus.setMessage(transaction);
        addBlock(address, 3);
        return -2;
    }
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

CacheBlock& Cache::getCacheBlock(uint address) {
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);

    bool found = false;
    for (auto & it : cache[index]){
        uint iter_tag = it.tag;
        if(iter_tag == tag){
            return it;
        }
    }
    throw invalid_argument("problem");
}

int Cache::getCacheBlockTag(uint address) {
    CacheBlock &hit = getCacheBlock(address);
    return hit.tag;
}

State Cache::getCacheBlockState(uint address) {
    CacheBlock &hit = getCacheBlock(address);
    return hit.state;
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
        //TODO: during deletion, if has a dirty bit, must be evicted into main memory
        int tag_to_delete = cache[index].front().tag;
        State state_to_delete = cache[index].front().state;
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
    for (CacheBlock & it : cache[index]){
        CacheBlock &hit = it;
        if(hit.tag == tag){
            hit.changeState(state);
            found = true;
        }
    }
    if(!found){
        addBlock(address, state);
    }
    return 0;

}

void Cache::dump() {
    int i,j;
    i=j=0;
    cout << "-----------" << endl << "* Cache dump " << this->attached_core << endl;
    for (list<CacheBlock> & it : cache){
        for (CacheBlock & it2 : it){
            cout << "j : " << j << "  " << it2.tag << " " << it2.state.to_ulong() << endl;
        }
        j++;
    }
    cout << "end" << endl;

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