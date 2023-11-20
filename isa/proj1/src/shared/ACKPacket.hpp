#ifndef ACK_PACKET_H
#define ACK_PACKET_H

#include <string>
#include "definitions.h"
#include "Packet.hpp"
#include "DATAPacket.hpp"

#define ACK_PACKET_LENGTH_BYTES 4

class ACKPacket : public Packet {
    unsigned short block_number;
    public:
        ACKPacket(unsigned short block_number);
        ACKPacket(const char* data);
        ACKPacket(DATAPacket data_packet);
        short int getBlockNumber();
        void setBlockNumber(short int block_number);
        unsigned short getOpcode();
        string toByteStream();
        size_t getLength();
        size_t maxSizeBytes();
        string log(string ip, unsigned short srcport, unsigned short dstport);
};

#endif