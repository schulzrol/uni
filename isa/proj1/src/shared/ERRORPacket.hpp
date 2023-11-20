/*
* Autor: Roland Schulz (xschul06)
*/

#ifndef ERROR_PACKET_H
#define ERROR_PACKET_H

#include <string>
#include "Packet.hpp"

enum tftp_error_code {
    NOT_DEFINED = 0,
    FILE_NOT_FOUND = 1,
    ACCESS_VIOLATION = 2,
    DISK_FULL = 3,
    ILLEGAL_OPERATION = 4,
    UNKNOWN_TID = 5,
    FILE_ALREADY_EXISTS = 6,
    NO_SUCH_USER = 7,
    BAD_OPTION = 8
};

class ERRORPacket : public Packet {
    tftp_error_code error_code;
    string error_message;
    public:
        ERRORPacket(tftp_error_code error_code, string error_message);
        ERRORPacket(tftp_error_code error_code);
        ERRORPacket(const char* data);
        tftp_error_code getErrorCode();
        string getErrorMessage();
        void setErrorCode(tftp_error_code error_code);
        void setErrorMessage(string error_message);
        string getErrorMessage(tftp_error_code error_code);
        unsigned short getOpcode();
        string toByteStream();
        size_t getLength();
        size_t maxSizeBytes();
        string log(string ip, unsigned short srcport, unsigned short dstport);
};

#endif