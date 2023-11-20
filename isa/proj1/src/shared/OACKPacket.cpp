/*
* Autor: Roland Schulz (xschul06)
*/

#include "OACKPacket.hpp"

map<string, string> readOptions(const char* buffer, size_t length){
    map<string, string> options = {};
    for (int i = 0; i < length; i++) {
        string key = "";
        string value = "";
        while (buffer[i] != '\0') {
            key += buffer[i];
            i++;
        }
        i++;
        while (buffer[i] != '\0') {
            value += buffer[i];
            i++;
        }
        options[toLower(key)] = value;
    }
    return options;
}

string writeOptions(map<string, string> options){
    string packet = "";
    for (pair<string, string> kv : options) {
        packet += kv.first + '\0' + kv.second + '\0';
    }
    return packet;
}

string logOptions(map<string, string> options){
    // {OPT1_NAME}={OPT1_VALUE} ... {OPTn_NAME}={OPTn_VALUE}
    string log = "";
    for (pair<string, string> kv : options) {
        log += kv.first + "=" + kv.second + " ";
    }
    return log;
}

OACKPacket::OACKPacket(map<string, string> options){
    for (pair<string, string> kv : options) {
        this->setOption(kv.first, kv.second);
    }
}

OACKPacket::OACKPacket(const char *data, size_t length) {
    unsigned short gotOpcode = (unsigned short)data[1];
    if (data[0] != 0 || (gotOpcode != OACK)) {
        throw runtime_error("Invalid OACK opcode");
    }
    this->options = readOptions(data + OPCODE_LENGTH_BYTES, length - OPCODE_LENGTH_BYTES);
}

map<string, string> OACKPacket::getOptions() {
    return this->options;
}

map<string, string> OACKPacket::getOptions(set<string> keys) {
    // filter options by keys
    map<string, string> filteredOptions;
    for (pair<string, string> kv : this->options) {
        string key = toLower(kv.first);
        if (keys.find(key) != keys.end()) {
            filteredOptions[key] = kv.second;
        }
    }
    return filteredOptions;
}

void OACKPacket::setOption(string key, string value) {
    if (!(key.empty() || value.empty())){
        this->options[toLower(key)] = value;
    }
}

void OACKPacket::setOptions(map<string, string> options) {
    for (pair<string, string> kv : options) {
        this->setOption(kv.first, kv.second);
    }
}

string OACKPacket::toByteStream() {
    string packet(2, '\0');
    packet[0] = 0;
    packet[1] = OACK;
    for (pair<string, string> kv : this->options) {
        packet += kv.first + '\0' + kv.second + '\0';
    }
    return packet;
}

unsigned short OACKPacket::getOpcode() {
    return OACK;
}

size_t OACKPacket::getLength() {
    return this->toByteStream().length();
}

size_t OACKPacket::maxSizeBytes() {
    return (xRQPacket::maxSizeBytes()-4); // -4 bytes for filename and mode
}

size_t OACKPacket::keyCount() {
    return this->options.size();
}

string OACKPacket::log(string ip, unsigned short srcport, unsigned short dstport) {
    // OACK {SRC_IP}:{SRC_PORT} {$OPTS}
    return "OACK " + ip + ":" + to_string(srcport) + " " + logOptions(this->options);
}