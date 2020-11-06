//
// Created by nima on 04/11/2020.
//

#include "Core.h"


Core::Core(int core_id, Bus main_bus) {
    //TODO: logic to handle failure
    this->main_bus = main_bus;
    this->core_number = core_id;
    fill_instruction_buffer();
}

int Core::fill_instruction_buffer() {
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
    if(!blocked){
        Operation current_operation = instruction_buffer.front();
        instruction_buffer.pop();

        int current_instruction = current_operation.first;
        uint current_address = current_operation.second;

        switch(current_instruction){
            case 0:
                prRd(current_address);
                break;
            case 1:
                prWr(current_address);
                break;
            case 2:
                this->cycles_to_wait += current_address;
                this->blocked = true;
                break;
        }

    }
    else{
        this->cycles_to_wait --;
        this->blocked = (this->cycles_to_wait != 0);
    }

    return 0;
}

int Core::prRd(uint address) {
    this->l1_cache.loadAddress(address);
    return 0;
}

int Core::prWr(uint address) {
    this->l1_cache.writeAddress(address);
    return 0;
}
