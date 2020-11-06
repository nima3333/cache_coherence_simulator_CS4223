//============================================================================
// Name        : multicore_coherency.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>

#include "Bus.h"
#include "Cache.h"
#include "Core.h"

using namespace std;

int main() {
	cout << "Hello World!!!" << endl; // prints !!!Hello World!!!
    Bus a = Bus();
    BusMessage b = BusMessage();
    Core c = Core(1, a);

    uint M = 5;
    uint N = 2;
    uint address = 0x817af8;
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);
    cout << dec << (N+M) << " " << hex << tag << " " << index;
    return 0;

}
