#ifndef PACKET_H
#define PACKET_H

#include <string>
#include "definitions.h"
#include <exception>

using namespace std;

class Packet {
    public:
        virtual unsigned short getOpcode();
        virtual char* toByteStream();
};

#endif