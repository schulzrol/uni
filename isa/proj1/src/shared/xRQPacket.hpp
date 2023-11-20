#ifndef xRQPACKET_H
#define xRQPACKET_H

#include "definitions.h"
#include "Packet.hpp"
#include <string>
#include <map>
#include "upperlower.hpp"
#include "OACKPacket.hpp"
#include "isnum.hpp"

using namespace std;

class xRQPacket : public Packet {
    string filename;
    string mode;
    unsigned short opcode;
    map<string, string> options;
    public:
        xRQPacket(unsigned short opcode, string filename, string mode, map<string, string> options = {});
        xRQPacket(unsigned short opcode, string filename, tftp_mode mode, map<string, string> options = {});
        xRQPacket(unsigned short opcode, const char* data, size_t length);
        string getFilename();
        string getMode();
        tftp_mode getModeEnum();
        void setFilename(const char* filename);
        void setMode(const char* mode);
        string toByteStream();
        unsigned short getOpcode();
        size_t getLength();
        static size_t maxSizeBytes();
        map<string, string> getOptions();
        void setOption(string key, string value);
        void setOptions(map<string,string> options);
        bool containsValidBlksizeOption(unsigned short& blksize, int leq_than);
};

bool mapContainsValidBlksizeOption(map<string, string> options, unsigned short* blksize, int leq_than);

#endif