//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_BUS_H
#define MULTICORE_BUS_H

#include "BusMessage.h"
#include <cstdlib>

class Bus {
public:
    Bus(int block_size);

    bool isEmpty();

    void setMessage(BusMessage message);

    void setMessageIfEmpty(BusMessage message);

    BusMessage getMessage();

    void clearBus();

private:
    BusMessage message;
    long long data_traffic{0};
    long long invalidations{0};
    long long updates{0};
    int block_size;
};

#endif //MULTICORE_BUS_H
