//
// Created by nima on 04/11/2020.
//

#include "Core.h"

#include <utility>

Core::Core(int core_id, int cache_size, int associativity, int block_size, Bus &main_bus, Bus &resp_bus, string program_name, string protocol)
        :  main_bus(main_bus), response_bus(resp_bus), program(std::move(program_name)), protocol(std::move(protocol)),
        l1_cache(Cache(cache_size, associativity, block_size, main_bus, resp_bus, core_id, protocol)){
    this->core_number = core_id;
    fill_instruction_buffer();
}


int Core::fill_instruction_buffer() {
    //Get the proper file name, let's assume we do not use C++20
    //string file_name = format("/file/{}_{}.data", program, core_number);    C++20 way
    string filename = string("../files/") + program + string("_") + to_string(core_number) + string(".data");

    //Loading the file into a vector
    ifstream myfile(filename);
    if (myfile.is_open()) {
        string line;
        while (getline(myfile, line)) {
            string line_1 = line.substr(0, line.find(" "));
            int operation = atoi(line_1.c_str());

            string line_2 = line.substr(line.find(" "), line.length());
            long argument = stoul(line_2, nullptr, 16);

            instruction_buffer.push(make_pair(operation, argument));
        }
        myfile.close();
        cout << "Core "<< core_number << " loading achieved" << endl;
        return 1;
    } else {
        cout << "Core "<< core_number << " loading error" << endl;
        return 0;
    }
}

int Core::next_cycle() {
    if (instruction_buffer.empty() && !blocked) {
        return 1;
    }
    if (!blocked) {
        Operation current_operation = instruction_buffer.front();

        int current_instruction = current_operation.first;
        uint current_address = current_operation.second;
        this->blocked = true;
        int done;
        switch (current_instruction) {
            case 0:
                done = prRd(current_address);
                this->load_counter++;
                break;
            case 1:
                done = prWr(current_address);
                this->store_counter++;
                break;
            case 2:
                this->cycles_to_wait += current_address;
                done = 1;
                this->computing_counter++;
                break;
        }
        if (done) {
            instruction_buffer.pop();
        }
    } else {
        this->cycles_to_wait--;
        this->blocked = (this->cycles_to_wait != 0);
        this->idle_counter++;
    }

    return 0;
}

int Core::prRd(uint address) {
    int cache_waiting_cycles = this->l1_cache.loadAddress(address);
    if (cache_waiting_cycles == -1) {
        //Retry later
        this->blocked = false;
        return 0;
    } else if (cache_waiting_cycles == -2) {
        //Snooping of response required to determine next state
        this->snoopingPhaseRequired = true;
        return 0;
    } else {
        this->cycles_to_wait += cache_waiting_cycles;
        return 1;
    }
    return 0;
}

int Core::prWr(uint address) {
    int cache_waiting_cycles = this->l1_cache.writeAddress(address);
    if (cache_waiting_cycles == -1) {
        //Retry later
        this->blocked = false;
        return 0;
    } else if (cache_waiting_cycles == -2) {
        //Snooping of response required to determine next state
        this->snoopingPhaseRequired = true;
        return 0;
    } else {
        this->cycles_to_wait += cache_waiting_cycles;
        return 1;
    }
}

void Core::cacheSnoop() {
    int return_value = l1_cache.snoopMainBus();
    if(return_value>0){
        this->blocked = true;
        this->cycles_to_wait += return_value;
    }
}

int Core::cacheSnoopResponse() {
    //Return value is 1 if snooping the response bus was done, not necessary
    if (!snoopingPhaseRequired) {
        return 0;
    }
    Operation current_operation = instruction_buffer.front();

    int current_instruction = current_operation.first;
    uint current_address = current_operation.second;

    int return_value = l1_cache.snoopResponseBus(current_instruction, current_address);
    this->cycles_to_wait += return_value;
    this->snoopingPhaseRequired = false;
    instruction_buffer.pop();
    return 1;
}

void Core::dumpCache() {
    l1_cache.dump();
}

float Core::getCacheMissRate() const {
    long long cache_miss = l1_cache.getCacheMissNumber();
    long long cache_hit = l1_cache.getCacheHitNumber();
    return float(cache_miss) / float(cache_miss + cache_hit);
}

long long Core::getCacheMiss() const {
    return l1_cache.getCacheMissNumber();
}

long long Core::getCacheHit() const {
    return l1_cache.getCacheHitNumber();
}

long long Core::getOverallExecCycles() const{
    return this->computing_counter + this->store_counter + this->load_counter +  this->idle_counter;
}

void Core::displayStatistics() {
    cout << this->core_number << " " << getOverallExecCycles() << " " << this->computing_counter << " " << this->load_counter << " " << this->store_counter << " " << getCacheMissRate() << " " << l1_cache.getPrivateData() << " " << l1_cache.getSharedData() << endl;
}