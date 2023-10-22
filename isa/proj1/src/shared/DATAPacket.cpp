#include "DATAPacket.h"

DataPacket::DataPacket(short int block_number, char *data, int data_length = DATA_LENGTH_BYTES) {
    this->block_number = block_number;
    this->data = (char *)memcpy(new char[data_length], data, data_length);
    this->data_length = data_length;
};
DataPacket::DataPacket(char *data, int data_length = DATA_LENGTH_BYTES) {
    if (data[0] != 0 || data[1] != this->getOpcode())
    {
        throw "Invalid opcode";
    }
    this->block_number = (data[2] << 8) + data[3];
    this->data = (char *)memcpy(new char[data_length], data + 4, data_length);
    this->data_length = data_length;
}
short int DataPacket::getBlockNumber() {
    return this->block_number;
}
char * DataPacket::getData() {
    return this->data;
}
void DataPacket::setBlockNumber(short int block_number) {
    this->block_number = block_number;
}
void DataPacket::setData(char *data) {
    this->data = data;
}
char* DataPacket::toByteStream() {
    char *data = new char[OPCODE_LENGTH_BYTES + BLOCK_NUMBER_LENGTH_BYTES + this->data_length];
    memset(data, this->getOpcode(), OPCODE_LENGTH_BYTES);
    data[2] = this->getBlockNumber() >> 8;
    data[3] = this->getBlockNumber();
    memcpy(data + OPCODE_LENGTH_BYTES + BLOCK_NUMBER_LENGTH_BYTES, this->data, this->data_length);
    return data;
}

unsigned short DataPacket::getOpcode() {
    return 3;
}