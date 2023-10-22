//
// Created by Roland Schulz on 15.10.2023.
//
#include <iostream>
#include <map>
#include <string>

#include "shared/error_codes.h"

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
                {"port", "69"},
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
    cout << config.toString() << endl;
    return 0;
}