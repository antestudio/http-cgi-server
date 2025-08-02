#include "cgi_handler.h"
#include "interpreter.h"
#include <sstream>
#include <unistd.h>

using namespace std;

string CgiHandler::htmlEscape(const string& input) {
    ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '&': oss << "&amp;"; break;
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&apos;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

string CgiHandler::handleRequest(
    const string& script,
    const unordered_map<string, string>& env,
    const string& inputData)
{

    // Устанавливаем переменные окружения
    for (const auto& var : env) {
        setenv(var.first.c_str(), var.second.c_str(), 1);
    }

    // Устанавливаем stdin данные если есть
    if (!inputData.empty()) {
        FILE* tmp = tmpfile();
        fwrite(inputData.data(), 1, inputData.size(), tmp);
        rewind(tmp);
        dup2(fileno(tmp), STDIN_FILENO);
    }

    ostringstream output;
    Interpreter interpreter([&output] (const string& str) {
        output << str;
    });

    try {
        interpreter.interpret(script);
        return generateHttpResponse(output.str());
    } catch (const exception& e) {
        string errorContent =
R"(<!DOCTYPE html>
<html>
<head>
    <meta encoding="utf-8">
    <title>Error</title>
</head>
<body>
    <h1>Error</h1>
    <pre>)" + htmlEscape(e.what()) + R"(</pre>
</body>
</html>)";
        return generateHttpResponse(errorContent);
    }
}

string CgiHandler::generateHttpResponse(const string& content) {
    ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: text/html\r\n"
             << "Content-Length: " << content.size() << "\r\n"
             << "\r\n"
             << content;
    return response.str();
}