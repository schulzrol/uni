#include "RRQPacket.hpp"

RRQPacket::RRQPacket(string filename, string mode): xRQPacket(RRQ, filename, mode){};
RRQPacket::RRQPacket(const char* data) : xRQPacket(RRQ, data){};