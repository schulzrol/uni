/*
* Autor: Roland Schulz (xschul06)
*/

#include "ERRORPacket.hpp"

ERRORPacket::ERRORPacket(tftp_error_code error_code, string error_message) {
    this->error_code = error_code;
    this->error_message = error_message;
}

ERRORPacket::ERRORPacket(tftp_error_code error_code) {
    this->error_code = error_code;
    this->error_message = getErrorMessage(error_code);
}

ERRORPacket::ERRORPacket(const char* data) {
    tftp_opcode opcode = tftp_opcode(ntohs(*(unsigned short*)data));
    if(opcode != ERR) {
        throw runtime_error("Invalid opcode");
    }
    this->error_code = tftp_error_code(ntohs(*(unsigned short*)(data+2))); // +2 because opcode
    this->error_message = string(data+4); // +4 because opcode and error code
}

tftp_error_code ERRORPacket::getErrorCode() {
    return this->error_code;
}

string ERRORPacket::getErrorMessage() {
    return this->error_message;
}

string ERRORPacket::getErrorMessage(tftp_error_code error_code) {
    switch(error_code) {
        case NOT_DEFINED:
            return "Not defined, see error message (if any).";
        case FILE_NOT_FOUND:
            return "File not found.";
        case ACCESS_VIOLATION:
            return "Access violation.";
        case DISK_FULL:
            return "Disk full or allocation exceeded.";
        case ILLEGAL_OPERATION:
            return "Illegal TFTP operation.";
        case UNKNOWN_TID:
            return "Unknown transfer ID.";
        case FILE_ALREADY_EXISTS:
            return "File already exists.";
        case NO_SUCH_USER:
            return "No such user.";
        case BAD_OPTION:
            return "Bad request option or unsupported or denied.";
        default:
            return "Unknown error code.";
    }
}

void ERRORPacket::setErrorCode(tftp_error_code error_code) {
    this->error_code = error_code;
    this->error_message = getErrorMessage(error_code);
}

void ERRORPacket::setErrorMessage(string error_message) {
    this->error_message = error_message;
}

unsigned short ERRORPacket::getOpcode() {
    return ERR;
}

string ERRORPacket::toByteStream() {
    unsigned short opcode = this->getOpcode();
    unsigned short errcode = this->getErrorCode();
    string packet(4, '\0');
    packet[0] = static_cast<char>(opcode >> 8);
    packet[1] = static_cast<char>(opcode);
    packet[2] = static_cast<char>(errcode >> 8);
    packet[3] = static_cast<char>(errcode);
    packet += this->error_message + '\0';
    return packet;
}

unsigned short getOpcode() {
    return ERR;
}
size_t ERRORPacket::getLength() {
    return 4 + this->error_message.length() + 1; // +1 for null terminator
}

size_t ERRORPacket::maxSizeBytes() {
    return 516;
}

string ERRORPacket::log(string ip, unsigned short srcport, unsigned short dstport) {
    // ERROR {SRC_IP}:{SRC_PORT}:{DST_PORT} {CODE} "{MESSAGE}"
    return "ERROR " + ip + ":" + to_string(srcport) + ":" + to_string(dstport) + " " + to_string(this->error_code) + " \"" + this->error_message + "\"";
}