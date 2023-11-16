#ifndef ACK_PACKET_H
#define ACK_PACKET_H

#include <string>
#include "definitions.h"
#include "Packet.hpp"

#define ACK_PACKET_LENGTH_BYTES 4

class ACKPacket : public Packet {
    short int block_number;
    public:
        ACKPacket(short int block_number);
        ACKPacket(const char* data);
        short int getBlockNumber();
        void setBlockNumber(short int block_number);
        unsigned short getOpcode();
        string toByteStream();
        size_t getLength();
        size_t maxSizeBytes();
};

#endif