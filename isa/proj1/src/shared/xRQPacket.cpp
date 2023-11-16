#include "xRQPacket.hpp"

using namespace std;

xRQPacket::xRQPacket(unsigned short opcode, string filename, string mode) {
    this->filename = filename;
    this->mode = mode;
    this->opcode = opcode;
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
const char *xRQPacket::getFilename() {
    return this->filename.c_str();
}
const char *xRQPacket::getMode() {
    return this->mode.c_str();
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
char *xRQPacket::toByteStream() {
    char *data = new char[OPCODE_LENGTH_BYTES + strlen(this->getFilename()) + 1 + strlen(this->getMode()) + 1]; // +1s for null terminated strings
    unsigned short opcode = this->getOpcode();
    data[0] = opcode >> 8;
    data[1] = opcode;
    memcpy(data + OPCODE_LENGTH_BYTES, this->getFilename(), strlen(this->getFilename()) + 1);                           // +1 for null terminated string
    memcpy(data + OPCODE_LENGTH_BYTES + strlen(this->getFilename()) + 1, this->getMode(), strlen(this->getMode()) + 1); // +1s for null terminated string
    return data;
}
size_t xRQPacket::getLength() {
    return OPCODE_LENGTH_BYTES + strlen(this->getFilename()) + 1 + strlen(this->getMode()) + 1; // +1s for null terminated strings
}
size_t xRQPacket::maxSizeBytes() {
    return 512;
}