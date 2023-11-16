#include "WRQPacket.hpp"

WRQPacket::WRQPacket(string filename, string mode) : xRQPacket(OPCODE_WRQ, filename, mode){};
WRQPacket::WRQPacket(const char* data) : xRQPacket(OPCODE_WRQ, data){};