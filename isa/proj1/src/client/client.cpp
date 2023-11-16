//
// Created by Roland Schulz on 15.10.2023.
//
#include <iostream>
#include <map>
#include <string>

#include "shared/error_codes.h"
#include "shared/WRQPacket.hpp"
#include "shared/RRQPacket.hpp"
#include "shared/PacketFactory.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <err.h>
#include <arpa/inet.h>

using namespace std;

void getHelp(){
    auto help = "\
tftp-client -h hostname [-p dest_port] [-f filepath] -t dest_filepath \n\
    -h IP adresa/doménový název vzdáleného serveru \n\
    -p port vzdáleného serveru, pokud není specifikován předpokládá se výchozí dle specifikace (69)\n\
    -f cesta ke stahovanému souboru na serveru (download), pokud není specifikován používá se obsah stdin (upload) \n\
    -t cesta, pod kterou bude soubor na vzdáleném serveru/lokálně uložen";
    cout << help << endl;
}

class Config {
    public:
        Config(int argc, char** argv){
            auto args = Config::parseArgs(argc, argv);
            this->hostname = args["hostname"];
            this->dest_port = stoi(args["dest_port"]);
            this->filepath = args["filepath"];
            this->dest_filepath = args["dest_filepath"];
        }
        string hostname;
        int dest_port;
        string filepath;
        string dest_filepath;
        
        string toString(){
            return "hostname: " + this->hostname + ", dest_port: " + to_string(this->dest_port) + ", filepath: " + this->filepath + ", dest_filepath: " + this->dest_filepath;
        }

    private:
        map<string, string> parseArgs(int argc, char** argv){
            map<string, string> args = {
                {"hostname", ""},
                {"dest_port", "69"},
                {"filepath", ""},
                {"dest_filepath", ""}
            };

            for(int i = 1; i < argc; i++){
                string arg = argv[i];
                if(arg == "-h"){
                    args["hostname"] = argv[++i];
                }else if(arg == "-p"){
                    args["dest_port"] = argv[++i];
                }else if(arg == "-f"){
                    args["filepath"] = argv[++i];
                }else if(arg == "-t"){
                    args["dest_filepath"] = argv[++i];
                }else{
                    cout << "Neznámý parametr: " << arg << endl;
                    getHelp();
                    exit(UNKNOWN_ARG);
                }
            }

            // postconditions
            if(args["hostname"] == ""){
                cout << "Chybí parametr -h" << endl;
                getHelp();
                exit(NE_ARGS);
            }
            if(args["dest_filepath"] == ""){
                cout << "Chybí parametr -t" << endl;
                getHelp();
                exit(NE_ARGS);
            }

            return args;
        }
};

/**
Initial Connection Protocol for reading a file

   1. Host  A  sends  a  "RRQ"  to  host  B  with  source= A's TID,
      destination= 69.

   2. Host B sends a "DATA" (with block number= 1) to host  A  with
      source= B's TID, destination= A's TID.
 */

void printBytes(char* bytes, size_t length){
    for(int i = 0; i < length; i++){
        cout << hex << (int)bytes[i] << " ";
    }
    cout << endl;
}

int main(int argc, char** argv){
    auto config = Config(argc, argv);
    int sock; // socket descriptor
    int i;
    struct sockaddr_in assigned_server_process, from;
    struct sockaddr_in server; // address structures of the server and the client
    struct hostent *servent;         // network host entry required by gethostbyname()
    char buffer[1024];

    memset(&server, 0, sizeof(server)); // erase the server structure
    server.sin_family = AF_INET;

    // make DNS resolution of the first parameter using gethostbyname()
    if ((servent = gethostbyname(config.hostname.c_str())) == NULL) // check the first parameter
        errx(1, "gethostbyname() failed\n");

    // copy the first parameter to the server.sin_addr structure
    memcpy(&server.sin_addr, servent->h_addr, servent->h_length);

    server.sin_port = htons(config.dest_port); // server port (network byte order)

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) // create a client socket
        err(1, "socket() failed\n");

    printf("* Client socket created\n");

    WRQPacket* packet = new WRQPacket("test.txt", "netascii");
    // send data to the server
    //while ((msg_size = read(STDIN_FILENO, buffer, 1024)) > 0) {
    assigned_server_process = server;
    while (1) { 
        i = sendto(sock, packet->toByteStream(), packet->getLength(), 0, (const sockaddr*)& assigned_server_process, sizeof(server)); // send data to the server
        if (i == -1)                         // check if data was sent correctly
            err(1, "send() failed");
        else if (i != packet->getLength())
            err(1, "send(): buffer written partially");

        // read the answer from the server
        socklen_t length = sizeof(assigned_server_process);
        if ((i = recvfrom(sock, buffer, 1024, 0, (sockaddr*) &assigned_server_process, &length)) == -1) {
            err(1, "recv() failed");
        }
        else if (i > 0) {
            // port of assigned server process
            int port = ntohs(assigned_server_process.sin_port);
            printf("* UDP packet received from %s, port %d\n", inet_ntoa(assigned_server_process.sin_addr), port);
            Packet* returnPacket = PacketFactory::createPacket(buffer, i, WRQPacket::maxSizeBytes());
            if (returnPacket == NULL) {
                printf("* Packet is NULL\n");
                cout << "Packet bytes: " << buffer << endl;
                continue;
            }
            printf("* Packet opcode: %d\n", returnPacket->getOpcode());
            printf("* Packet length: %zu\n", returnPacket->getLength());
            cout << "Packet bytes: " << returnPacket->toByteStream() << endl;
            printf("%.*s", i, buffer); // print the answer
        }
    }

    close(sock);
    printf("* Closing the client socket ...\n");
    return 0;
}