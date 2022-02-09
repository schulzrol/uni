#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <unistd.h>
#include <cstdio>
#include <climits>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <fstream>
#include "HttpStatusCodes_Cpp.h"

using namespace std;

// https://stackoverflow.com/a/2659995/8354587
#define MAX_REQUEST_BYTES 8000
#define BACKLOG_MAX 5

string replace_all(string str, const string &remove, const string &insert) {
    string::size_type pos = 0;
    while ((pos = str.find(remove, pos)) != string::npos)
        str.replace(pos++, remove.size(), insert);

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
    string retval;
    if (fgets(line, HOST_NAME_MAX, cpuinfo) != nullptr) {
        auto ls = string(line);
        retval = ls.substr(ls.find_first_of(':') + 2);
    }
    else
        retval = "FAILED";
    fclose(cpuinfo);
    return retval;
}

string hostname_handler() {
    char hostname[255];
    gethostname(hostname, 255);
    return {hostname};
}

vector<int> get_proc_stat_times(){
    ifstream proc_stat("/proc/stat");
    proc_stat.ignore(5, ' ');
    vector<int> times;
    for (int time; proc_stat >> time; times.push_back(time));  // convert strings to size_ts and push to times

    return times;
}

int get_cpu_times(int& idle_time, int& total_time){
    vector<int> cpu_times = get_proc_stat_times();
    for_each(cpu_times.begin(), cpu_times.end(), [](int t) { cout << t << " ";});
    cout << endl;
    if (cpu_times.size() < 4)
        return 0;
    int temp_idle = cpu_times[3];
    idle_time = temp_idle;
    total_time = accumulate(cpu_times.begin(), cpu_times.end(), 0);
    return 1;
}

float get_cpu_utilization(int over_seconds){
    size_t previous_idle_time=0, previous_total_time=0;
    float utilization = 0;
    int idle_time, total_time;
    for (int i = 0; i < over_seconds; i+=get_cpu_times(idle_time, total_time)) {
        const float idle_time_delta = idle_time - previous_idle_time;
        const float total_time_delta = total_time - previous_total_time;
        utilization = 100.0 * (1- idle_time_delta / total_time_delta);
        cout << utilization << endl;
        previous_idle_time = idle_time;
        previous_total_time = total_time;
        sleep(1);
    }
    return utilization;
}

string load_handler() {
    string utilization = to_string(get_cpu_utilization(5));
    return utilization.substr(0, utilization.find('.')) + "%";
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

vector<int> children;
int server_fd;

void cleanup(){
    cout << "cleaning up!" << endl;
    for (auto child: children) {
        kill(child, SIGTERM);
    }
    close(server_fd);
}

void sigint_handler(int s){
    cleanup();
    exit(1);
}

int main(int argc, char **argv) {
    signal(SIGINT, sigint_handler);

    if (argc < 1){
        cerr << "PORT not specified" << endl;
        exit(EXIT_FAILURE);
    }
    const long PORT = strtol(argv[1], nullptr, 10);

    auto cpumodel = get_cpu_model();

    vector<handlerT> handlers = {
            {.path="/hostname", .execute=hostname_handler},
            {.path="/cpu-name", .execute=[cpumodel](){return cpumodel;}},
            {.path="/load", .execute=load_handler},
    };

    int new_socket;
    //initialize socket
    server_fd = socket(AF_INET,  // ip
                           SOCK_STREAM, // virtual circuit service
                           0  // no variations in protocol -> 0
                           );
    if (server_fd < 0) {
        cerr << "cannot create socket" << endl;
        exit(EXIT_FAILURE);
    }


    struct sockaddr_in address = {
            .sin_family = AF_INET,  // same as socket af
            .sin_port = htons(PORT),  // specified port
            .sin_addr = {htons(INADDR_ANY)}, // any net interface -> 0.0.0.0
    };
    socklen_t addrlen = sizeof(address);
    if (bind(server_fd,(struct sockaddr *) &address,sizeof(address)) < 0) {
        cerr << "bind failed on port "<< PORT << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG_MAX) < 0) {
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
            char buffer[MAX_REQUEST_BYTES];
            read(new_socket, buffer, MAX_REQUEST_BYTES);
            string response = get_response_for_request(buffer, handlers);
            write(new_socket, response.c_str(), response.length());
            close(new_socket);
            exit(0);
        }
        else {
            close(new_socket);
            children.push_back(pid);
        }
    }
}
