#include "Packet.hpp"

unsigned short Packet::getOpcode()
{
    throw runtime_error("Not implemented");
}

string Packet::toByteStream()
{
    throw runtime_error("Not implemented");
}

size_t Packet::getLength()
{
    throw runtime_error("Not implemented");
}

size_t Packet::maxSizeBytes()
{
    throw runtime_error("Not implemented");
}

Packet::~Packet() { }
