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


/**
 * Replace every occurrence of \p remove in \p str with \p insert
 *
 * @param str template string, possibly containing \p remove
 * @param remove string to replace with \p insert
 * @param insert string used as value to replace \p remove with
 * @return \p str with every occurrence of \p remove replaced with \p insert
 */
string replaceEvery(string str, const string &remove, const string &insert) {
    string::size_type pos = 0;
    while ((pos = str.find(remove, pos)) != string::npos)
        str.replace(pos++, remove.size(), insert);

    return str;
}

/**
 * Fills in \p templateStr based on defined replacements
 *
 * @param templateStr string used as a template
 * @param replacements key->value pairs to place into \p templateStr
 * @return \p templateStr formatted with replacements from \p replacements
 */
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

/**
 * @returns CPU model as a string
 * @pre Requires /proc/cpuinfo, thus only Linux compatible
 */
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
    return replaceEvery(returnValue, "\n", "");
}

/**
 * @returns current hostname
 */
string hostnameRequestHandler() {
    char hostname[255];
    gethostname(hostname, 255);
    return {hostname};
}

/**
 * Get first line of numbers found in /proc/stat. Used for load estimation
 *
 * @returns first line of numbers found in /proc/stat
 */
vector<int> getProcStatTimes(){
    ifstream fileProcStat("/proc/stat");
    fileProcStat.ignore(5, ' ');
    vector<int> times;
    for (int time; fileProcStat >> time; times.push_back(time));  // convert strings to size_ts and push to times

    return times;
}

/**
 * Processes /proc/stat times into /p idleTime and /p totalTime
 *
 * @param[out] idleTime overall cpu idle
 * @param[out] totalTime sum of cpu overall times
 * @returns a 1 whether successfully computed next idle/total times, 0 otherwise (used for effective sampling in a loop)
 */
int getCPUTimes(int& idleTime, int& totalTime){
    vector<int> cpuTimes = getProcStatTimes();
    if (cpuTimes.size() < 4)
        return 0;
    int tempIdleTime = cpuTimes[3];
    idleTime = tempIdleTime;
    totalTime = accumulate(cpuTimes.begin(), cpuTimes.end(), 0);
    return 1;
}

/**
 * Get CPU utilization/load in percents sampled over \p over_seconds seconds
 *
 * Based on https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 *
 * @param over_seconds number of samples (each taken with a frequency of approximately 1Hz)
 * @return double specifying the CPU load/utilization over \p over_seconds seconds
 */
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

/**
 * @returns current CPU utilization/load sampled over 5 seconds with frequency of approximately 1Hz
 */
string loadRequestHandler() {
    string utilization = to_string(getCPUUtilization(5));
    return utilization.substr(0, utilization.find('.')) + "%";
}

/**
 * Retrieve path from GET request string
 *
 * @param request string containing the read GET request
 * @returns path from GET request string
 */
string getPathFromRequest(const string& request){
    auto path = request.substr(request.find_first_of('/'));  // GET /path/ HTTP... -> /path/ HTTP...
    path = path.substr(0, path.find_first_of(' '));  // /path/ HTTP... -> /path/
    return path;
}

/**
 *  Executes a corresponding handler for resource specified in \p request path and constructs response string with
 *  proper status codes, headers and body.
 *
 * @param request GET request string
 * @param bodyHandlers collection specifying server handling capabilities using (path)->(body for path) pairs
 * @returns full response string with body
 */
string handleRequest(const string& request, const vector<handlerT>& bodyHandlers){
    string code = "404";
    string status = "Not Found";
    string body;

    auto path = getPathFromRequest(request);
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

    auto responseTemplate =
            "{VERSION} {CODE} {STATUS} \r\n"
            "Content-Type: {CONTENT TYPE}\r\n"
            "Content-Length: {CONTENT LENGTH}\r\n"
            "Connection: {CONNECTION}\r\n"
            "\r\n"
            "{BODY}";

    return renderTemplate(responseTemplate, replacements);
}

vector<int> children;
int serverFD;

/**
 * cleanup job killing forked children and closing server socket
 */
void cleanup(){
    cout << "cleaning up!" << endl;
    for (auto child: children) {
        kill(child, SIGTERM);
    }
    close(serverFD);
}

/**
 *
 * @param s
 */
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

    /* Possible room for improvement: vector<string, void->string> to map<string, void->string>>,
    Cpp maps don't have default values (which bugged me), and I think the time saved using maps O(1) is negligible since
    net-latency and handling of requests is already pretty costly
    */
    vector<handlerT> handlers = {
            {.path="/hostname", .execute=hostnameRequestHandler},
            {.path="/cpu-name", .execute=[cpuModel](){return cpuModel;}},
            {.path="/load", .execute=loadRequestHandler},
    };

    serveHttp(port, handlers);
}
