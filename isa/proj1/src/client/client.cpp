//
// Created by Roland Schulz on 15.10.2023.
//
#include <iostream>
#include <map>
#include <string>

#include "shared/error_codes.h"
#include "shared/WRQPacket.h"
#include "shared/RRQPacket.h"

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
    WRQPacket WRQ(config.dest_filepath, "netascii");
    cout << "WRQ" << endl;
    cout << WRQ.getFilename() << endl;
    cout << WRQ.getMode() << endl;
    cout << WRQ.getOpcode() << endl;
    auto bytes = WRQ.toByteStream();
    printBytes(bytes, WRQ.getLength());
    WRQPacket WRQ2(bytes);
    cout << "WRQ2" << endl;

    cout << WRQ2.getFilename() << endl;
    cout << WRQ2.getMode() << endl;
    cout << WRQ2.getOpcode() << endl;
    printBytes(WRQ2.toByteStream(), WRQ2.getLength());
    return 0;
}