#include <iostream>
#include <string>
#include <map>

using namespace std;
#include "shared/definitions.h"

string operationLogFormat(tftp_opcode op){
    switch (op){
        case tftp_opcode::RRQ:
            return "RRQ {SRC_IP}:{SRC_PORT} \"{FILEPATH}\" {MODE} {$OPTS}";
        case tftp_opcode::WRQ:
            return "WRQ {SRC_IP}:{SRC_PORT} \"{FILEPATH}\" {MODE} {$OPTS}";
        case tftp_opcode::ACK:
            return "ACK {SRC_IP}:{SRC_PORT} {BLOCK_ID}";
        case tftp_opcode::DATA:
            return "DATA {SRC_IP}:{SRC_PORT}:{DST_PORT} {BLOCK_ID}";
        case tftp_opcode::OACK:
            // Jednotlivé extension options {$OPTS} pak ve formátu dle pořadí v datovém přenosu:
            // {OPT1_NAME}={OPT1_VALUE} ... {OPTn_NAME}={OPTn_VALUE}
            return "OACK {SRC_IP}:{SRC_PORT} {$OPTS}";
        case tftp_opcode::ERR:
            return "ERROR {SRC_IP}:{SRC_PORT}:{DST_PORT} {CODE} \"{MESSAGE}\"";
    }
}