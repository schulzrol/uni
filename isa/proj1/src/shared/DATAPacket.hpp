#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <string.h>
#include "definitions.h"
#include "Packet.hpp"

#define DEFAULT_BLOCK_SIZE_BYTES 512

class DATAPacket : public Packet {

    short int block_number;
    string data;
    size_t block_size;
    tftp_mode mode;
    public:
        DATAPacket(short int block_number, const char* data, tftp_mode mode, size_t block_size);
        DATAPacket(const char* data, size_t block_size);
        short int getBlockNumber();
        string getData();
        tftp_mode getMode();
        void setMode(tftp_mode mode);
        string getDataEncoded(tftp_mode mode);
        void setBlockNumber(short int block_number);
        void setData(const char* data, size_t block_size);
        unsigned short getOpcode();
        string toByteStream();
        size_t blockSizeBytes();
        size_t maxSizeBytes();
};

#endif