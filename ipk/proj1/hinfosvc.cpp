#include <iostream>
#include <string>
#include <string_view>
#include <functional>
#include <unistd.h>
#include <cstdio>
#include <climits>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "HttpStatusCodes_Cpp.h"

using namespace std;

// https://stackoverflow.com/a/254j8212/8354587
std::string replace_all(std::string str, const std::string &remove, const std::string &insert)
{
    std::string::size_type pos = 0;
    while ((pos = str.find(remove, pos)) != std::string::npos) {
        str.replace(pos, remove.size(), insert);
        pos++;
    }

    return str;
}

string render_template(string templatestr, const vector<pair<string, string>>& replacements){
    for(const auto& replace : replacements) {
        templatestr = replace_all(templatestr, replace.first, replace.second);
    }
    return templatestr;
}

typedef struct handlerT{
    std::string path;
    function<std::string(void)> execute;
} handlerT;


string get_cpu_model(){
    FILE *cpuinfo = popen("grep '^model name' /proc/cpuinfo | uniq", "r");
    if (cpuinfo == nullptr)
        return "FAILED";
    char line[HOST_NAME_MAX];
    if (fgets(line, HOST_NAME_MAX, cpuinfo) != nullptr) {
        auto ls = string(line);
        return ls.substr(ls.find_first_of(':') + 2);
    }
    else
        return "FAILED";
}

string hostname_handler() {
    char hostname[255];
    gethostname(hostname, 255);
    return {hostname};
}

string load_handler() {
    return "LOAD";
}

string extract_path_from_request(const string& buffer){
    auto path = buffer.substr(buffer.find_first_of('/'));  // GET /path/ HTTP... -> /path/ HTTP...
    path = path.substr(0, path.find_first_of(' '));  // /path/ HTTP... -> /path/
    return path;
}

string get_response_for_request(const string& buffer, const vector<handlerT>& body_handlers){
    string code = "404";
    string status = "Not Found";
    string body;

    auto path = extract_path_from_request(buffer);
    for(const handlerT& handler : body_handlers) {
        if (handler.path == path) {
            try { body = handler.execute(); }
            catch (int error_code){
                code = to_string(error_code);
                status = HttpStatus::reasonPhrase(error_code);
                break;
            }

            code = "200";
            status = "OK";
        }
    }

    vector<pair<string, string>> replacements = {
            {"{VERSION}", "HTTP/1.1"},
            {"{CODE}", code},
            {"{STATUS}", status},
            {"{CONTENT TYPE}", "text/plain"},
            {"{BODY}", body},
    };

    auto response_template =
            "{VERSION} {CODE} {STATUS} \r\n"
            "Content-Type: {CONTENT TYPE}\r\n\r\n"
            "{BODY}";

    return render_template(response_template, replacements);
}

int main(int argc, char **argv) {
    auto cpumodel = get_cpu_model();

    vector<handlerT> handlers = {
            {.path="/hostname", .execute=hostname_handler},
            {.path="/cpu-name", .execute=[cpumodel](){return cpumodel;}},
            {.path="/load", .execute=load_handler},
    };

    int new_socket;
    //initialize socket
    int server_fd = socket(AF_INET,  // ip
                           SOCK_STREAM, // virtual circuit service
                           0  // no variations in protocol -> 0
                           );
    if (server_fd < 0) {
        cerr << "cannot create socket" << endl;
        exit(EXIT_FAILURE);
    }

    const long PORT = (argc > 1 ) ? strtol(argv[1], nullptr, 10) : 8080;

    struct sockaddr_in address = {
            .sin_family = AF_INET,  // same as socket af
            .sin_port = htons(PORT),  // specified port
            .sin_addr = {htons(INADDR_ANY)}, // any net interface -> 0.0.0.0
    };
    socklen_t addrlen = sizeof(address);
    if (bind(server_fd,(struct sockaddr *) &address,sizeof(address)) < 0)
    {
        cerr << "bind failed" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        cout << "failed in listen" << endl;
        exit(EXIT_FAILURE);
    }

    int pid;
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, &addrlen)) < 0) {
            cout << "failed in accept" << endl;
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid < 0) {
            cerr << "failed in fork" << endl;
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            char buffer[30000];
            auto nread = read(new_socket, buffer, 30000);
            cout << buffer << endl;
            string response = get_response_for_request(buffer, handlers);
            write(new_socket, response.c_str(), response.length());
            close(new_socket);
            exit(0);
        }
        else {
            cout << "parent created child with pid: " << pid << endl;
            close(new_socket);
        }
    }
    return 0;
}
