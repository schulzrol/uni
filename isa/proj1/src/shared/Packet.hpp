#ifndef PACKET_H
#define PACKET_H

#include <string>
#include "definitions.h"
#include <exception>

using namespace std;

class Packet {
    public:
        virtual ~Packet();
        virtual unsigned short getOpcode();
        virtual string toByteStream();
        virtual size_t getLength();
        size_t maxSizeBytes();
};

#endif