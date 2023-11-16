#ifndef WRQPACKET_H
#define WRQPACKET_H

#include "xRQPacket.hpp"

class WRQPacket : public xRQPacket {
    public:
        WRQPacket(string filename, string mode);
        WRQPacket(const char* data);
};

#endif