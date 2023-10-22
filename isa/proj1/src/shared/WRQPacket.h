#ifndef WRQPACKET_H
#define WRQPACKET_H

#include "RRQPacket.h"

class WRQPacket : public RRQPacket {
    public:
        WRQPacket(string filename, string mode);
        WRQPacket(const char* data);
};

#endif