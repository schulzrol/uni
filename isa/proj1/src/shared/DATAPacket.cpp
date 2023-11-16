#include "DATAPacket.hpp"

DATAPacket::DATAPacket(short int block_number, const char *data, size_t block_size = DEFAULT_BLOCK_SIZE_BYTES) {
    this->block_number = block_number;
    this->data = (char *)memcpy(new char[block_size], data, block_size);
    this->block_size = block_size;
};
DATAPacket::DATAPacket(const char *data, size_t block_size = DEFAULT_BLOCK_SIZE_BYTES) {
    if (data[0] != 0 || data[1] != this->getOpcode())
    {
        throw "Invalid opcode";
    }
    this->block_number = (data[2] << 8) + data[3];
    this->data = (char *)memcpy(new char[block_size], data + 4, block_size);
    this->block_size = block_size;
}
short int DATAPacket::getBlockNumber() {
    return this->block_number;
}
char * DATAPacket::getData() {
    return this->data;
}

void DATAPacket::setBlockNumber(short int block_number) {
    this->block_number = block_number;
}
void DATAPacket::setData(const char *data, size_t block_size = DEFAULT_BLOCK_SIZE_BYTES) {
    memcpy(this->data, data, block_size);
}
char* DATAPacket::toByteStream() {
    char *data = new char[OPCODE_LENGTH_BYTES + BLOCK_NUMBER_LENGTH_BYTES + this->block_size];
    unsigned short opcode = this->getOpcode();
    data[0] = opcode >> 8;
    data[1] = opcode;
    data[2] = this->getBlockNumber() >> 8;
    data[3] = this->getBlockNumber();
    memcpy(data + OPCODE_LENGTH_BYTES + BLOCK_NUMBER_LENGTH_BYTES, this->data, this->block_size);
    return data;
}

unsigned short DATAPacket::getOpcode() {
    return 3;
}

size_t DATAPacket::blockSizeBytes() {
    return this->block_size;
}

size_t DATAPacket::maxSizeBytes() {
    return OPCODE_LENGTH_BYTES + BLOCK_NUMBER_LENGTH_BYTES + this->block_size;
}