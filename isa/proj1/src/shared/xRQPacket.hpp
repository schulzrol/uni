#ifndef xRQPACKET_H
#define xRQPACKET_H

#include "definitions.h"
#include "Packet.hpp"
#include <string>

using namespace std;

class xRQPacket : public Packet {
    string filename;
    string mode;
    unsigned short opcode;
    public:
        xRQPacket(unsigned short opcode, string filename, string mode);
        xRQPacket(unsigned short opcode, const char* data);
        const char* getFilename();
        const char* getMode();
        void setFilename(const char* filename);
        void setMode(const char* mode);
        char* toByteStream();
        unsigned short getOpcode();
        size_t getLength();
        static size_t maxSizeBytes();
};

#endif