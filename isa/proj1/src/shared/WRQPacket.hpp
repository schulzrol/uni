#ifndef WRQPACKET_H
#define WRQPACKET_H

#include "xRQPacket.hpp"

class WRQPacket : public xRQPacket {
    public:
        WRQPacket(string filename, string mode, map<string, string> options = {});
        WRQPacket(string filename, tftp_mode mode, map<string, string> options = {});
        WRQPacket(const char* data, size_t length);
};

#endif