/*
* Autor: Roland Schulz (xschul06)
*/

#include "ACKPacket.hpp"

using namespace std;

ACKPacket::ACKPacket(unsigned short block_number){
    this->block_number = block_number;
}

ACKPacket::ACKPacket(const char* data) {
    unsigned short opcode = ntohs(*(unsigned short*)data);
    if (opcode != ACK){
        throw runtime_error("Invalid opcode");
    }
    this->block_number = ntohs(*(unsigned short*)(data + 2));
}

ACKPacket::ACKPacket(DATAPacket data_packet){
    this->block_number = data_packet.getBlockNumber();
}

short int ACKPacket::getBlockNumber(){
    return this->block_number;
}

void ACKPacket::setBlockNumber(short int block_number){
    this->block_number = block_number;
}

unsigned short ACKPacket::getOpcode(){
    return ACK;
}

string ACKPacket::toByteStream(){
    string packet;
    packet.push_back(0);
    packet.push_back(ACK);
    packet.push_back(static_cast<char>(this->block_number >> 8));
    packet.push_back(static_cast<char>(this->block_number));
    return packet;
}

size_t ACKPacket::getLength(){
    return ACK_PACKET_LENGTH_BYTES;
}

size_t ACKPacket::maxSizeBytes(){
    return ACK_PACKET_LENGTH_BYTES;
}

string ACKPacket::log(string ip, unsigned short srcport, unsigned short dstport){
    // ACK {SRC_IP}:{SRC_PORT} {BLOCK_ID}
    return "ACK " + ip + ":" + to_string(srcport) + " " + to_string(this->block_number);
}