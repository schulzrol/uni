#include "DataTransfer.hpp"

#include <err.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool reserveSpaceForFile(FILE* f, size_t n){
    int r = 1;
    #ifdef __APPLE__
        r = fcntl(fileno(f), F_PREALLOCATE, 0, n);
    #else 
        r = fallocate(fileno(f), 0, 0, n);
    #endif
    return r == 0;
}

void handleErrnoFeedback(int errno_copy,
                         ERRORPacket *ep,
                         ssize_t *n,
                         int child_fd,
                         const struct sockaddr_in assigned_client,
                         socklen_t *length)
{
    switch (errno_copy) {
        case EFBIG:
        case ENOSPC:
            ep->setErrorCode(DISK_FULL);
            sendPacket(n, child_fd, ep, assigned_client, *length, NULL, NULL);
            break;
        case EEXIST:
            ep->setErrorCode(FILE_ALREADY_EXISTS);
            sendPacket(n, child_fd, ep, assigned_client, *length, NULL, NULL);
            break;
        case EACCES:
            ep->setErrorCode(ACCESS_VIOLATION);
            sendPacket(n, child_fd, ep, assigned_client, *length, NULL, NULL);
            break;
        case EISDIR:
            ep->setErrorCode(FILE_NOT_FOUND);
            sendPacket(n, child_fd, ep, assigned_client, *length, NULL, NULL);
            break;
        case ENOENT:
            ep->setErrorCode(FILE_NOT_FOUND);
            sendPacket(n, child_fd, ep, assigned_client, *length, NULL, NULL);
            break;
        case ENOTDIR:
            ep->setErrorCode(FILE_NOT_FOUND);
            sendPacket(n, child_fd, ep, assigned_client, *length, NULL, NULL);
            break;
        default:
            ep->setErrorCode(NOT_DEFINED);
            ep->setErrorMessage(strerror(errno_copy));
            sendPacket(n, child_fd, ep, assigned_client, *length, NULL, NULL);
            break;
    }
}

// partner_addr co mi dal pro me nic neznamena, protoze kdo ti posila zjistis az z prvni prijmute zpravy
DataTransfer::DataTransfer(int my_socket, tftp_mode transfer_mode, bool usedOptions, unsigned short block_size, int timeout_s, unsigned short myport) {
    this->my_socket = my_socket;
    this->transfer_mode = transfer_mode;
    this->block_size = block_size;
    this->usedOptions = usedOptions;
    this->timeout_s = timeout_s;
    this->retries = 0;
    this->max_retries = 5-1;
    this->last_sent = "";
    this->last_sent_n = 0;
    this->myport = myport;
}

bool handleSendToReturn(ssize_t n, size_t length){
    if (n == -1) {
        err(1, "send() failed");
        return false;
    }
    else if (n != length) {
        err(1, "send(): buffer written partially");
        return false;
    }
    return true;
}

bool handleRecvFromReturn(ssize_t n){
    if (n == -1){
        err(1, "recvfrom() failed");
        return false;
    }
    return true;
}

/**
 * @brief Function for receiving a packet from a partner
 * 
 * @param n number of bytes received
 * @param socket socket to receive from
 * @param buffer buffer to store received data
 * @param buflen length of buffer
 * @param from_addr address of the sender
 * @param from_size size of the address of the sender
 * @param reference_address_already_set true if function should check equality of sender and established partner TIDs
 * @param partner_addr established partner address
 * @param partner_size size of established partner address
 * @return int 0 if success, 1 if error, 2 if TIDs don't match, 3 if timeout
 */
int receivePacket(ssize_t* n, int socket, char* buffer, size_t buflen, sockaddr_in* from_addr, socklen_t* from_size, bool* reference_address_already_set, sockaddr_in* partner_addr, socklen_t* partner_size, int timeout_s){
    struct pollfd pfd = {.fd = socket, .events = POLLIN};
    int poll_result = 0;
    if (timeout_s >= 0){
        poll_result = poll(&pfd, 1, timeout_s*1000);
        if (poll_result == 0){
            // timeout
            return 3;
        }
        else if (poll_result == -1){
            // error
            return 1;
        }
    }

    *n = recvfrom(socket, buffer, buflen, 0,(sockaddr*) from_addr, from_size);
    if (!handleRecvFromReturn(*n)) {
        // error receiving packet
        return 1;
    }
    if (!(*reference_address_already_set)) {
        *partner_addr = *from_addr;
        *partner_size = *from_size;
        *reference_address_already_set = true;
    }
    else if (from_addr->sin_port != partner_addr->sin_port) {
        // received from unexpected port
        // TODO send back error and go back to waiting for my packet
        return 2;
    }
    return 0;
}


int sendPacketFromString(ssize_t* n, int socket, string packet, const sockaddr_in partner_addr, socklen_t partner_size, string* last_sent_packet, ssize_t* last_sent_packet_n){
    *n = sendto(socket, packet.c_str(), packet.size(), 0, (const sockaddr *)&partner_addr, partner_size);
    if (!handleSendToReturn(*n, packet.size())) {
        // TODO try again or timeout
        std::cout << "Error sending DATAPacket" << endl;
        return 1;
    }
    if (last_sent_packet != NULL){
        *last_sent_packet = packet;
        *last_sent_packet_n = *n;
    }
    return 0;
}


/**
 * @brief Function for sending a packet to a partner
 * 
 * @param n number of bytes sent
 * @param socket socket to send through
 * @param packet packet to send
 * @param partner_addr address of the partner
 * @param partner_size size of partner address
 * @return int 0 if success, 1 if error sending packet
 */
int sendPacket(ssize_t* n, int socket, Packet* packet, const sockaddr_in partner_addr, socklen_t partner_size, string* last_sent_packet, ssize_t* last_sent_packet_n){
    return sendPacketFromString(n, socket, packet->toByteStream(), partner_addr, partner_size, last_sent_packet, last_sent_packet_n);
}

// todo add handler for when the first waiting recvfrom times out (possibly only a packet object to resend)
/**
 * @brief Function for handling downloading of file from a remote partner
 *        Corresponds to the looping part of transfer for client side of RRQ and server side of WRQ
 *  t  self      partner
 *  |  |         |  
 *  |  |<-DATA---|  Waits expecting data packet from partner with increasing block number (starting 0, then block number 1, 2, etc.)
 *  |  |         |  until the length of the data packet is less than block_size.
 *  |  |----ACK->|  Then sends an ACK with the block number of the last received data packet.
 *  |  |         |  
 *  v  |  loop?  |  Loops until the last data packet is received (data length in packet < block_size).
 * 
 * @param to Already opened file to write to received data, expects write permissions and binary mode
 * @param skip_first_data_receive set to true if running on server side of WRQ, false if running on client side of RRQ
 * @param partner_addr address of the partner - must be not NULL if skip_first_data_receive set to true.
 *                     If NULL, the address of the first received packet will be used.
 * @param partner_size size of partner address - must be not NULL if skip_first_data_receive set to true
 * @return int 0 if successful, nonzero if unsuccessful
 */
int DataTransfer::downloadFile(FILE* to, bool skip_first_data_receive, const sockaddr_in* partner_addr, const socklen_t* partner_size, const sockaddr_in* default_partner_addr, const socklen_t* default_partner_size) {
    ssize_t n;
    size_t w;
    bool havent_already_handled_oack = !skip_first_data_receive;
    bool havent_already_sent_oack = skip_first_data_receive;
    const size_t buflen = max(1024, this->block_size + 4);
    sockaddr_in from_addr;
    socklen_t from_size = sizeof(from_addr);
    bool reference_address_already_set = (partner_addr != NULL && partner_size != NULL);
    sockaddr_in reference_addr = (reference_address_already_set) ? *partner_addr : from_addr; // set to from_addr because it will be overwritten
    socklen_t reference_addr_size = (reference_address_already_set) ? *partner_size : from_size; // set to from_size because it will be overwritten
    char buffer[buflen];
    ERRORPacket ep(NOT_DEFINED);
    DATAPacket *dp = NULL;
    bool receivedLastDataPacket = false;
    unsigned short block_number = (skip_first_data_receive) ? 0 : (this->usedOptions) ? 0 : 1;
    do {
        if (this->retries > this->max_retries){
            std::cout << "Max retries reached" << endl;
            return 0;
        }
        // Receive packet
        if (!skip_first_data_receive) {
            {
                std::cout << "Waiting for DATAPacket" << endl;
                switch (receivePacket(&n, this->my_socket, buffer, buflen, &from_addr, &from_size, &reference_address_already_set, &reference_addr, &reference_addr_size, this->timeout_s))
                {
                case 0:
                    break;
                case 1:
                    // TODO try again?
                    std::cout << "Error receiving DATAPacket" << endl;
                    continue;
                case 2:
                    ep.setErrorCode(UNKNOWN_TID);
                    sendPacket(&n, this->my_socket, &ep, from_addr, from_size, NULL, NULL);
                    std::cout << "Received DATAPacket from unexpected TID" << endl;
                    ep.setErrorCode(NOT_DEFINED);
                    continue;
                case 3: // timeout
                    std::cout << "Timeout " << this->retries << " out of " << this->max_retries << endl;
                    std::cout << "Sending last sent packet again" << endl;
                    if (block_number == 0)
                    {
                        sendPacketFromString(&this->last_sent_n, this->my_socket, this->last_sent, *default_partner_addr, *default_partner_size, NULL, NULL);
                    }
                    else
                    {
                        sendPacketFromString(&this->last_sent_n, this->my_socket, this->last_sent, reference_addr, reference_addr_size, NULL, NULL);
                    }
                    this->retries += 1;
                    this->timeout_s *= 2;
                    continue;
                default:
                    return 1;
                }
            }

            // expect data packet (TODO what if its not a data packet?)
            Packet *packet;
            try
            {
                packet = PacketFactory::createPacket(buffer, n, this->transfer_mode);
                string ip = inet_ntoa(from_addr.sin_addr);
                unsigned short srcport = ntohs(from_addr.sin_port);
                unsigned short dstport = this->myport;
                std::cerr << packet->log(ip, srcport, dstport) << endl;
                switch (packet->getOpcode())
                {
                case tftp_opcode::DATA:
                    this->retries = 0;
                    this->timeout_s = 1;
                    if (this->usedOptions && havent_already_handled_oack){
                        this->block_size = DEFAULT_BLOCK_SIZE_BYTES;
                    }
                    dp = (DATAPacket *)packet;
                    // Check the block number
                    if (dp->getBlockNumber() != block_number)
                    {
                        ep.setErrorCode(ILLEGAL_OPERATION);
                        sendPacket(&n, this->my_socket, &ep, from_addr, from_size, NULL, NULL);
                        std::cout << "Invalid DATAPacket block number " << dp->getBlockNumber() << " expected " << block_number << endl;
                        continue;
                    }
                    // Check the length of the received block of data
                    if (dp->getData().size() < this->block_size)
                    {
                        std::cout << "Received last DATAPacket with block number " << dp->getBlockNumber() << endl;
                        receivedLastDataPacket = true;
                    }
                    else
                    {
                        std::cout << "Received DATAPacket with block number " << dp->getBlockNumber() << endl;
                    }
                    std::cout << "Writing to file" << endl;
                    // Write data to file
                    w = fwrite(dp->getData().c_str(), 1, dp->getData().size(), to);
                    if (w != dp->getData().size())
                    {
                        std::cout << "Error writing to file" << endl;
                        handleErrnoFeedback(errno, &ep, &n, this->my_socket, from_addr, &from_size);
                        return 1;
                    }
                    delete dp; // TODO if memory becomes an issue, ditch packetfactory and use a switch statement to create packet objects in static memory
                    break;
                case tftp_opcode::OACK:
                    if (this->usedOptions && havent_already_handled_oack){
                        this->retries = 0;
                        this->timeout_s = 1;
                        havent_already_handled_oack = false;
                        if (this->handleOACKPacket((OACKPacket*)packet, from_addr, from_size, &ep) != 0){
                            return 1;
                        }
                    } else {
                        ep.setErrorCode(ILLEGAL_OPERATION);
                        sendPacket(&n, this->my_socket, &ep, from_addr, from_size, NULL, NULL);
                        std::cout << "Received unexpected OACK packet" << endl;
                        continue;
                    }
                    break;
                case tftp_opcode::ERR:
                    // TODO
                    std::cout << "Received ERROR packet" << endl;
                    return 1;
                default:
                    ep.setErrorCode(ILLEGAL_OPERATION);
                    sendPacket(&n, this->my_socket, &ep, from_addr, from_size, NULL, NULL);
                    std::cout << "Received unexpected packet" << endl;
                    delete packet;
                    continue;
                }
            }
            catch (runtime_error &e)
            {
                // TODO invalid or unknown packet, send back error and go back to waiting for my expected packet
                std::cout << e.what() << endl;
                continue;
            }
        } else {
            skip_first_data_receive = false;
        }
        // Send back O/ACK
        if (this->usedOptions && havent_already_sent_oack){
            havent_already_sent_oack = false;
            // send oack packet
            OACKPacket oack({{"blksize", to_string(this->block_size)}});
            std::cout << "Sending OACKPacket with block size " << this->block_size << endl;
            switch(sendPacket(&n, this->my_socket, &oack, reference_addr, reference_addr_size, &this->last_sent, &this->last_sent_n)){
                case 0:
                    break;
                case 1:
                    // TODO try again or timeout
                    std::cout << "Error sending OACK" << endl;
                    return 1;
                default:
                    return 1;
            }
            std::cout << "Sent OACKPacket with block size " << this->block_size << endl;
        }
        else
        {
            ACKPacket ack = ACKPacket(block_number);
            std::cout << "Sending ACKPacket with block number " << ack.getBlockNumber() << endl;
            switch(sendPacket(&n, this->my_socket, &ack, reference_addr, reference_addr_size, &this->last_sent, &this->last_sent_n)){
                case 0:
                    break;
                case 1:
                    // TODO try again or timeout
                    std::cout << "Error sending ACK" << endl;
                    return 1;
                default:
                    return 1;
            }
            std::cout << "Sent ACKPacket with block number: " << ack.getBlockNumber() << endl;
        }
        // Get ready for next block
        block_number++;
    } while(!receivedLastDataPacket);
    return 0;
}


int DataTransfer::handleOACKPacket(OACKPacket* oack, const sockaddr_in partner_addr, const socklen_t partner_size, ERRORPacket *ep){
    ssize_t n;
    map<string, string> blksize_option = oack->getOptions({"blksize"});
    if (blksize_option.size() != 1 || oack->keyCount() > 1 || !isNum(blksize_option["blksize"])) {
        ep->setErrorCode(BAD_OPTION);
        sendPacket(&n, this->my_socket, ep, partner_addr, partner_size, NULL, NULL);
        std::cout << "Didn't receive blksize or received more than asked for" << endl;
        delete oack;
        return 1;
    }
    // is num and is there
    int blksize = stoi(blksize_option["blksize"]);
    // checks if in range and smaller or equal to the set block size
    if (blksize < 8 || blksize > 65464 || blksize > this->block_size)
    {
        ep->setErrorCode(BAD_OPTION);
        sendPacket(&n, this->my_socket, ep, partner_addr, partner_size, NULL, NULL);
        std::cout << "blksize out of range" << endl;
        delete oack;
        return 1;
    }
    // is in range and is valid
    this->block_size = blksize;
    delete oack;
    return 0;
}

    // todo add handler for when the first waiting recvfrom times out (possibly only a packet object to resend)
    /**
     * @brief Function for handling uploading of file to a remote partner
     *        Corresponds to the looping part of transfer for client side of WRQ and server side of RRQ
     *  t  self      partner
     *  |  |         |
     *  |  |<-ACK----|  Waits for ACK from partner with block number same as the last sent data packet (starting 0).
     *  |  |         |
     *  |  |---DATA->|  Then sends a data packet with increasing block number (starting 1, then block number 2, 3, etc.).
     *  |  |         |
     *  v  |  loop?  |  Loops until the last data packet is sent (EOF). Waits for the last ACK but doesn't send anything after that.
     *
     * @param from Already opened file to read from to send data, expects read permissions and binary mode
     * @param skip_first_ack_receive set to true if running on server side of RRQ, false if running on client side of WRQ
     * @param partner_addr address of the partner - must be not NULL if skip_first_ack_receive set to true.
     *                     If NULL, the address of the first received packet will be used.
     * @param partner_size size of partner address - must be not NULL if skip_first_ack_receive set to true.
     * @return int 0 if successful, nonzero if unsuccessful
     */
    int DataTransfer::uploadFile(FILE * from, bool skip_first_ack_receive, const sockaddr_in *partner_addr, const socklen_t *partner_size, const sockaddr_in* default_partner_addr, const socklen_t* default_partner_size)
    {
        ssize_t n;
        size_t r;
        bool havent_already_handled_oack = !skip_first_ack_receive;
        bool havent_already_sent_oack = skip_first_ack_receive;
        const size_t buflen = max(1024, this->block_size * 2);
        sockaddr_in from_addr;
        socklen_t from_size = sizeof(from_addr);
        bool reference_address_already_set = (partner_addr != NULL && partner_size != NULL);
        sockaddr_in reference_addr = (reference_address_already_set) ? *partner_addr : from_addr;    // set to from_addr because it will be overwritten
        socklen_t reference_addr_size = (reference_address_already_set) ? *partner_size : from_size; // set to from_size because it will be overwritten
        char buffer[buflen];
        bool sentLastDataPacket = false;
        bool file_sent = false;
        ACKPacket *ack = NULL;
        ERRORPacket ep(NOT_DEFINED);
        unsigned short block_number = 0;
        while (!file_sent)
        {

            if (this->retries > this->max_retries){
                std::cout << "Max retries reached" << endl;
                return 0;
            }
            // Receive an ACK
            if (!skip_first_ack_receive)
            {
                {
                    std::cout << "Waiting for ACK" << endl;
                    switch (receivePacket(&n, this->my_socket, buffer, buflen, &from_addr, &from_size, &reference_address_already_set, &reference_addr, &reference_addr_size, this->timeout_s))
                    {
                    case 0:
                        break;
                    case 1:
                        // TODO try again?
                        std::cout << "Error receiving ACKPacket" << endl;
                        continue;
                    case 2:
                        // TODO send to sender back error and go back to waiting for my packet
                        ep.setErrorCode(UNKNOWN_TID);
                        sendPacket(&n, this->my_socket, &ep, from_addr, from_size, NULL, NULL);
                        std::cout << "Received ACKPacket from unexpected TID" << endl;
                        ep.setErrorCode(NOT_DEFINED);
                        continue;
                    case 3: // timeout
                        std::cout << "Timeout " << this->retries << " out of " << this->max_retries << endl;
                        std::cout << "Sending last sent packet again" << endl;
                        if (block_number == 0){
                            sendPacketFromString(&this->last_sent_n, this->my_socket, this->last_sent, *default_partner_addr, *default_partner_size, NULL, NULL);
                        } else {
                            sendPacketFromString(&this->last_sent_n, this->my_socket, this->last_sent, reference_addr, reference_addr_size, NULL, NULL);
                        }
                        this->retries += 1;
                        this->timeout_s *= 2;
                        continue;
                    default:
                        return 1;
                    }
                }
                // expect ACK packet (TODO what if its not a ACK packet?)
                Packet *packet;
                try
                {
                    packet = PacketFactory::createPacket(buffer, n, this->transfer_mode);
                    string ip = inet_ntoa(from_addr.sin_addr);
                    unsigned short srcport = ntohs(from_addr.sin_port);
                    unsigned short dstport = this->myport;
                    std::cerr << packet->log(ip, srcport, dstport) << endl;
                    switch (packet->getOpcode())
                    {
                    case tftp_opcode::ACK:
                        if (this->usedOptions && havent_already_handled_oack)
                        {
                            this->block_size = DEFAULT_BLOCK_SIZE_BYTES;
                        }
                        ack = (ACKPacket *)packet;
                        if (ack->getBlockNumber() != block_number)
                        {
                            ep.setErrorCode(ILLEGAL_OPERATION);
                            sendPacket(&n, this->my_socket, &ep, from_addr, from_size, NULL, NULL);
                            std::cout << "Invalid ACK block number" << endl;
                            continue;
                        }
                        if (sentLastDataPacket)
                        {
                            file_sent = true;
                            continue;
                        }
                        this->retries = 0;
                        this->timeout_s = 1;
                        delete ack;
                        break;

                    case tftp_opcode::OACK:
                        if (this->usedOptions && havent_already_handled_oack)
                        {
                            this->retries = 0;
                            this->timeout_s = 1;
                            havent_already_handled_oack = false;
                            if (this->handleOACKPacket((OACKPacket *)packet, from_addr, from_size, &ep) != 0)
                            {
                                return 1;
                            }
                        }
                        else
                        {
                            ep.setErrorCode(ILLEGAL_OPERATION);
                            sendPacket(&n, this->my_socket, &ep, from_addr, from_size, NULL, NULL);
                            std::cout << "Received unexpected OACK packet" << endl;
                            continue;
                        }
                        break;
                    case tftp_opcode::ERR:
                        // TODO
                        std::cout << "Received ERROR packet" << endl;
                        return 1;
                    default:
                        ep.setErrorCode(ILLEGAL_OPERATION);
                        sendPacket(&n, this->my_socket, &ep, from_addr, from_size, NULL, NULL);
                        delete packet;
                        continue;
                    }
                }
                catch (runtime_error &e)
                {
                    // TODO invalid or unknown packet, send back error and go back to waiting for my expected packet
                    std::cout << e.what() << endl;
                    continue;
                }
            }
            else
            {
                skip_first_ack_receive = false;
            }

        if (this->usedOptions && havent_already_sent_oack) { 
            havent_already_sent_oack = false;
            // send oack packet
            OACKPacket oack({{"blksize", to_string(this->block_size)}});
            std::cout << "Sending OACKPacket with block size " << this->block_size << endl;
            switch(sendPacket(&n, this->my_socket, &oack, reference_addr, reference_addr_size, &this->last_sent, &this->last_sent_n)){
                case 0:
                    break;
                case 1:
                    std::cout << "Error sending OACK" << endl;
                    return 1;
                default:
                    return 1;
            }
            std::cout << "Sent OACKPacket with block size " << this->block_size << endl;
        } else {
            // Get ready to send next block
            block_number++;

            // Read data from file
            {
                r = fread(buffer, 1, this->block_size, from);
                if (ferror(from)){
                    std::cout << "Error reading file" << endl;
                    return 1;
                }
                if (r < this->block_size) {
                    std::cout << "Read last block of data from file r=" << r << endl;
                    sentLastDataPacket = true;
                }
            }
            // Send data packet
            {
                DATAPacket dp(block_number, buffer, this->transfer_mode, r);
                std::cout << "Sending DATAPacket with block number " << dp.getBlockNumber() << " of size " << dp.getData().size() << endl;
                switch(sendPacket(&n, this->my_socket, &dp, reference_addr, reference_addr_size, &this->last_sent, &this->last_sent_n)){
                    case 0:
                        break;
                    case 1:
                        // TODO try again or timeout
                        std::cout << "Error sending ACK" << endl;
                        return 1;
                    default:
                        return 1;
                }
                std::cout << "Sent DATAPacket with block number: " << dp.getBlockNumber() << endl;
            }
        }
    }
    return 0;
}