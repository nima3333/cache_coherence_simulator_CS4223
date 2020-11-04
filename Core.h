//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_CORE_H
#define MULTICORE_CORE_H


#include "Cache.h"



class Core {
public:
    Core(int core_id);
    int fill_instruction_buffer();
    int next_cycle();

private:
    int core_number;  //To load the correct file
    string program{"bodytrack"};
    int store_counter{0}; //Store the number of stores
    int load_counter{0};
    int computing_counter{0};
    queue<Operation> instruction_buffer;
};


#endif //MULTICORE_CORE_H
