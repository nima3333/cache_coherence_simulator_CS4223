//
// Created by nima on 04/11/2020.
//

#include "Bus.h"
#include "constants.h"

bool Bus::isEmpty() const {
    return this->message.type == Void;
}

void Bus::setMessage(BusMessage message_to_set) {
    this->message = message_to_set;

    //For response bus, data traffic only here
    if (message.type == FlushOpt) {
        data_traffic += block_size;
    }

    //For main bus, BusRdx is an invalidation
    if (message.type == BusRdX or message.type == BusUpgr) {
        invalidations++;
    }

    if (message.type == BusUpdate) {
        updates = timeConstants::cache_to_cache * (block_size / 4);
        data_traffic += updates;
    }
}

BusMessage Bus::getMessage() {
    return this->message;
}

void Bus::clearBus() {
    this->message = BusMessage();
}

Bus::Bus(int block_size) : block_size(block_size) {
}

void Bus::setMessageIfEmpty(BusMessage message_to_set) {
    if (isEmpty()) {
        setMessage(message_to_set);
    }
}

void Bus::getStatisticsInvalidations() const{
    cout << "Bus invalid: " << invalidations << endl;
}

void Bus::getStatisticsDataTraffic() const{
    cout << "Bus traffic: " << data_traffic << endl;
}

void Bus::getStatisticsUpdates() const{
    cout << "Bus updates: " << updates << endl;
}