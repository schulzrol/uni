#include "Packet.hpp"

unsigned short Packet::getOpcode()
{
    throw runtime_error("Not implemented");
}

char* Packet::toByteStream()
{
    throw runtime_error("Not implemented");
}

size_t Packet::getLength()
{
    throw runtime_error("Not implemented");
}