/*
* Autor: Roland Schulz (xschul06)
*/

#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <string.h>
#include "definitions.h"
#include "Packet.hpp"

class DATAPacket : public Packet {
    unsigned short block_number;
    string data;
    unsigned short block_size;
    tftp_mode mode;
    private:
        string getDataDecoded(tftp_mode mode);
    public:
        DATAPacket(unsigned short block_number, const char* data, tftp_mode mode, unsigned short block_size);
        DATAPacket(const char* data, tftp_mode mode, unsigned short block_size);
        unsigned short getBlockNumber();
        string getData();
        tftp_mode getMode();
        void setMode(tftp_mode mode);
        string getDataEncoded(tftp_mode mode);
        void setBlockNumber(unsigned short block_number);
        void setData(const char* data, unsigned short block_size);
        unsigned short getOpcode();
        string toByteStream();
        unsigned short blockSizeBytes();
        size_t maxSizeBytes();
        size_t getLength();
        string log(string ip, unsigned short srcport, unsigned short dstport);
};

#endif