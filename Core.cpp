//
// Created by nima on 04/11/2020.
//

#include "Core.h"


Core::Core(int core_id, Bus &main_bus, Bus &resp_bus)
:l1_cache(Cache(1024, 2, 16, main_bus, resp_bus, core_id)), main_bus(main_bus), response_bus(resp_bus)
{
    this->core_number = core_id;

    fill_instruction_buffer();
}

int Core::fill_instruction_buffer()
{
    //Get the proper file name, let's assume we do not use C++20
    //string file_name = format("/file/{}_{}.data", program, core_number);    C++20 way
    string filename = string("../files/") + program + string("_") + to_string(core_number) + string(".data");

    //Loading the file into a vector
    ifstream myfile(filename);
    cout << filename << endl;
    if (myfile.is_open())
    {
        cout << "Loading core" << endl;
        string line;
        while ( getline (myfile,line) )
        {
            string line_1= line.substr(0, line.find(" "));
            int operation = atoi(line_1.c_str());

            string line_2= line.substr(line.find(" "), line.length());
            long argument = stoul(line_2, nullptr, 16);

            instruction_buffer.push(make_pair(operation, argument));
        }
        myfile.close();
        cout << "Loading achieved" << endl;
        return 1;
    }
    else{
        cout << "Unable to open file";
        return 0;
    }
}

int Core::next_cycle() {
    if(instruction_buffer.empty() && !blocked){
        return 1;
    }
    if(!blocked){
        Operation current_operation = instruction_buffer.front();

        int current_instruction = current_operation.first;
        uint current_address = current_operation.second;
        this->blocked = true;
        int done;
        switch(current_instruction){
            case 0:
                done = prRd(current_address);
                break;
            case 1:
                done = prWr(current_address);
                break;
            case 2:
                this->cycles_to_wait += current_address;
                done = 1;
                break;
        }
        if(done){
            instruction_buffer.pop();
        }
    }
    else{
        this->cycles_to_wait --;
        this->blocked = (this->cycles_to_wait != 0);
    }

    return 0;
}

int Core::prRd(uint address) {
    int cache_waiting_cycles = this->l1_cache.loadAddress(address);
    if(cache_waiting_cycles == -1){
        //Retry later
        this->blocked=false;
        return 0;
    }
    else if(cache_waiting_cycles == -2){
        //Snooping of response required to determine next state
        this->snoopingPhaseRequired = true;
        return 0;
    }
    else{
        this->cycles_to_wait += cache_waiting_cycles;
        return 1;
    }
    return 0;
}

int Core::prWr(uint address) {
    int cache_waiting_cycles = this->l1_cache.writeAddress(address);
    if(cache_waiting_cycles == -1){
        //Retry later
        this->blocked=false;
        return 0;
    }
    else if(cache_waiting_cycles == -2){
        //Snooping of response required to determine next state
        this->snoopingPhaseRequired = true;
        return 0;
    }
    else{
        this->cycles_to_wait += cache_waiting_cycles;
        return 1;
    }

    return 0;
}

int Core::cacheSnoop() {
    int return_value = l1_cache.snoopMainBus();
    return 0;
}

int Core::cacheSnoopResponse() {
    if(!snoopingPhaseRequired){
        return 0;
    }
    Operation current_operation = instruction_buffer.front();

    int current_instruction = current_operation.first;
    uint current_address = current_operation.second;

    int return_value = l1_cache.snoopResponseBus(current_instruction, current_address);
    this->cycles_to_wait += return_value;
    this->snoopingPhaseRequired=false;
    instruction_buffer.pop();
    return 1;
}

void Core::dumpCache(){
    l1_cache.dump();
}