#ifndef OACK_PACKET_HPP
#define OACK_PACKET_HPP

#include "Packet.hpp"
#include "xRQPacket.hpp"
#include <map>
#include <set>
#include <string>
#include "upperlower.hpp"
#include "definitions.h"

using namespace std;

map<string,string> readOptions(const char* buffer, size_t length);
string writeOptions(map<string, string> options);
string logOptions(map<string, string> options);

class OACKPacket : public Packet {
    private:
        map<std::string, std::string> options;
    public:
        OACKPacket(const char *data, size_t length);
        OACKPacket(map<string, string> options);
        map<string, string> getOptions();
        map<string, string> getOptions(set<string> keys);
        void setOption(string key, string value);
        void setOptions(map<string, string> options);
        string toByteStream();
        unsigned short getOpcode();
        size_t getLength();
        size_t keyCount();
        size_t maxSizeBytes();
        string log(string ip, unsigned short srcport, unsigned short dstport);
};

#endif