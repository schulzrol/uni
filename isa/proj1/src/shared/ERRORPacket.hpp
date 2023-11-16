#ifndef ERROR_PACKET_H
#define ERROR_PACKET_H

#include <string>
#include "Packet.hpp"

class ERRORPacket : public Packet {
    short int error_code;
    string error_message;
    public:
        ERRORPacket(short int error_code, string error_message);
        ERRORPacket(const char* data);
        short int getErrorCode();
        string getErrorMessage();
        void setErrorCode(short int error_code);
        void setErrorMessage(string error_message);
        unsigned short getOpcode();
        string toByteStream();
        size_t getLength();
        size_t maxSizeBytes();
};

#endif