//
// Created by nima on 04/11/2020.
//

#include "Cache.h"

#include <utility>

//TODO: verify write back / write allocate ++++
//TODO: verify cache block number, etc  +++++

Cache::Cache(int cache_size, int associativity, int block_size, Bus &main_bus, Bus &response_bus_arg, int attached_core, string protocol)
        : main_bus(main_bus), response_bus(response_bus_arg), protocol(std::move(protocol)) {
    this->cache_size = cache_size;
    this->associativity = associativity;
    this->block_size = block_size;
    this->attached_core = attached_core;
    this->nb_cache_blocs = cache_size / (block_size * associativity);
    initialize_cache(cache_size, associativity, block_size);
    this->N = (int) ceil(log2(block_size / 4));
    this->M = (int) ceil(log2(cache_size / (block_size * associativity)));
    cout << "Protocol : " << this->protocol << " " << "N, M = " << N << " " << M << endl;
}

int Cache::initialize_cache(int cache_size, int associativity, int block_size) {
    nb_cache_blocs = cache_size / (block_size * associativity);
    for (int i = 0; i < nb_cache_blocs; i++) {
        cache.emplace_back();
        cache_content.emplace_back();
    }
    return 0;
}

//TODO: in each function, "if mesi / else dragon ..."
int Cache::loadAddress(uint address) {
    //Tested with one value
    uint tag = address >> (N + M);
    uint index = (address << (32 - N - M)) >> (32 - M);
    //Check if exists in cache
    if (cache_content[index].find(tag) != cache_content[index].end()) {  //Present
        putLastUsed(address);
        //Cache hit ? Check state
        State block_state = getCacheBlockState(address);
        //According to protocols ... FIXME: ugly
        if (this->protocol == protocolNames::mesi){
            if (block_state == 0) {  //Invalid block, will have to get it
                //Bus transaction
                if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
                    return -1;
                }
                //Register cache miss
                cache_miss++;

                BusMessage transaction = BusMessage(BusRd, this->attached_core, address);
                main_bus.setMessage(transaction);
                return -2;
            } else {  //Cache Hit
                //Register cache Hit
                cache_hit++;

                //Apply LRU rule
                putLastUsed(address);
                //Number of cycles to wait
                return timeConstants::cache_hit;
            }
        }
        else if (this->protocol==protocolNames::moesi){
            if (block_state == 0) {  //Invalid block, will have to get it
                //Bus transaction
                if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
                    return -1;
                }
                //Register cache miss
                cache_miss++;

                BusMessage transaction = BusMessage(BusRd, this->attached_core, address);
                main_bus.setMessage(transaction);
                return -2;
            } else {  //Cache Hit
                //Register cache Hit
                cache_hit++;

                //Apply LRU rule
                putLastUsed(address);
                //Number of cycles to wait
                return timeConstants::cache_hit;
            }
        }
    } else { //Not present
        //Bus transaction
        if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
            return -1;
        }
        //Register cache miss
        cache_miss++;
        BusMessage transaction = BusMessage(BusRd, this->attached_core, address);
        main_bus.setMessage(transaction);
        //TODO: doc for better readability
        return -2;
    }
    return 0;
}

int Cache::snoopMainBus() {
    BusMessage on_bus = main_bus.getMessage();
    if (on_bus.senderId == attached_core) {  //it's his own message, no reaction
        return 0;
    } else {  // Take into account the message
        MessageType message_type = on_bus.type;
        uint address = on_bus.address;
        //Check if address is in cache
        int inCache = isInCache(address);
        if (inCache) {
            //Get state
            int state = (int) getCacheBlockState(address).to_ulong();

            //According to protocols ... FIXME: ugly
            if(this->protocol == protocolNames::mesi){
                switch (state) {
                    case 0: //If cache is invalid, do nothing
                        return 0;
                    case 1: //If cache in Exclusive

                        if (message_type == BusRd) {
                            BusMessage response = BusMessage(FlushOpt, attached_core, address);
                            response_bus.setMessageIfEmpty(response);
                            changeCacheBlockState(address, 2); //Transition to Shared
                        } else if (message_type == BusRdX) {
                            BusMessage response = BusMessage(FlushOpt, attached_core, address);
                            response_bus.setMessageIfEmpty(response);
                            changeCacheBlockState(address, 0); //Transition to Invalid
                        }
                        /* Not necessary, only in share
                        else if(message_type==BusUpgr){
                            changeCacheBlockState(address, 0); //Transition to Invalid
                        }
                         */
                        break;
                    case 2: //If cache in Shared
                        if (message_type == BusRd) {
                            BusMessage response = BusMessage(FlushOpt, attached_core, address);
                            response_bus.setMessageIfEmpty(response);

                        } else if (message_type == BusRdX) {
                            BusMessage response = BusMessage(FlushOpt, attached_core, address);
                            response_bus.setMessageIfEmpty(response);

                            changeCacheBlockState(address, 0); //Transition to Invalid
                        } else if (message_type == BusUpgr) {
                            changeCacheBlockState(address, 0); //Transition to Invalid
                        }
                        break;
                    case 3: //If cache in Modified
                        if (message_type == BusRd) {
                            BusMessage response = BusMessage(FlushOpt, attached_core, address);
                            response_bus.setMessageIfEmpty(response);
                            changeCacheBlockState(address, 2); //Transition to Shared
                            //In MESI State, written back to main memory and to cache
                            return timeConstants::main_memory_fetch;

                        } else if (message_type == BusRdX) {
                            BusMessage response = BusMessage(FlushOpt, attached_core, address);
                            response_bus.setMessageIfEmpty(response);
                            changeCacheBlockState(address, 0); //Transition to Invalid
                            //In MESI State, written back to main memory and to cache
                            return timeConstants::main_memory_fetch;
                        }
                        /* Not necessary, only in share
                        else if(message_type==BusUpgr){
                            changeCacheBlockState(address, 0); //Transition to Invalid
                        }
                         */
                        break;
                }

            }


        } else {;}
    }

    return 0;
}

int Cache::snoopResponseBus(int current_instruction, uint current_address) {

    //According to protocols ... FIXME: ugly
    if(this->protocol==protocolNames::mesi){
        //For read operations
        if (current_instruction == 0) {
            if (response_bus.isEmpty()) { //Transition to Exclusive
                int extra_time = changeCacheBlockState(current_address, 1);
                return timeConstants::main_memory_fetch + extra_time;  //Get from main memory
            } else {  //Transition to Share
                BusMessage response = response_bus.getMessage(); //Stub data transfer between cache
                int extra_time = changeCacheBlockState(current_address, 2);
                return this->block_size * timeConstants::cache_to_cache + extra_time; //Get from other cache
            }

        }
            //For write operations
        else if (current_instruction == 1) {
            int extra_time = changeCacheBlockState(current_address, 3);
            if (response_bus.isEmpty()) {  // fetch from main memory
                return timeConstants::main_memory_fetch + extra_time;  //Get from main memory
            } else {  //fetch from cache
                return this->block_size * timeConstants::cache_to_cache + extra_time; //Get from other cache
            }
        } else {
            //Something's wrong
            throw invalid_argument("load addr pb 2");
        }
    }
    throw invalid_argument("load addr pb 2");
}

int Cache::writeAddress(uint address) {
    uint tag = address >> (N + M);
    uint index = (address << (32 - N - M)) >> (32 - M);

    //Check if exists in cache
    if (cache_content[index].find(tag) != cache_content[index].end()) {  //Present
        putLastUsed(address);
        //Cache hit ? Check state
        State block_state = getCacheBlockState(address);
        //According to protocols ... FIXME: ugly
        if(this->protocol == protocolNames::mesi) {
            //Check state
            if (block_state == 1) {  //Exclusive block, just switch to Modified
                //Register cache Hit
                cache_hit++;
                //No bus transaction
                int extra_time = changeCacheBlockState(address,
                                                       3);  //Extratime = 0 always here, no eviction in changing state
                if (extra_time != 0) throw invalid_argument("extra time should be zero");
                return timeConstants::cache_hit;
            } else if (block_state == 3) {  //Modified block, nothing to do
                //Register cache Hit
                cache_hit++;
                return timeConstants::cache_hit;
            } else if (block_state == 2) {  //Shared block
                if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
                    return -1;
                }
                //Register cache Hit
                cache_hit++;
                BusMessage transaction = BusMessage(BusUpgr, this->attached_core, address);
                main_bus.setMessage(transaction);
                int extra_time = changeCacheBlockState(address, 3);
                if (extra_time != 0) throw invalid_argument("extra time should be zero");
                return timeConstants::cache_hit;
            } else if (block_state == 0) {  //Invalid block
                if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
                    return -1;
                }
                //Register cache miss
                cache_miss++;
                //Put BusRdX, next steps depend of the other caches
                BusMessage transaction = BusMessage(BusRdX, this->attached_core, address);
                main_bus.setMessage(transaction);
                return -2;
            }
        }
        else if (this->protocol == protocolNames::moesi) {
            //Check state
            if (block_state == 1) {  //Exclusive block, just switch to Modified
                //Register cache Hit
                cache_hit++;
                //No bus transaction
                int extra_time = changeCacheBlockState(address, 3);  //Extratime = 0 always here, no eviction in changing state
                if (extra_time != 0) throw invalid_argument("extra time should be zero");
                return timeConstants::cache_hit;

            } else if (block_state == 2) {  //Shared block
                //With MOESI, we can switch to the Owned state and modify it
                if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
                    return -1;
                }
                //Register cache Hit
                cache_hit++;

                BusMessage transaction = BusMessage(BusUpdate, this->attached_core, address);
                main_bus.setMessage(transaction);
                int extra_time = changeCacheBlockState(address, 4);
                if (extra_time != 0) throw invalid_argument("extra time should be zero");

                return timeConstants::cache_hit;

            } else if (block_state == 3) {  //Modified block, nothing to do
                //Register cache Hit
                cache_hit++;

                return timeConstants::cache_hit;

            } else if (block_state == 4) {  //Owned block, specificity of the MOESI protocol
                //Have to transfer to other cache blocks that are in share state
                if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
                    return -1;
                }
                BusMessage transaction = BusMessage(BusUpdate, this->attached_core, address);
                main_bus.setMessage(transaction);

                //Register cache Hit
                cache_hit++;

                return timeConstants::cache_hit;

            } else if (block_state == 0) {  //Invalid block
                if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
                    return -1;
                }
                //Register cache miss
                cache_miss++;
                //Put BusRdX, next steps depend of the other caches
                BusMessage transaction = BusMessage(BusRdX, this->attached_core, address);
                main_bus.setMessage(transaction);
                return -2;
            }

        }
    } else { //Not present
        if (!main_bus.isEmpty()) {  //Bus occupied, cannot proceeds
            return -1;
        }
        //Register cache miss
        cache_miss++;
        //Put BusRdX, next steps depend of the other caches
        BusMessage transaction = BusMessage(BusRdX, this->attached_core, address);
        main_bus.setMessage(transaction);
        return -2;
    }
    return 0;
}

int Cache::isInCache(uint address) {
    //Tested with one value
    uint tag = address >> (N + M);
    uint index = (address << (32 - N - M)) >> (32 - M);

    //Check if exists in cache
    if (cache_content[index].find(tag) != cache_content[index].end()) {  //Present
        return 1;
    } else {
        return 0;
    }
}

CacheBlock &Cache::getCacheBlock(uint address) {
    uint tag = address >> (N + M);
    uint index = (address << (32 - N - M)) >> (32 - M);

    bool found = false;
    for (auto &it : cache[index]) {
        uint iter_tag = it.tag;
        if (iter_tag == tag) {
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

int Cache::putLastUsed(uint address) {
    uint tag = address >> (N + M);
    uint index = (address << (32 - N - M)) >> (32 - M);

    //Find corresponding cache block
    bool found = false;
    CacheBlock hit;
    for (list<CacheBlock>::iterator it = cache[index].begin(); it != cache[index].end(); ++it) {
        uint iter_tag = it->tag;
        if (iter_tag == tag) {
            hit = *it;
            found = true;
        }
    }
    if (!found) throw invalid_argument("load addr pb 3");

    //Delete
    cache[index].remove(hit);

    //Re-Insertion
    cache[index].push_back(hit);

    return 0;
}

int Cache::addBlock(uint address, State state) {
    uint tag = address >> (N + M);
    uint index = (address << (32 - N - M)) >> (32 - M);
    bool block_eviction = false;
    if (cache[index].size() == associativity) {
        int tag_to_delete = cache[index].front().tag;
        State state_to_delete = cache[index].front().state;
        cache[index].pop_front();
        cache_content[index].erase(tag_to_delete);
        //if modified, eviction needed
        block_eviction = (state_to_delete == 3);
    }
    cache[index].emplace_back(state, tag);
    cache_content[index].emplace(tag);
    //Statistics on Shared/Private lines
    if (state == 2 or state == 1) {  //if block in Exclusive or Share, shared data
        shared_data++;
    } else if (state == 3) { //If block in Modified, private data
        private_data++;
    }
    return timeConstants::eviction * block_eviction;
}

int Cache::changeCacheBlockState(uint address, State state) {
    uint tag = address >> (N + M);
    uint index = (address << (32 - N - M)) >> (32 - M);
    //Statistics on Shared/Private lines
    if (state == 2 or state == 1) {  //if block in Exclusive or Share, shared data
        shared_data++;
    } else if (state == 3) { //If block in Modified, private data
        private_data++;
    }
    //Find corresponding cache block
    bool found = false;
    for (CacheBlock &it : cache[index]) {
        CacheBlock &hit = it;
        if (hit.tag == tag) {
            hit.changeState(state);
            found = true;
        }
    }
    if (!found) {
        int to_add = addBlock(address, state);
        return to_add;
    } else {
        return 0;
    }
}

void Cache::dump() {
    int i, j;
    i = j = 0;
    cout << "-----------" << endl << "* Cache dump " << this->attached_core << endl;
    for (list<CacheBlock> &it : cache) {
        for (CacheBlock &it2 : it) {
            cout << "j : " << j << "  " << it2.tag << " " << it2.state.to_ulong() << endl;
        }
        j++;
    }
    cout << "end" << endl;
}

long long Cache::getCacheMissNumber() const {
    return cache_miss;
}

long long Cache::getCacheHitNumber() const {
    return cache_hit;
}