
#include "PacketFactory.hpp"

Packet* PacketFactory::createPacket(const char* data, size_t data_len, tftp_mode mode) {
    // read the opcode from the first two bytes of the data in network byte order (big endian)
    tftp_opcode opcode = tftp_opcode(ntohs(*(unsigned short*)data));
    size_t block_size = data_len-4;
    switch(opcode) {
        case RRQ:
            return new RRQPacket(data);
        case WRQ:
            return new WRQPacket(data);
        case DATA:
            return new DATAPacket(data, mode, block_size);
        /*
        case ACK:
            return ACKPacket(data);
        case ERR:
            return ERRORPacket(data);
        */
        default:
            throw runtime_error("Unsupported opcode");
    }
}