/*
* Autor: Roland Schulz (xschul06)
*/

#include "RRQPacket.hpp"

RRQPacket::RRQPacket(string filename, string mode, map<string,string> options): xRQPacket(RRQ, filename, mode, options){};
RRQPacket::RRQPacket(string filename, tftp_mode mode, map<string,string> options): xRQPacket(RRQ, filename, mode, options){};
RRQPacket::RRQPacket(const char* data, size_t length) : xRQPacket(RRQ, data, length){};