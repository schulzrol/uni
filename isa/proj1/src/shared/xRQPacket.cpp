#include "xRQPacket.hpp"

using namespace std;

xRQPacket::xRQPacket(unsigned short opcode, string filename, string mode, map<string, string> options) {
    this->filename = filename;
    this->mode = toLower(mode);
    this->opcode = opcode;
    this->setOptions(options);
}

xRQPacket::xRQPacket(unsigned short opcode, string filename, tftp_mode mode, map<string, string> options) {
    this->filename = filename;
    this->opcode = opcode;
    switch(mode) {
        case netascii:
            this->mode = "netascii";
            break;
        case octet:
            this->mode = "octet";
            break;
    }
    this->setOptions(options);
}

xRQPacket::xRQPacket(unsigned short opcode, const char *data, size_t length) {
    length = min(length, this->maxSizeBytes());
    unsigned short gotOpcode = (unsigned short)data[1];
    if (data[0] != 0 || (gotOpcode != opcode))
    {
        throw runtime_error("Invalid request opcode");
    }
    this->opcode = opcode;
    this->filename = string(data + OPCODE_LENGTH_BYTES);
    this->mode = toLower(string(data + OPCODE_LENGTH_BYTES + this->filename.length() + 1)); // +1 for null terminated string
    this->options = {};
    if (length > (2+this->filename.length()+1+this->mode.length()+1)) {
        const char* options_start = data + OPCODE_LENGTH_BYTES + this->filename.length() + 1 + this->mode.length() + 1; // +1s for null terminated strings
        size_t options_length = length - (OPCODE_LENGTH_BYTES+this->filename.length()+1+this->mode.length()+1);
        this->options = readOptions(options_start, options_length);
    }
}

bool mapContainsValidBlksizeOption(map<string, string> options, unsigned short* blksize, int leq_than){
    if (options.find("blksize") == options.end() || !isNum(options["blksize"])) {
        return false;
    }
    int temp_blksize = stoi(options["blksize"]);
    if (temp_blksize < 8 || temp_blksize > 65464 || temp_blksize > leq_than) {
        return false;
    }
    *blksize = temp_blksize;
    return true;
}

bool xRQPacket::containsValidBlksizeOption(unsigned short& blksize, int leq_than) {
    return mapContainsValidBlksizeOption(this->options, &blksize, leq_than);
}

map<string, string> xRQPacket::getOptions() {
    return this->options;
}

void xRQPacket::setOption(string key, string value) {
    if (!(key.empty() || value.empty())){
        this->options[toLower(key)] = value;
    }
}

void xRQPacket::setOptions(map<string, string> options) {
    for (pair<string, string> kv : options) {
        this->setOption(kv.first, kv.second);
    }
}


string xRQPacket::getFilename() {
    return this->filename;
}
string xRQPacket::getMode() {
    return this->mode;
}

tftp_mode xRQPacket::getModeEnum() {
    if (toLower(this->mode) == "netascii") {
        return netascii;
    } else if (toLower(this->mode) == "octet") {
        return octet;
    } else {
        throw runtime_error("Unsupported mode");
    }
}
void xRQPacket::setFilename(const char *filename) {
    this->filename = filename;
}
void xRQPacket::setMode(const char *mode) {
    this->mode = toLower(mode);
}
unsigned short xRQPacket::getOpcode() {
    return this->opcode;
}
string xRQPacket::toByteStream() {
    unsigned short opcode = this->getOpcode();
    string packet(2, '\0');
    packet[0] = static_cast<char>(opcode >> 8);
    packet[1] = static_cast<char>(opcode);
    packet += this->getFilename() + '\0';
    packet += this->getMode() + '\0';
    packet += writeOptions(this->getOptions());
    return packet;
}
size_t xRQPacket::getLength() {
    return this->toByteStream().length();
}
size_t xRQPacket::maxSizeBytes() {
    return 512;
}