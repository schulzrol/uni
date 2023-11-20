/*
* Autor: Roland Schulz (xschul06)
*/

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
    // maximum UDP payload size
    return 65507;
}

string Packet::log(string ip, unsigned short srcport, unsigned short dstport) {
    throw runtime_error("Not implemented");
}

Packet::~Packet() { }
