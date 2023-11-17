//
// Created by Roland Schulz on 15.10.2023.
//
#include <iostream>
#include <err.h>
#include <map>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>

#include "shared/error_codes.h"
#include "shared/definitions.h"
#include "shared/PacketFactory.hpp"
#include "shared/RRQPacket.hpp"
#include "shared/WRQPacket.hpp"

using namespace std;

void getHelp(){
    auto help = "\
tftp-server [-p port] root_dirpath\n\
    -p místní port, na kterém bude server očekávat příchozí spojení\n\
       pokud není specifikován předpokládá se výchozí dle specifikace (69)\n\
    root_dirpath cesta k adresáři, pod kterým se budou ukládat příchozí soubory";
    cout << help << endl;
}

class Config{
    public:
        Config(int argc, char** argv){
            auto args = Config::parseArgs(argc, argv);
            this->port = stoi(args["port"]);
            this->root_dirpath = args["root_dirpath"];
        }
        int port;
        string root_dirpath;
        
        string toString(){
            return "port: " + to_string(this->port) + ", root_dirpath: " + this->root_dirpath;
        }
    private:
        map<string, string> parseArgs(int argc, char** argv){
            map<string, string> args = {
                {"port", to_string(SERVER_PORT)},
                {"root_dirpath", ""}
            };

            for (int i = 1; i < argc; i++){
                string arg = argv[i];
                if (arg == "-p"){
                    args["port"] = argv[++i];
                }
                else{
                    args["root_dirpath"] = argv[i];
                }
            }

            // postconditions
            if (args["root_dirpath"] == ""){
                cout << "Chybí parametr: root_dirpath" << endl;
                getHelp();
                exit(NE_ARGS);
            }

            return args;
        }
};


void handleChildPacket(const char* buffer,
                       size_t n,
                       int child_fd,
                       const struct sockaddr_in from,
                       const struct sockaddr_in assigned_client,
                       socklen_t length,
                       tftp_mode* mode,
                       bool* child_exit) {
    unsigned short block_number = 0;
    // check if packet is from assigned client
    if (from.sin_port != assigned_client.sin_port){
        cout << "Received packet from unknown client. Send back error to unknown client." << endl;
        cout << "Assigned client port: " << assigned_client.sin_port << endl;
        cout << "Received packet from port: " << from.sin_port << endl;
        // TODO send error packet
        size_t r = sendto(child_fd, "Error", 6, 0, (struct sockaddr *)&assigned_client, length); // send the answer
        if (r == -1){
            cout << "Error sending packet to assigned client" << endl;
        }
        if (r < 6){
            cout << "Not all bytes sent to assigned client" << endl;
        }
        return;
    }

    cout << "Received packet from assigned client" << endl;

    Packet* packet;
    try {
        packet = PacketFactory::createPacket(buffer, n, *mode);
    } catch (runtime_error& e) {
        cout << "Error creating packet from buffer: " << e.what() << endl;
        // TODO send error packet
        return;
    }
    
    switch (packet->getOpcode()) {
        case RRQ: {
            cout << "Received RRQ packet" << endl;
            RRQPacket* rrq_packet = (RRQPacket*)packet;
            cout << "RRQ packet filename: " << rrq_packet->getFilename() << endl;
            cout << "RRQ packet mode: " << rrq_packet->getMode() << endl;
            *mode = rrq_packet->getModeEnum();
            delete rrq_packet;
            break;
        }
        case WRQ: {
            cout << "Received WRQ packet" << endl;
            WRQPacket* wrq_packet = (WRQPacket*)packet;
            cout << "WRQ packet filename: " << wrq_packet->getFilename() << endl;
            cout << "WRQ packet mode: " << wrq_packet->getMode() << endl;
            *mode = wrq_packet->getModeEnum();
            delete wrq_packet;
            break;
        }
        case DATA: {
            cout << "Received DATA packet" << endl;
            DATAPacket* data_packet = (DATAPacket*)packet;
            cout << "DATA packet block number: " << data_packet->getBlockNumber() << endl;
            cout << "DATA packet data: " << data_packet->getData() << endl;
            block_number = data_packet->getBlockNumber();
            if (data_packet->getData().length() < DEFAULT_BLOCK_SIZE_BYTES){
                cout << "Received last DATA packet of length " << data_packet->getData().length() << "/" << DEFAULT_BLOCK_SIZE_BYTES << " bytes" << endl;
                *child_exit = true;
            }
            delete data_packet;
            break;
        }
    }
    
    ACKPacket ack = ACKPacket(block_number);
    {
        size_t r = sendto(child_fd, ack.toByteStream().c_str(), ack.getLength(), 0, (struct sockaddr *)&assigned_client, length); // send the answer
        if (r == -1)
        {
            // TODO send error packet
            cout << "Error sending packet to assigned client" << endl;
        }
        if (r < ack.getLength())
        {
            // TODO send error packet?
            cout << "Not all bytes sent to assigned client" << endl;
        }
        cout << "Sent ACK with block number: " << ack.getBlockNumber() << endl;
    }
}

/**
 * @brief Main for child process to server a client
 * 
 * @param child_process return value of fork()
 * @param config server configuration
 * @param client address of the client
 * @return int 0 if success
 */
void child_main(int* child_process,
               Config config,
               const struct sockaddr_in assigned_client,
               map<int, int> client_process_port,
               const struct sockaddr_in parent,
               const char* first_packet_buffer,
               const size_t first_packet_len
               ) {
    *child_process = 0;
    socklen_t length = sizeof(assigned_client);
    size_t n;
    const size_t bufsize = 1024;
    struct sockaddr_in from;
    bool first_packet_received = (first_packet_buffer != NULL);
    char first_packet_buffer_copy[first_packet_len];
    if (!memcpy(first_packet_buffer_copy, first_packet_buffer, first_packet_len)) {
        cout << "Error copying first packet buffer" << endl;
        return;
    }
    char buffer[bufsize];       // receiving buffer
    if ((*child_process = fork()) == 0) {
        // bind to a random port
        struct sockaddr_in child_server;
        int child_fd;
        child_server.sin_family = AF_INET;                // set IPv4 addressing
        child_server.sin_addr.s_addr = htonl(INADDR_ANY); // the server listens to any interface
        child_server.sin_port = htons(0);     // the server listens on this port (random)
        if ((child_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { // create the server UDP socket
            err(1, "socket() failed");
        }
        if ((::bind(child_fd, (struct sockaddr *)&child_server, sizeof(child_server))) == -1) { // binding with the port
            err(1, "bind() failed");
        }
        int child_port = ntohs(child_server.sin_port);
        cout << "Child port: " << child_port << endl;
        tftp_mode mode = netascii;
        bool child_exit = false;
        if (first_packet_received){
            handleChildPacket(first_packet_buffer_copy, first_packet_len, child_fd, assigned_client, assigned_client, length, &mode, &child_exit);
        }
        while ((!child_exit) && (n = recvfrom(child_fd, buffer, bufsize, 0, (struct sockaddr *)&from, &length)) >= 0) {
            handleChildPacket(buffer, n, child_fd, from, assigned_client, length, &mode, &child_exit);
        }
        cout << "Child process exiting" << endl;
        close(child_fd);
        exit(0);
    }
}

int main(int argc, char** argv) {
    // map for client TIDs and child process ports
    auto config = Config(argc, argv);
    int fd;                    // an incoming socket descriptor
    struct sockaddr_in server; // server's address structure
    size_t n;
    char buffer[1024];       // receiving buffer
    struct sockaddr_in client; // client's address structure
    socklen_t length;
    map<int, int> client_process_port; // client -> child process port mapping

    server.sin_family = AF_INET;                // set IPv4 addressing
    server.sin_addr.s_addr = htonl(INADDR_ANY); // the server listens to any interface
    server.sin_port = htons(config.port);     // the server listens on this port

    printf("* Opening an UDP socket ...\n");
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) // create the server UDP socket
        err(1, "socket() failed");

    printf("* Binding to the port %d (%d)\n", config.port, server.sin_port);
    if (::bind(fd, (struct sockaddr *)&server, sizeof(server)) == -1) // binding with the port
        err(1, "bind() failed");

    length = sizeof(client);
    while ((n = recvfrom(fd, buffer, 1024, 0, (struct sockaddr *)&client, &length)) >= 0) {
        printf("* Request received from %s, port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        int client_port = ntohs(client.sin_port);
        // create child process if not already exists
        bool created_child_for_this_port = (client_process_port.find(client_port) != client_process_port.end());
        bool child_process_running = created_child_for_this_port && waitpid(client_process_port[client_port], NULL, WNOHANG) == 0;
        if (!child_process_running) {
            cout << "Creating new child process for client port: " << client_port << endl;
            int child_pid = 0;
            child_main(&child_pid, config, client, client_process_port, server, buffer, n);
            if (child_pid != -1){
                cout << "Child process created: " << child_pid << endl;
                client_process_port[client_port] = child_pid;
            }
        } 
        else {
            // child process was already created yet client still sends a packet to the server instead of to the child process
            // send back error
            cout << "Child process already running for client port: " << client_port << endl;
            size_t r = sendto(fd, "Error", 6, 0, (struct sockaddr *)&client, length); // send the answer
            if (r == -1){
                cout << "Error sending packet to assigned client" << endl;
            }
            if (r < 6){
                cout << "Not all bytes sent to assigned client" << endl;
            }
        }
    }
    printf("* Closing the socket\n");
    close(fd);

    return 0;
}
