#include "DATAPacket.hpp"


string to_netascii(const string data){
    string netascii_data;
    for(char ch : data) {
        if(ch == '\n') {
            netascii_data.push_back('\r');
            netascii_data.push_back('\n');
        }
        else {
            netascii_data.push_back(ch);
        }
    }
    return netascii_data;
}

string to_octet(const string data){
    return data;
}

tftp_mode get_mode(const string mode) {
    if(mode == "netascii") {
        return netascii;
    } else if(mode == "octet") {
        return octet;
    } else {
        throw "Unsupported mode";
    }
}

DATAPacket::DATAPacket(short int block_number, const char *data, tftp_mode mode, size_t block_size = DEFAULT_BLOCK_SIZE_BYTES) {
    this->block_number = block_number;
    this->data = string(data, block_size);
    this->block_size = block_size;
    this->mode = mode;
};
DATAPacket::DATAPacket(const char *data, size_t block_size = DEFAULT_BLOCK_SIZE_BYTES) {
    if (data[0] != 0 || data[1] != this->getOpcode()) {
        throw "Invalid opcode";
    }
    this->block_number = (data[2] << 8) + data[3];
    this->data = string(data + 4, block_size);
    this->block_size = block_size;
    // +1 for null terminated string, +4 for opcode and block number
    this->mode = get_mode(string(data + 4 + this->data.length() + 1));
}
short int DATAPacket::getBlockNumber() {
    return this->block_number;
}
string DATAPacket::getData() {
    return this->data;
}

string DATAPacket::getDataEncoded(tftp_mode mode) {
    switch(mode) {
        case netascii:
            return to_netascii(this->getData());
        case octet:
            return to_octet(this->getData());
        default:
            throw "Unsupported mode";
    }
}

void DATAPacket::setBlockNumber(short int block_number) {
    this->block_number = block_number;
}
void DATAPacket::setData(const char *data, size_t block_size = DEFAULT_BLOCK_SIZE_BYTES) {
    this->data = string(data, block_size);
    this->block_size = block_size;
}
string DATAPacket::toByteStream() {
    string packet;
    unsigned short opcode = this->getOpcode();

    // Append opcode bytes to the string
    packet.push_back(static_cast<char>(opcode >> 8));
    packet.push_back(static_cast<char>(opcode));

    unsigned short blockNumber = this->getBlockNumber();
    packet.push_back(static_cast<char>(blockNumber >> 8));
    packet.push_back(static_cast<char>(blockNumber));

    packet.append(this->getDataEncoded(this->getMode()));

    return packet;
}

tftp_mode DATAPacket::getMode(){
    return this->mode;
}

void DATAPacket::setMode(tftp_mode mode){
    this->mode = mode;
}

unsigned short DATAPacket::getOpcode() {
    return DATA;
}

size_t DATAPacket::blockSizeBytes() {
    return this->block_size;
}

size_t DATAPacket::maxSizeBytes() {
    return OPCODE_LENGTH_BYTES + BLOCK_NUMBER_LENGTH_BYTES + this->block_size;
}