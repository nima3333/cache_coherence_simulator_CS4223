//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_BUS_H
#define MULTICORE_BUS_H

#include "BusMessage.h"
#include <cstdlib>
#include <iostream>

using namespace std;

class Bus {
public:
    explicit Bus(int block_size);

    [[nodiscard]] bool isEmpty() const;

    void setMessage(BusMessage message);

    void setMessageIfEmpty(BusMessage message);

    BusMessage getMessage();

    void clearBus();

    void getStatisticsInvalidations() const;
    void getStatisticsDataTraffic() const;

private:
    BusMessage message;
    long long data_traffic{0};
    long long invalidations{0};
    long long updates{0};
    int block_size;

    long long int getInvalidationRequests() const;

};

#endif //MULTICORE_BUS_H
