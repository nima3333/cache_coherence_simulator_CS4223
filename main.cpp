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
    Bus resp = Bus();
    BusMessage b = BusMessage();
    Core c = Core(0, a, resp);

    int i = c.next_cycle();
    c.cacheSnoop();
    c.cacheSnoopResponse();
    while(i!=1){
        i = c.next_cycle();
        c.cacheSnoop();
        c.cacheSnoopResponse();
        a.clearBus();
        resp.clearBus();
    }

    uint M = 5;
    uint N = 2;
    uint address = 0x817af8;
    uint tag = address >> (N+M);
    uint index = (address << (32-N-M)) >> (32-M);
    cout << dec << (N+M) << " " << hex << tag << " " << index;

    return 0;



}
