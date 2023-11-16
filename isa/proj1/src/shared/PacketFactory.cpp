
#include "PacketFactory.hpp"

Packet* PacketFactory::createPacket(const char* data, size_t data_len, size_t block_size=DEFAULT_BLOCK_SIZE_BYTES) {
    // read the opcode from the first two bytes of the data in network byte order (big endian)
    unsigned short opcode = ntohs(*(unsigned short*)data);
    switch(opcode) {
        case 1:
            return new RRQPacket(data);
        case 2:
            return new WRQPacket(data);
        case 3:
            return new DATAPacket(data, block_size);
        /*
        TODO: implement the rest of the packet types
        case 4:
            return new ACKPacket(data);
        case 5:
            return new ERRORPacket(data);
        */
        default:
            return NULL;
    }
}