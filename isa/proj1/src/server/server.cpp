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

#include "shared/error_codes.h"
#include "shared/definitions.h"
#include "shared/PacketFactory.hpp"

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

int main(int argc, char** argv){
    auto config = Config(argc, argv);
    int fd;                    // an incoming socket descriptor
    struct sockaddr_in server; // server's address structure
    size_t n, r;
    char buffer[1024];       // receiving buffer
    struct sockaddr_in client; // client's address structure
    socklen_t length;

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
        
        //auto receive_packet = PacketFactory::createPacket(buffer, n);
        //switch (receive_packet->getOpcode()){
        //    case 1:
        //        cout << "RRQ" << endl;
        //        break;
        //    case 2:
        //        cout << "WRQ" << endl;
        //        break;
        //    case 3:
        //        cout << "DATA" << endl;
        //        break;
        //    case 4:
        //        cout << "ACK" << endl;
        //        break;
        //    case 5:
        //        cout << "ERROR" << endl;
        //        break;
        //    default:
        //        cout << "Unknown opcode" << endl;
        //        break;
        //}
        cout << "Data: " << buffer << endl;
        r = sendto(fd, buffer, n, 0, (struct sockaddr *)&client, length); // send the answer

        if (r == -1)
            err(1, "sendto()");
        else if (r != n)
            errx(1, "sendto(): Buffer written just partially");
        else
            printf("* Data sent");
    }
    printf("* Closing the socket\n");
    close(fd);

    return 0;
}
