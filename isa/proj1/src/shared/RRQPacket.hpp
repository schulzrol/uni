#ifndef RRQPACKET_H
#define RRQPACKET_H

#include "definitions.h"
#include "xRQPacket.hpp"
#include <string>

using namespace std;

class RRQPacket : public xRQPacket {
    public:
        RRQPacket(string filename, string mode, map<string, string> options = {});
        RRQPacket(string filename, tftp_mode mode, map<string, string> options = {});
        RRQPacket(const char* data, size_t length);
};

#endif