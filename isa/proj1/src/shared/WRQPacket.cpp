#include "WRQPacket.hpp"

WRQPacket::WRQPacket(string filename, string mode) : RRQPacket(filename, mode){};
WRQPacket::WRQPacket(const char* data) : RRQPacket(data){};