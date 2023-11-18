#include "DataTransfer.hpp"

#include <err.h>

DataTransfer::DataTransfer(int my_socket, struct sockaddr_in partner_addr, tftp_mode transfer_mode, unsigned short block_size) {
    this->my_socket = my_socket;
    this->partner_addr = partner_addr;
    this->transfer_mode = transfer_mode;
    this->block_size = block_size;
    this->partner_size = sizeof(this->partner_addr);
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
 * @return int 0 if successful, nonzero if unsuccessful
 */
int DataTransfer::downloadFile(FILE* to) {
    ssize_t n;
    size_t w;
    const size_t buflen = this->block_size*2;
    sockaddr_in from_addr = this->partner_addr;
    socklen_t from_size = this->partner_size;
    char buffer[buflen];
    bool receivedLastDataPacket = false;
    unsigned short block_number = 0;
    do {
        // Todo timeout
        // Receive packet
        {
            n = recvfrom(this->my_socket, buffer, buflen, 0,(sockaddr*) &from_addr, &from_size);
            if (!handleRecvFromReturn(n)){
                // TODO try again or timeout
                cout << "Error receiving ACK" << endl;
                return 1;
            }
            if (from_addr.sin_port != this->partner_addr.sin_port){
                // TODO send back error and go back to waiting for my packet
                cout << "Received packet from unexpected port" << endl;
                return 1;
            }
        }
        // TODO may also receive an error packet and timeout
        // expect data packet (TODO what if its not a data packet?)
        Packet* packet;
        try {
            packet = PacketFactory::createPacket(buffer, n, this->transfer_mode);
            switch(packet->getOpcode()){
                case tftp_opcode::DATA:
                    break;
                case tftp_opcode::ERR:
                    // TODO
                    cout << "Received ERROR packet" << endl;
                    return 1;
                default:
                    cout << "Received unexpected packet" << endl;
                    continue;
            }
        } catch (runtime_error& e) {
            // TODO invalid or unknown packet, send back error and go back to waiting for my expected packet
            cout << e.what() << endl;
            continue;
        }
        DATAPacket* dp = (DATAPacket*)packet;
        // Check the block number
        {
            if (dp->getBlockNumber() != block_number){
                // TODO try again or timeout, or send back error and go back to waiting for my expected packet
                cout << "Invalid DATAPacket block number" << endl;
                continue;
            }
        }
        // Check the length of the received block of data
        {
            if (dp->getLength() < block_size){
                cout << "Received last DATAPacket with block number " << dp->getBlockNumber() << endl;
                receivedLastDataPacket = true;
            } else {
                cout << "Received DATAPacket with block number "<< dp->getBlockNumber() << endl;
            }
        }
        cout << "Writing to file" << endl;
        // Write data to file
        w = fwrite(dp->getData().c_str(), 1, dp->getData().size(), to);
        if (w != dp->getData().size()){
            cout << "Error writing to file" << endl;
            return 1;
        }
        // Send back ACK
        ACKPacket ack = ACKPacket(dp->getBlockNumber());
        {
            n = sendto(my_socket, ack.toByteStream().c_str(), ack.getLength(), 0, (struct sockaddr *)&partner_addr, partner_size); // send the answer
            if (!handleSendToReturn(n, ack.getLength())){
                // TODO try again or timeout
                cout << "Error sending ACK" << endl;
                return 1;
            }
            cout << "Sent ACK with block number: " << ack.getBlockNumber() << endl;
        }
        // Get ready for next block
        delete dp; // TODO if memory becomes an issue, ditch packetfactory and use a switch statement to create packet objects in static memory
        block_number++;
    } while(!receivedLastDataPacket);
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
 * @return int 0 if successful, nonzero if unsuccessful
 */
int DataTransfer::uploadFile(FILE* from) {
    ssize_t n;
    size_t r;
    const size_t buflen = this->block_size*2;
    sockaddr_in from_addr = this->partner_addr;
    socklen_t from_size = this->partner_size;
    char buffer[buflen];
    bool sentLastDataPacket = false;
    bool file_sent = false;
    unsigned short block_number = 0;
    cout << "Waiting for ACK" << endl;
    while (!file_sent) {
        // Receive an ACK
        {
        // Receive packet
            {
                n = recvfrom(this->my_socket, buffer, buflen, 0,(sockaddr*) &from_addr, &from_size);
                if (!handleRecvFromReturn(n)){
                    // TODO try again or timeout
                    cout << "Error receiving ACK" << endl;
                    return 1;
                }
                if (from_addr.sin_port != this->partner_addr.sin_port){
                    // TODO send back error and go back to waiting for my packet
                    cout << "Received packet from unexpected port" << endl;
                    return 1;
                }
            }
            try {
                ACKPacket ack = ACKPacket(buffer);
                if (ack.getBlockNumber() != block_number){
                    // TODO try again or timeout
                    cout << "Invalid ACK block number" << endl;
                    continue;
                }
                if (sentLastDataPacket){
                    file_sent = true;
                    continue;
                }
                cout << "Received ACK " << ack.getBlockNumber() << endl;
            } catch (runtime_error& e) {
                cout << e.what() << endl;
                continue;
            }
        }
        // Get ready to send next block
        block_number++;
        // Read data from file
        {
            r = fread(buffer, 1, this->block_size, from);

            if (ferror(from)){
                cout << "Error reading file" << endl;
                return 1;
            }
        }
        // Send data packet
        {
            DATAPacket dp(block_number, buffer, this->transfer_mode, r);
            if (dp.getLength() < block_size) {
                cout << "Sending last DATAPacket with block number " << dp.getBlockNumber() << endl;
            }
            else {
                cout << "Sending DATAPacket with block number " << dp.getBlockNumber() << endl;
            }
            n = sendto(this->my_socket, dp.toByteStream().c_str(), dp.getLength(), 0, (const sockaddr *)&this->partner_addr, this->partner_size);
            if (!handleSendToReturn(n, dp.getLength()))
            {
                // TODO try again or timeout
                cout << "Error sending DATAPacket" << endl;
                return 1;
            }
            cout << "Sent DATAPacket with block number: " << dp.getBlockNumber() << endl;
        }
    }

    cout << "File sent" << endl;
}