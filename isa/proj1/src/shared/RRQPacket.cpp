#include "RRQPacket.hpp"

RRQPacket::RRQPacket(string filename, string mode): xRQPacket(OPCODE_RRQ, filename, mode){};

RRQPacket::RRQPacket(const char* data) : xRQPacket(OPCODE_RRQ, data){};