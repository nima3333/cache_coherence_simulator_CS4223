//
// Created by nima on 04/11/2020.
//

#include "BusMessage.h"

BusMessage::BusMessage(MessageType type, int senderid) {
    this->type = type;
    this->senderId = senderid;
}

BusMessage::BusMessage() {
    this->type = Void;
    this->senderId = -1;
}