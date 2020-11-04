//
// Created by nima on 04/11/2020.
//

#ifndef MULTICORE_BUSMESSAGE_H
#define MULTICORE_BUSMESSAGE_H

enum MessageType {
    Void = 0,
    BusRd = 1,
    BudRdX = 2,
    FlushOpt = 3,
    BusUpgr = 4,
    Flush = 5
};

class BusMessage {
public:
    BusMessage(MessageType type, int senderid);

    BusMessage();

private:
    MessageType type{Void};
    int senderId{};
};


#endif //MULTICORE_BUSMESSAGE_H
