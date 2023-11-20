/*
* Autor: Roland Schulz (xschul06)
*/

#include "WRQPacket.hpp"

WRQPacket::WRQPacket(string filename, string mode, map<string, string> options) : xRQPacket(WRQ, filename, mode, options){};
WRQPacket::WRQPacket(string filename, tftp_mode mode, map<string, string> options) : xRQPacket(WRQ, filename, mode, options){};
WRQPacket::WRQPacket(const char* data, size_t length) : xRQPacket(WRQ, data, length){};