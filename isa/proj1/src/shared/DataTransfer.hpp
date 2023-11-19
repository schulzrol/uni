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
    tftp_mode transfer_mode;
    unsigned short block_size;
    public:
        DataTransfer(int my_socket, tftp_mode transfer_mode, unsigned short block_size = DEFAULT_BLOCK_SIZE_BYTES);
        int uploadFile(FILE* from=stdin, bool skip_first_ack_receive=false, const sockaddr_in* partner_addr=NULL, const socklen_t* partner_size = NULL);
        int downloadFile(FILE* to, bool skip_first_data_receive=false, const sockaddr_in* partner_addr=NULL, const socklen_t* partner_size = NULL);
};


void handleErrnoFeedback(int errno_copy,
                         ERRORPacket *ep,
                         ssize_t *n,
                         int child_fd,
                         const struct sockaddr_in assigned_client,
                         socklen_t *length);

bool handleRecvFromReturn(ssize_t n);
bool handleSendToReturn(ssize_t n, size_t length);

int sendPacket(ssize_t* n, int socket, Packet* packet, sockaddr_in partner_addr, socklen_t partner_size);
int receivePacket(ssize_t* n, int socket, char* buffer, size_t buflen, sockaddr_in* from_addr, socklen_t* from_size, bool* reference_address_already_set, sockaddr_in* partner_addr, socklen_t* partner_size);

#endif