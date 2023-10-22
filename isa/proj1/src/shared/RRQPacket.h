#ifndef RRQPACKET_H
#define RRQPACKET_H

#include "definitions.h"
#include "Packet.h"
#include <string>

using namespace std;

class RRQPacket : public Packet {
    string filename;
    string mode;
    public:
        RRQPacket(string filename, string mode);
        RRQPacket(const char* data);
        const char* getFilename();
        const char* getMode();
        void setFilename(const char* filename);
        void setMode(const char* mode);
        char* toByteStream();
        unsigned short getOpcode();
        size_t getLength();
};

#endif