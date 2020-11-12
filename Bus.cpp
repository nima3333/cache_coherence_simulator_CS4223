//
// Created by nima on 04/11/2020.
//

#include "Bus.h"

bool Bus::isEmpty() {
    return this->message.type==Void;
}

void Bus::setMessage(BusMessage message) {
    this->message = message;

    //For response bus, data traffic only here
    if(message.type == FlushOpt){
        data_traffic += block_size;
    }

    //For main bus, BusRdx is an invalidation
    if(message.type == BusRdX){
        invalidations ++;
    }

    //TODO: updates for Dragon
}

BusMessage Bus::getMessage() {
    return this->message;
}

void Bus::clearBus() {
    this->message = BusMessage();
}

Bus::Bus(int block_size):block_size(block_size) {
}

void Bus::setMessageIfEmpty(BusMessage message) {
    if(isEmpty()){
        this->message = message;
    }
}
