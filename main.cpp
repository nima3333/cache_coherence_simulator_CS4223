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
    Core c1 = Core(0, a, resp);
    Core c2 = Core(1, a, resp);

    int i1, i2;

    do{
        i1 = c1.next_cycle();
        i2 = c2.next_cycle();
        c1.cacheSnoop();
        c2.cacheSnoop();
        c1.cacheSnoopResponse();
        c2.cacheSnoopResponse();
        a.clearBus();
        resp.clearBus();
    } while(i1!=1 or i2!=1);

    c1.dumpCache();
    c2.dumpCache();


    return 0;



}
