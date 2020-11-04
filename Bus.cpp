//
// Created by nima on 04/11/2020.
//

#include "Bus.h"

bool Bus::isEmpty() {
    return false;
}

void Bus::setMessage(BusMessage message) {
    this->message = message;
}

BusMessage Bus::getMessage() {
    return this->message;
}

void Bus::clearBus() {

}

Bus::Bus() {
}
