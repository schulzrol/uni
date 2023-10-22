#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <string.h>
#include "definitions.h"
#include "Packet.h"

#define DATA_LENGTH_BYTES 512

class DataPacket : Packet {

    short int block_number;
    char* data;
    size_t data_length;
    public:
        DataPacket(short int block_number, char* data, int data_length);
        DataPacket(char* data, int data_length);
        short int getBlockNumber();
        char* getData();
        void setBlockNumber(short int block_number);
        void setData(char* data);
        unsigned short getOpcode();
        char* toByteStream();
};

#endif