//
// Created by Roland Schulz on 15.10.2023.
//
#include <iostream>
#include <cerrno>
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
#include "shared/DataTransfer.hpp"
#include "shared/ERRORPacket.hpp"
#include "shared/isnum.hpp"

void getHelp(){
    auto help = "\
tftp-server [-p port] root_dirpath\n\
    -p místní port, na kterém bude server očekávat příchozí spojení\n\
       pokud není specifikován předpokládá se výchozí dle specifikace (69)\n\
    root_dirpath cesta k adresáři, pod kterým se budou ukládat příchozí soubory";
    std::cout << help << endl;
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
                    if (i+1 >= argc){
                        std::cout << "Chybí hodnota parametru -p" << endl;
                        getHelp();
                        exit(NE_ARGS);
                    }
                    args["port"] = argv[++i];
                }
                else{
                    args["root_dirpath"] = argv[i];
                }
            }

            // postconditions
            if (args["root_dirpath"] == ""){
                std::cout << "Chybí parametr: root_dirpath" << endl;
                getHelp();
                exit(NE_ARGS);
            }

            if (!isNum(args["port"])){
                std::cout << "Parametr port musí být číslo" << endl;
                getHelp();
                exit(NE_ARGS);
            }

            if (stoi(args["port"]) < 0 || stoi(args["port"]) > 65535){
                std::cout << "Parametr -p musí být v rozsahu 0-65535" << endl;
                getHelp();
                exit(NE_ARGS);
            }

            return args;
        }
};

FILE* handleFileOpen(string filename,
                     string mode,
                     ERRORPacket* ep,
                     ssize_t* n,
                     int child_fd,
                     const struct sockaddr_in assigned_client,
                     socklen_t* length,
                     string tsize,
                     long* file_size)
                     {
    FILE *f = fopen(filename.c_str(), mode.c_str());
    if (f == NULL) {
        std::cout << "Error opening file \'" << filename << "\' : " << strerror(errno) << endl;
        handleErrnoFeedback(errno, ep, n, child_fd, assigned_client, length);
    }
    if (f != NULL && tsize != ""){
        if (reserveSpaceForFile(f, stoul(tsize))){
            std::cout << "Error reserving space for file \'" << filename << "\' : " << strerror(errno) << endl;
            handleErrnoFeedback(errno, ep, n, child_fd, assigned_client, length);
        }
    }
    if (file_size){
        fseek(f, 0, SEEK_END);
        *file_size = ftell(f);
        fseek(f, 0, SEEK_SET);
    }
    return f;
}

/**
 * @brief Main for child process to server a client
 * 
 * @param config server configuration
 * @param client address of the client
 * @return int 0 if success
 */
void child_main(int* child_process,
               Config config,
               const struct sockaddr_in assigned_client,
               const struct sockaddr_in parent,
               const char* first_packet_buffer,
               const size_t first_packet_len
               ) {
    *child_process = 0;
    if ((*child_process = fork()) == 0) {
        ERRORPacket ep(NOT_DEFINED);
        // bind to a random port
        socklen_t length = sizeof(assigned_client);
        struct sockaddr_in child_server;
        unsigned short blksize = DEFAULT_BLOCK_SIZE_BYTES;
        map<string, string> options = {};
        bool usedOptions = false;
        ssize_t n;
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
        
        socklen_t myport_len = sizeof(child_server);
        if (getsockname(child_fd, (struct sockaddr *)&child_server, &myport_len) == -1){
            err(1, "getsockname() failed");
        }
        int child_port = ntohs(child_server.sin_port);
        std::cout << "Child port: " << child_port << endl;

        Packet* first_packet;
        try {
            first_packet = PacketFactory::createPacket(first_packet_buffer, first_packet_len, tftp_mode::octet);
            string ip = inet_ntoa(assigned_client.sin_addr);
            unsigned short srcport = ntohs(assigned_client.sin_port);
            std::cerr << first_packet->log(ip, srcport, child_port) << endl;
        } catch (runtime_error& e) {
            std::cout << "Error creating packet from buffer: " << e.what() << endl;
            ep.setErrorCode(ILLEGAL_OPERATION);
            sendPacket(&n, child_fd, &ep, assigned_client, length, NULL, NULL);
            return;
        }
        switch (first_packet->getOpcode()){
            case RRQ: {
                RRQPacket* rrq = (RRQPacket*)first_packet;
                options = rrq->getOptions();
                if (mapContainsValidBlksizeOption(options, &blksize, 65535)){
                    usedOptions = true;
                }
                if (options.find("tsize") != options.end()){
                    string tsize = options["tsize"];
                    if (tsize != "0") {
                        ep.setErrorCode(ILLEGAL_OPERATION);
                        sendPacket(&n, child_fd, &ep, assigned_client, length, NULL, NULL);
                        std::cout << "bad tsize option value not supported" << endl;
                        delete rrq;
                        return;
                    }
                }
                DataTransfer dt = DataTransfer(child_fd, rrq->getModeEnum(), usedOptions, blksize, 5, child_port);
                string filename = config.root_dirpath + "/" + rrq->getFilename();
                delete rrq;

                long filesize = 0;
                FILE* f = handleFileOpen(filename, "rb", &ep, &n, child_fd, assigned_client, &length, "", &filesize);
                if (f == NULL) {
                    std::cout << "Child process exiting" << endl;
                    close(child_fd);
                    exit(1);
                }
                dt.uploadFile(f, true, &assigned_client, &length, &assigned_client, &length);
                fclose(f);
                break;
            }
            case WRQ: {
                WRQPacket* wrq = (WRQPacket*)first_packet;
                options = wrq->getOptions();
                if (mapContainsValidBlksizeOption(options, &blksize, 65535)){
                    usedOptions = true;
                }
                DataTransfer dt = DataTransfer(child_fd, wrq->getModeEnum(), usedOptions, blksize, 5, child_port);
                string filename = config.root_dirpath + "/" + wrq->getFilename();
                delete wrq;

                FILE* f = handleFileOpen(filename, "wxb", &ep, &n, child_fd, assigned_client, &length, "", NULL);
                if (f == NULL) {
                    std::cout << "Child process exiting" << endl;
                    close(child_fd);
                    exit(1);
                }
                dt.downloadFile(f, true, &assigned_client, &length, &assigned_client, &length);
                fclose(f);
                break;
            }
            default:
                ep.setErrorCode(ILLEGAL_OPERATION);
                sendPacket(&n, child_fd, &ep, assigned_client, length, NULL, NULL);
                std::cout << "Unknown packet opcode" << endl;
                break;
        }

        std::cout << "Child process exiting" << endl;
        close(child_fd);
        exit(0);
    }
}

int main(int argc, char** argv) {
    // map for client TIDs and child process ports
    auto config = Config(argc, argv);
    int fd;                    // an incoming socket descriptor
    struct sockaddr_in server; // server's address structure
    ssize_t n;
    char buffer[1024];       // receiving buffer
    struct sockaddr_in client; // client's address structure
    socklen_t length;
    ERRORPacket ep(NOT_DEFINED);
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
            std::cout << "Creating new child process for client port: " << client_port << endl;
            int child_pid = 0;
            child_main(&child_pid, config, client, server, buffer, n);
            if (child_pid != -1){
                std::cout << "Child process created: " << child_pid << endl;
                client_process_port[client_port] = child_pid;
            }
        } 
        else {
            // child process was already created yet client still sends a packet to the server instead of to the child process
            // send back error
            std::cout << "Child process already running for client port: " << client_port << endl;
            ep.setErrorCode(ILLEGAL_OPERATION);
            sendPacket(&n, fd, &ep, client, length, NULL, NULL);
        }
    }
    printf("* Closing the socket\n");
    close(fd);

    return 0;
}
