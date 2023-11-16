#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <string.h>
#include "definitions.h"
#include "Packet.hpp"

#define DEFAULT_BLOCK_SIZE_BYTES 512

class DATAPacket : public Packet {

    short int block_number;
    char* data;
    size_t block_size;
    public:
        DATAPacket(short int block_number, const char* data, size_t block_size);
        DATAPacket(const char* data, size_t block_size);
        short int getBlockNumber();
        char* getData();
        void setBlockNumber(short int block_number);
        void setData(const char* data, size_t block_size);
        unsigned short getOpcode();
        char* toByteStream();
        size_t blockSizeBytes();
        size_t maxSizeBytes();
};

#endif