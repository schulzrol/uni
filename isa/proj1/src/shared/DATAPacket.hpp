#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <string.h>
#include "definitions.h"
#include "Packet.hpp"

#define DATA_LENGTH_BYTES 512

class DATAPacket : public Packet {

    short int block_number;
    char* data;
    size_t data_length;
    public:
        DATAPacket(short int block_number, const char* data, size_t data_length);
        DATAPacket(const char* data, size_t data_length);
        short int getBlockNumber();
        char* getData();
        void setBlockNumber(short int block_number);
        void setData(const char* data, size_t data_length);
        unsigned short getOpcode();
        char* toByteStream();
};

#endif