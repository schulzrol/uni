#include "WRQPacket.hpp"

WRQPacket::WRQPacket(string filename, string mode) : xRQPacket(WRQ, filename, mode){};
WRQPacket::WRQPacket(const char* data) : xRQPacket(WRQ, data){};