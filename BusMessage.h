//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_BUSMESSAGE_H
#define MULTICORE_BUSMESSAGE_H

typedef unsigned int uint;

enum MessageType {
    Void = 0,
    BusRd = 1,
    BusRdX = 2,
    FlushOpt = 3,
    BusUpgr = 4,
    Flush = 5
};

class BusMessage {
public:
    BusMessage(MessageType type, int senderid);
    BusMessage(MessageType type, int senderid, uint address);

    BusMessage();

    int senderId{};
    MessageType type{Void};
    uint address{0};
};


#endif //MULTICORE_BUSMESSAGE_H
