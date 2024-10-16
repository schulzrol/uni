/*
* Autor: Roland Schulz (xschul06)
*/


#include "PacketFactory.hpp"

Packet* PacketFactory::createPacket(const char* data, size_t data_len, tftp_mode mode) {
    // read the opcode from the first two bytes of the data in network byte order (big endian)
    tftp_opcode opcode = tftp_opcode(ntohs(*(unsigned short*)data));
    size_t block_size = data_len-4;
    switch(opcode) {
        case RRQ:
            return new RRQPacket(data, data_len);
        case WRQ:
            return new WRQPacket(data, data_len);
        case DATA:
            return new DATAPacket(data, mode, block_size);
        case ACK:
            return new ACKPacket(data);
        case ERR:
            return new ERRORPacket(data);
        case OACK:
            return new OACKPacket(data, data_len);
        default:
            throw runtime_error("Unsupported opcode");
    }
}