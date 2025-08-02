#include <sstream>
#include <string>
#include "net/http/message.hpp"


std::string_view HttpMessage::getTitle(void) const {
  return title;
}


void HttpMessage::setTitle(std::string_view aTitle) {
  title = aTitle;
}


std::string_view HttpMessage::getBody(void) const {
  return body;
}


void HttpMessage::setBody(std::string_view aBody) {
  body = aBody;
}


const std::map<std::string, std::string> HttpMessage::getHeaders(void) const {
  return headers;
}


std::string& HttpMessage::operator[](std::string_view e) {
  return headers[e.data()];
}


HttpMessage::HttpMessage(std::string_view string) {
  std::istringstream sin(string.data());

  std::string line;
  std::getline(sin, line);
  setTitle(line);

  std::getline(sin, line);
  std::string h_name, h_body;
  while (line.size() > 1) {
    std::istringstream line_sin = std::istringstream(line);
    line_sin >> h_name >> h_body;
    h_name.pop_back(); // remove ':'
    operator[](h_name) = h_body;

    std::getline(sin, line);
  }

  std::string body;
  std::getline(sin, body, '\0');
  setBody(body);
}


std::string HttpMessage::toString(void) const {
  std::stringstream result;

  /* Header */
  result << getTitle() << '\n';

  /* HTTP-headers */
  for (std::pair<std::string, std::string> header: headers) {
    result << header.first << ": " << header.second << '\n';
  }

  /* Body */
  result << '\n' << getBody();
  return result.str();
}
