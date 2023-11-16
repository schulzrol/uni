#ifndef PACKET_FACTORY_H
#define PACKET_FACTORY_H

#include "definitions.h"
#include "Packet.hpp"
#include "RRQPacket.hpp"
#include "WRQPacket.hpp"
#include "DATAPacket.hpp"
#include "ACKPacket.hpp"
#include "ERRORPacket.hpp"
#include <string>
#include <exception>

using namespace std;

class PacketFactory {
    public:
        static Packet* createPacket(const char* data, size_t data_length, size_t block_size);
};

#endif
