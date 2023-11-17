#include "xRQPacket.hpp"

using namespace std;

xRQPacket::xRQPacket(unsigned short opcode, string filename, string mode) {
    this->filename = filename;
    // TODO: check if mode is valid and turn to lowercase
    this->mode = mode;
    this->opcode = opcode;
}

xRQPacket::xRQPacket(unsigned short opcode, string filename, tftp_mode mode) {
    this->filename = filename;
    this->opcode = opcode;
    // TODO create a common function for this
    switch(mode) {
        case netascii:
            this->mode = "netascii";
            break;
        case octet:
            this->mode = "octet";
            break;
    }
}

xRQPacket::xRQPacket(unsigned short opcode, const char *data) {
    unsigned short gotOpcode = (unsigned short)data[1];
    if (data[0] != 0 || (gotOpcode != opcode))
    {
        throw runtime_error("Invalid request opcode");
    }
    this->opcode = opcode;
    this->filename = string(data + OPCODE_LENGTH_BYTES);
    this->mode = string(data + OPCODE_LENGTH_BYTES + this->filename.length() + 1); // +1 for null terminated string
}

string xRQPacket::getFilename() {
    return this->filename;
}
string xRQPacket::getMode() {
    return this->mode;
}

tftp_mode xRQPacket::getModeEnum() {
    if (this->mode == "netascii") {
        return netascii;
    } else if (this->mode == "octet") {
        return octet;
    } else {
        throw runtime_error("Unsupported mode");
    }
}
void xRQPacket::setFilename(const char *filename) {
    this->filename = filename;
}
void xRQPacket::setMode(const char *mode) {
    this->mode = mode;
}
unsigned short xRQPacket::getOpcode() {
    return this->opcode;
}
string xRQPacket::toByteStream() {
    unsigned short opcode = this->getOpcode();
    string packet(2, '\0');
    packet[0] = static_cast<char>(opcode >> 8);
    packet[1] = static_cast<char>(opcode);
    packet += this->getFilename() + '\0';
    packet += this->getMode() + '\0';
    return packet;
}
size_t xRQPacket::getLength() {
    return OPCODE_LENGTH_BYTES + this->getFilename().length() + 1 + this->getMode().length() + 1; // +1s for null terminated strings
}
size_t xRQPacket::maxSizeBytes() {
    return 512;
}