#include <iostream>
#include <string>
#include <string_view>
#include <functional>
#include <unistd.h>
#include <cstdio>
#include <climits>
#include <list>

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

string replace_foreach(string str, const list<pair<string, string>>& replacements){
    for(const auto& replace : replacements) {
        str = replace_all(str, replace.first, replace.second);
    }
    return str;
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

int main(int argc, char **argv) {
  auto cpumodel = get_cpu_model();

  handlerT handlers[] = {
    {.path="/hostname", .execute=hostname_handler},
    {.path="/cpu-name", .execute=[cpumodel](){return cpumodel;}},
    {.path="/load", .execute=load_handler},
  };

  string path = (argc > 1) ? argv[1] : "hostname";

  string body = "not found";
  for(const handlerT& handler : handlers) {
    if (handler.path == path)
      body = handler.execute();
  }

  list<pair<std::string, std::string>> replacements = {
    {"{VERSION}", "HTTP/1.1"},
    {"{CODE}", "200"},
    {"{STATUS}", "OK"},
    {"{BODY}", body}
  };

  auto response_template = "{VERSION} {CODE} {STATUS} {BODY}";
  cout << replace_foreach(response_template, replacements) << endl;
  return 0;
}
