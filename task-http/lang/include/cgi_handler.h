#ifndef CGI_HANDLER_H
#define CGI_HANDLER_H

#include <string>
#include <unordered_map>

class CgiHandler {
public:
    static std::string handleRequest(
        const std::string& script,
        const std::unordered_map<std::string, std::string>& env,
        const std::string& inputData = "");
    
private:
    static std::string generateHttpResponse(const std::string& content);
    static std::string htmlEscape(const std::string& input);
};

#endif // CGI_HANDLER_H