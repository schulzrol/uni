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
    sockaddr_in assigned_server_process;
    FILE* input = stdin;
    // how to read from file
    const size_t buflen = block_size*2;
    char buffer[buflen];
    size_t msg_size;
    socklen_t length = sizeof(assigned_server_process);
    // send write request to the server
    cout << "Sending WRQ" << endl;
    WRQPacket wrq(dest_filepath, mode);
    {
        n = sendto(socket, wrq.toByteStream().c_str(), wrq.getLength(), 0, (const sockaddr*)& server, sizeof(server)); // send data to the server
        if (!handleSendToReturn(n, wrq.getLength())){
            // TODO try again or timeout
            cout << "Error sending WRQ" << endl;
            return;
        }
    }
    // TODO add check that TID is the same as when sending
    // wait for ACK with block number 0
    cout << "Waiting for ACK" << endl;
    {
        n = recvfrom(socket, buffer, buflen, 0,(sockaddr*) &assigned_server_process, &length);
        if (!handleRecvFromReturn(n)){
            // TODO try again or timeout
            cout << "Error receiving ACK" << endl;
            return;
        }
        try {
            ACKPacket ack = ACKPacket(buffer);
            if (ack.getBlockNumber() != 0){
                // TODO try again or timeout
                cout << "Invalid ACK block number" << endl;
                return;
            }
            cout << "Received ACK " << ack.getBlockNumber() << endl;
        } catch (runtime_error& e) {
            cout << e.what() << endl;
            return;
        }
    }

    // keep sending data packets until the end of the file
    cout << "Starting to send data packets" << endl;
    // TODO what if file is empty?
    unsigned short block_number = 1;
    while((msg_size = fread(buffer, 1, block_size, input)) > 0) {
        // send data packet
        DATAPacket datap(block_number, buffer, mode, msg_size);
        {
            if (datap.getLength() < block_size){
                cout << "Sending last DATAPacket with block number " << datap.getBlockNumber() << endl;
            } else {
                cout << "Sending DATAPacket with block number "<< datap.getBlockNumber() << endl;
            }
            n = sendto(socket, datap.toByteStream().c_str(), datap.getLength(), 0, (const sockaddr *)&assigned_server_process, length); // send data to the server
            if (!handleSendToReturn(n, datap.getLength())){
                // TODO try again or timeout
                cout << "Error sending DATAPacket" << endl;
                return;
            }
        }
        // expect ACK with block number block_number
        {
            if (datap.getLength() < block_size){
                cout << "Waiting for last ACK with block number " << datap.getBlockNumber() << endl;
            } else {
            cout << "Waiting for ACK with block number "<< datap.getBlockNumber() << endl;
            }
            n = recvfrom(socket, buffer, buflen, 0,(sockaddr*) &assigned_server_process, &length);
            if (!handleRecvFromReturn(n)){
                // TODO try again or timeout
                cout << "Error receiving ACK" << endl;
                return;
            }
            // TODO add check that TID is the same as when sending

            try {
                ACKPacket ack = ACKPacket(buffer);
                if (ack.getBlockNumber() != datap.getBlockNumber()){
                    // TODO try again or timeout
                    cout << "Invalid ACK block number" << endl;
                    return;
                }
                cout << "Received ACK " << ack.getBlockNumber() << endl;
            } catch (runtime_error& e) {
                cout << e.what() << endl;
                return;
            }
        }
        // get ready for next block
        cout << "Sending next block" << endl;
        block_number++;
    }

    if (ferror(input)){
        cout << "Error reading file" << endl;
        return;
    }
    cout << "File sent" << endl;
}


void download(int socket, string filepath, string dest_filepath, struct sockaddr_in server, tftp_mode mode, unsigned int block_size = DEFAULT_BLOCK_SIZE_BYTES){
    ssize_t n;
    sockaddr_in assigned_server_process;
    FILE* output = stdout;
    // how to read from file
    const size_t buflen = block_size*2;
    char buffer[buflen];
    socklen_t length = sizeof(assigned_server_process);
    // send write request to the server
    cout << "Sending RRQ" << endl;
    RRQPacket rrq(dest_filepath, mode);
    {
        n = sendto(socket, rrq.toByteStream().c_str(), rrq.getLength(), 0, (const sockaddr*)& server, sizeof(server)); // send data to the server
        if (!handleSendToReturn(n, rrq.getLength())){
            // TODO try again or timeout
            cout << "Error sending WRQ" << endl;
            return;
        }
    }
    // keep receiving data packets until their length is less than block_size
    bool receivedLastDataPacket = false;
    unsigned short block_number = 0;
    do {
        // Todo timeout
        // Receive packet
        {
            n = recvfrom(socket, buffer, buflen, 0,(sockaddr*) &assigned_server_process, &length);
            if (!handleRecvFromReturn(n)){
                // TODO try again or timeout
                cout << "Error receiving ACK" << endl;
                return;
            }
        }
        // TODO may also receive an error packet and timeout
        // expect data packet (TODO what if its not a data packet?)
        DATAPacket datap = DATAPacket(buffer, rrq.getModeEnum(), n-4); // -4 because of opcode and block number
        // Check the block number
        {
            if (datap.getBlockNumber() != block_number){
                // TODO try again or timeout
                cout << "Invalid DATAPacket block number" << endl;
                return;
            }
        }
        // Check the length of the received block of data
        {
            if (datap.getLength() < block_size){
                cout << "Received last DATAPacket with block number " << datap.getBlockNumber() << endl;
                receivedLastDataPacket = true;
            } else {
                cout << "Received DATAPacket with block number "<< datap.getBlockNumber() << endl;
            }
        }
        cout << "Writing to file" << endl;
        // Write data to file
        fwrite(datap.getData().c_str(), 1, datap.getData().size(), output);
        // Send back ACK
        ACKPacket ack = ACKPacket(block_number);
        {
            n = sendto(socket, ack.toByteStream().c_str(), ack.getLength(), 0, (struct sockaddr *)&assigned_server_process, length); // send the answer
            if (!handleSendToReturn(n, ack.getLength())){
                // TODO try again or timeout
                cout << "Error sending ACK" << endl;
                return;
            }
            cout << "Sent ACK with block number: " << ack.getBlockNumber() << endl;
        }
        // Get ready for next block
        block_number++;
    } while(!receivedLastDataPacket);
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
        upload(sock, config.dest_filepath, server, tftp_mode::octet);
    } else {
        // TODO download
        download(sock, config.filepath, config.dest_filepath, server, tftp_mode::octet);
    }

    close(sock);
    printf("* Closing the client socket ...\n");
    return 0;
}