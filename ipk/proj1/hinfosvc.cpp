#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <unistd.h>
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
#define MAX_BACKLOG 5

string replaceEvery(string str, const string &remove, const string &insert) {
    string::size_type pos = 0;
    while ((pos = str.find(remove, pos)) != string::npos)
        str.replace(pos++, remove.size(), insert);

    return str;
}

string renderTemplate(string templateStr, const vector<pair<string, string>>& replacements){
    for(const auto& replace : replacements) {
        templateStr = replaceEvery(templateStr, replace.first, replace.second);
    }
    return templateStr;
}

typedef struct handlerT{
    std::string path;
    function<std::string(void)> execute;
} handlerT;

string getCPUModel(){
    FILE *cpuInfo = popen("grep '^model name' /proc/cpuinfo | uniq", "r");
    if (cpuInfo == nullptr)
        return "FAILED";
    char line[HOST_NAME_MAX];
    string returnValue;
    if (fgets(line, HOST_NAME_MAX, cpuInfo) != nullptr) {
        auto ls = string(line);
        returnValue = ls.substr(ls.find_first_of(':') + 2);
    }
    else
        returnValue = "FAILED";
    fclose(cpuInfo);
    return returnValue;
}

string hostnameRequestHandler() {
    char hostname[255];
    gethostname(hostname, 255);
    return {hostname};
}

vector<int> getProcStatTimes(){
    ifstream fileProcStat("/proc/stat");
    fileProcStat.ignore(5, ' ');
    vector<int> times;
    for (int time; fileProcStat >> time; times.push_back(time));  // convert strings to size_ts and push to times

    return times;
}

int getCPUTimes(int& idleTime, int& totalTime){
    vector<int> cpuTimes = getProcStatTimes();
    if (cpuTimes.size() < 4)
        return 0;
    int tempIdleTime = cpuTimes[3];
    idleTime = tempIdleTime;
    totalTime = accumulate(cpuTimes.begin(), cpuTimes.end(), 0);
    return 1;
}

float getCPUUtilization(int over_seconds){
    size_t previousIdleTime=0, previousTotalTime=0;
    float utilization = 0;
    int idleTime, totalTime;
    for (int i = 0; i < over_seconds; i+=getCPUTimes(idleTime, totalTime)) {
        const float idleTimeDelta = idleTime - previousIdleTime;
        const float totalTimeDelta = totalTime - previousTotalTime;
        utilization = 100.0 * (1- idleTimeDelta / totalTimeDelta);
        previousIdleTime = idleTime;
        previousTotalTime = totalTime;
        sleep(1);
    }
    return utilization;
}

string loadRequestHandler() {
    string utilization = to_string(getCPUUtilization(5));
    return utilization.substr(0, utilization.find('.')) + "%";
}

string getPathFromRequest(const string& buffer){
    auto path = buffer.substr(buffer.find_first_of('/'));  // GET /path/ HTTP... -> /path/ HTTP...
    path = path.substr(0, path.find_first_of(' '));  // /path/ HTTP... -> /path/
    return path;
}

string handleRequest(const string& buffer, const vector<handlerT>& bodyHandlers){
    string code = "404";
    string status = "Not Found";
    string body;

    auto path = getPathFromRequest(buffer);
    for(const handlerT& handler : bodyHandlers) {
        if (handler.path == path) {
            try { body = handler.execute(); }
            catch (int errorCode){
                code = to_string(errorCode);
                status = HttpStatus::reasonPhrase(errorCode);
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
            {"{CONTENT LENGTH}", to_string(body.size())},
            {"{CONNECTION}", "close"}
    };

    auto response_template =
            "{VERSION} {CODE} {STATUS} \r\n"
            "Content-Type: {CONTENT TYPE}\r\n"
            "Content-Length: {CONTENT LENGTH}\r\n"
            "Connection: {CONNECTION}\r\n"
            "\r\n"
            "{BODY}";

    return renderTemplate(response_template, replacements);
}

vector<int> children;
int serverFD;

void cleanup(){
    cout << "cleaning up!" << endl;
    for (auto child: children) {
        kill(child, SIGTERM);
    }
    close(serverFD);
}

void SIGINTHandler(int s){
    cleanup();
    exit(1);
}

void serveHttp(int port, vector<handlerT> handlers, int maxBacklog = MAX_BACKLOG, int maxBytesPerRequest = MAX_REQUEST_BYTES){
    int newSocket;
    //initialize socket
    serverFD = socket(AF_INET,     // ip
                      SOCK_STREAM, // virtual circuit service
                      0            // no variations in protocol -> 0
    );
    if (serverFD < 0) {
        cerr << "cannot create socket" << endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address = {
            .sin_family = AF_INET,  // same as socket af
            .sin_port = htons(port),  // specified port
            .sin_addr = {htons(INADDR_ANY)}, // any net interface -> 0.0.0.0
    };

    socklen_t addressLen = sizeof(address);
    if (bind(serverFD,(struct sockaddr *) &address,sizeof(address)) < 0) {
        cerr << "bind failed on port "<< port << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serverFD, maxBacklog) < 0) {
        cout << "failed in listen" << endl;
        exit(EXIT_FAILURE);
    }

    int pid;
    while (true) {
        if ((newSocket = accept(serverFD, (struct sockaddr *) &address, &addressLen)) < 0) {
            cout << "failed in accept" << endl;
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid < 0) {
            cerr << "failed in fork" << endl;
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            char buffer[maxBytesPerRequest];
            read(newSocket, buffer, sizeof(buffer));
            string response = handleRequest(buffer, handlers);
            write(newSocket, response.c_str(), response.length());
            close(newSocket);
            exit(0);
        }
        else {
            close(newSocket);
            children.push_back(pid);
        }
    }
}

int main(int argc, char **argv) {
    signal(SIGINT, SIGINTHandler);

    if (argc < 1){
        cerr << "port not specified" << endl;
        exit(EXIT_FAILURE);
    }
    const long port = strtol(argv[1], nullptr, 10);

    auto cpuModel = getCPUModel();

    vector<handlerT> handlers = {
            {.path="/hostname", .execute=hostnameRequestHandler},
            {.path="/cpu-name", .execute=[cpuModel](){return cpuModel;}},
            {.path="/load", .execute=loadRequestHandler},
    };

    serveHttp(port, handlers);
}
