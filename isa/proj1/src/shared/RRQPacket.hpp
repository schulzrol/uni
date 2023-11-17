#ifndef RRQPACKET_H
#define RRQPACKET_H

#include "definitions.h"
#include "xRQPacket.hpp"
#include <string>

using namespace std;

class RRQPacket : public xRQPacket {
    public:
        RRQPacket(string filename, string mode);
        RRQPacket(string filename, tftp_mode mode);
        RRQPacket(const char* data);
};

#endif