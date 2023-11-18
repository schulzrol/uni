//
// Created by Roland Schulz on 15.10.2023.
//
#include <iostream>
#include <map>
#include <string>
#include <fstream>

#include "shared/error_codes.h"
#include "shared/WRQPacket.hpp"
#include "shared/RRQPacket.hpp"
#include "shared/PacketFactory.hpp"
#include "shared/DataTransfer.hpp"

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
    -f cesta ke stahovanému souboru na serveru (pro download), pokud není specifikován používá se obsah stdin (pro upload) \n\
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


void upload(int socket, string dest_filepath, struct sockaddr_in server, tftp_mode mode, unsigned int block_size = DEFAULT_BLOCK_SIZE_BYTES){
    ssize_t n;
    // Initiate connection
    // send write request to the server
    cout << "Sending WRQ" << endl;
    WRQPacket wrq(dest_filepath, mode);
    {
        n = sendto(socket, wrq.toByteStream().c_str(), wrq.getLength(), 0, (const sockaddr*)& server, sizeof(server)); // send data to the server
        if (!handleSendToReturn(n, wrq.getLength())){
            // TODO try again
            cout << "Error sending WRQ" << endl;
            return;
        }
    }
    DataTransfer dt(socket, mode, block_size);
    dt.uploadFile(stdin);
    cout << "Upload finished" << endl;
}

void download(int socket, string save_to_file, string file_on_remote, struct sockaddr_in server, tftp_mode mode, unsigned int block_size = DEFAULT_BLOCK_SIZE_BYTES){
    ssize_t n;
    // send read request to the server
    cout << "Sending RRQ" << endl;
    RRQPacket rrq(file_on_remote, mode);
    {
        n = sendto(socket, rrq.toByteStream().c_str(), rrq.getLength(), 0, (const sockaddr*)& server, sizeof(server)); // send data to the server
        if (!handleSendToReturn(n, rrq.getLength())){
            // TODO try again or timeout
            cout << "Error sending WRQ" << endl;
            return;
        }
    }

    FILE* f = fopen(save_to_file.c_str(), "wb");
    if (f == NULL) {
        cout << "Error opening file \'" << save_to_file << "\' for writing: " << strerror(errno) << endl;
    }
    DataTransfer dt(socket, mode, block_size);
    dt.downloadFile(f);
    fclose(f);
    cout << "Download finished" << endl;
}

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
    struct sockaddr_in server; // address structures of the server and the client
    struct hostent *servent;         // network host entry required by gethostbyname()
    tftp_mode default_mode = tftp_mode::octet;
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

    if (config.filepath.empty()){
        upload(sock, config.dest_filepath, server, default_mode);
    } else {
        download(sock, config.dest_filepath, config.filepath, server, default_mode);
    }

    close(sock);
    printf("* Closing the client socket ...\n");
    return 0;
}