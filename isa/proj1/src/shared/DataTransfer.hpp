#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <poll.h>

#include "isnum.hpp"
#include "PacketFactory.hpp"
#include "definitions.h"

using namespace std;

class DataTransfer {
    int my_socket;
    tftp_mode transfer_mode;
    unsigned short block_size;
    bool usedOptions;
    int timeout_s;
    int retries;
    int max_retries;
    public:
        string last_sent;
        ssize_t last_sent_n;
        DataTransfer(int my_socket, tftp_mode transfer_mode, bool usedOptions, unsigned short block_size = DEFAULT_BLOCK_SIZE_BYTES, int timeout_s=-1);
        int uploadFile(FILE* from=stdin, bool skip_first_ack_receive=false, const sockaddr_in* partner_addr=NULL, const socklen_t* partner_size = NULL, const sockaddr_in* default_partner_address = NULL, const socklen_t* default_partner_size=NULL);
        int downloadFile(FILE* to, bool skip_first_data_receive=false, const sockaddr_in* partner_addr=NULL, const socklen_t* partner_size = NULL, const sockaddr_in* default_partner_address = NULL, const socklen_t* default_partner_size=NULL);
    private:
        int handleOACKPacket(OACKPacket* oack, const sockaddr_in partner_addr, const socklen_t partner_size, ERRORPacket* ep);
};

bool reserveSpaceForFile(FILE* f, size_t n);

void handleErrnoFeedback(int errno_copy,
                         ERRORPacket *ep,
                         ssize_t *n,
                         int child_fd,
                         const struct sockaddr_in assigned_client,
                         socklen_t *length);

bool handleRecvFromReturn(ssize_t n);
bool handleSendToReturn(ssize_t n, size_t length);

int sendPacket(ssize_t* n, int socket, Packet* packet, const sockaddr_in partner_addr, socklen_t partner_size, string* last_sent, ssize_t* last_sent_n);
int receivePacket(ssize_t* n, int socket, char* buffer, size_t buflen, sockaddr_in* from_addr, socklen_t* from_size, bool* reference_address_already_set, sockaddr_in* partner_addr, socklen_t* partner_size, int timeout_s=-1);

#endif