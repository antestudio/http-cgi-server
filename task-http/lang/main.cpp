#include "cgi_handler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>

using namespace std;

extern "C" {
    extern char **environ;
}

[[noreturn]] void usage(void) {
    std::cout << "usage: lang path-to-script" << std::endl;
    std::exit(-1);
}

int main(int argc, char* argv[]) {
    if (argc < 2) usage();

    // Extract environment variablse
    unordered_map<string, string> envvars;
    for (char **env = environ; *env != NULL; env++) {
        std::string envString(*env);
        size_t pos = envString.find('=');
        if (pos != std::string::npos) {
            std::string key = envString.substr(0, pos);
            std::string value = envString.substr(pos + 1);
            envvars.insert({key, value});
        }
    }

    std::ifstream script_file(argv[1]);
    std::string line;

    std::getline(script_file, line);
    // Remove shebang from script
    if (line.starts_with("#!")) {
        line.clear();
    }
    script_file.ignore();

    std::ostringstream rest;
    script_file >> rest.rdbuf();

    // Handle request
    std::string script = line + rest.str();
    string response = CgiHandler::handleRequest(script, envvars, "");
    cout << response;
    return 0;
}