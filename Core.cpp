//
// Created by nima on 04/11/2020.
//

#include "Core.h"
#include <iostream>
#include <fstream>


Core::Core(int core_id) {
    //TODO: logic to handle failure
    this->core_number = core_id;
    fill_instruction_buffer();
    Cache cachee(1024, 2, 4);
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
    return 0;
}
