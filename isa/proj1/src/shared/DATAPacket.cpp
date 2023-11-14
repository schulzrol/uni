#include "DATAPacket.hpp"

DATAPacket::DATAPacket(short int block_number, const char *data, size_t data_length = DATA_LENGTH_BYTES) {
    this->block_number = block_number;
    this->data = (char *)memcpy(new char[data_length], data, data_length);
    this->data_length = data_length;
};
DATAPacket::DATAPacket(const char *data, size_t data_length = DATA_LENGTH_BYTES) {
    if (data[0] != 0 || data[1] != this->getOpcode())
    {
        throw "Invalid opcode";
    }
    this->block_number = (data[2] << 8) + data[3];
    this->data = (char *)memcpy(new char[data_length], data + 4, data_length);
    this->data_length = data_length;
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
void DATAPacket::setData(const char *data, size_t data_length = DATA_LENGTH_BYTES) {
    memcpy(this->data, data, data_length);
}
char* DATAPacket::toByteStream() {
    char *data = new char[OPCODE_LENGTH_BYTES + BLOCK_NUMBER_LENGTH_BYTES + this->data_length];
    unsigned short opcode = this->getOpcode();
    data[0] = opcode >> 8;
    data[1] = opcode;
    data[2] = this->getBlockNumber() >> 8;
    data[3] = this->getBlockNumber();
    memcpy(data + OPCODE_LENGTH_BYTES + BLOCK_NUMBER_LENGTH_BYTES, this->data, this->data_length);
    return data;
}

unsigned short DATAPacket::getOpcode() {
    return 3;
}