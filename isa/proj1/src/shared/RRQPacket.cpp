#include "RRQPacket.h"

RRQPacket::RRQPacket(string filename, string mode){
    this->filename = filename;
    this->mode = mode;
};

RRQPacket::RRQPacket(const char* data){
    if (data[0] != 0 || data[1] != this->getOpcode()){
        throw runtime_error("Invalid opcode");
    }
    this->filename = string(data + OPCODE_LENGTH_BYTES);
    this->mode = string(data + OPCODE_LENGTH_BYTES + this->filename.length() + 1); // +1 for null terminated string
};
const char* RRQPacket::getFilename(){
    return this->filename.c_str();
};
const char* RRQPacket::getMode(){
    return this->mode.c_str();
};
void RRQPacket::setFilename(const char* filename){
    this->filename = filename;
};
void RRQPacket::setMode(const char* mode){
    this->mode = mode;
};
char* RRQPacket::toByteStream(){
    char* data = new char[OPCODE_LENGTH_BYTES + strlen(this->getFilename()) + 1 + strlen(this->getMode()) + 1]; // +1s for null terminated strings
    unsigned short opcode = this->getOpcode();
    data[0] = opcode >> 8;
    data[1] = opcode;
    memcpy(data + OPCODE_LENGTH_BYTES, this->getFilename(), strlen(this->getFilename()) + 1); // +1 for null terminated string
    memcpy(data + OPCODE_LENGTH_BYTES + strlen(this->getFilename()) + 1, this->getMode(), strlen(this->getMode()) + 1); // +1s for null terminated string
    return data;
};

size_t RRQPacket::getLength(){
    return OPCODE_LENGTH_BYTES + strlen(this->getFilename()) + 1 + strlen(this->getMode()) + 1; // +1s for null terminated strings
};

unsigned short RRQPacket::getOpcode(){
    return 1;
};