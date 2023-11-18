#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "PacketFactory.hpp"
#include "definitions.h"

using namespace std;

class DataTransfer {
    int my_socket;
    struct sockaddr_in partner_addr;
    size_t partner_size;
    tftp_mode transfer_mode;
    unsigned short block_size;
    public:
        DataTransfer(int my_socket, struct sockaddr_in partner_addr, tftp_mode transfer_mode, unsigned short block_size = DEFAULT_BLOCK_SIZE_BYTES);
        int uploadFile(FILE* from=stdin);
        int downloadFile(FILE* to=stdout);
};

#endif