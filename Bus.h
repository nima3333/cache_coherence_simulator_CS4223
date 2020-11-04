//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_BUS_H
#define MULTICORE_BUS_H

#include "BusMessage.h"

class Bus {
public:
    Bus();
    bool isEmpty();
    void setMessage(BusMessage message);
    BusMessage getMessage();
    void clearBus();

private:
    BusMessage message;
};

#endif //MULTICORE_BUS_H
