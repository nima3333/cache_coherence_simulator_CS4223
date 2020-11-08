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
    return 0;



}
