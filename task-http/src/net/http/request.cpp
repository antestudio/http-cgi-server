#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include "net/http/request.hpp"


static std::pair<std::string, char> scroll(std::istream& ins, std::string_view delims) {
  std::string part;
  char c;
  bool prefix = true;

  while (ins) {
    c = ins.get();

    if (prefix && std::isblank(c)) {
      // skip whitespace before string
      continue;
    } else {
      // non-blank - don't skip whitespace anymore
      prefix = false;
    }

    for (auto delim: delims) {
      if (c == delim && !prefix) {
        return std::make_pair(part, c);
      }
    }

    part += c;
  }

  return std::make_pair(part, c);
}


HttpRequest::HttpRequest(std::string_view string):
  HttpMessage(string),
  method("GET") // is changed later
{
  std::istringstream title(HttpMessage::getTitle().begin());
  std::string piece;

  title >> piece;
  setMethod(piece);

  // Handle parameters after URL
  std::pair<std::string, char> pair = scroll(title, "? ");
  setURI(pair.first);

  if (pair.second == '?') {
    do {
      pair = scroll(title, "=& ");
      switch (pair.second) {
        case ' ':
        case '&':
          if (pair.first.size() > 0)
            setParam(pair.first, "");
          break;
        case '=':
          std::string key = pair.first;
          pair = scroll(title, "& ");
          setParam(key, pair.first);
      }
    } while (pair.second == '&');
  }

  title >> piece;
  setVersion(piece);
}


void HttpRequest::updateTitle(void) {
  std::stringstream builder;
  builder << getMethod() << ' ' << getURI() << ' ' << getVersion();
  HttpMessage::setTitle(builder.str());
}


Method HttpRequest::getMethod(void) const {
  return method;
}


void HttpRequest::setMethod(std::string aName) {
  method = aName;
  updateTitle();
}


std::string HttpRequest::getURI(void) const {
  return uri;
}


void HttpRequest::setURI(std::string aURI) {
  uri = aURI;
  updateTitle();
}


std::string HttpRequest::getVersion(void) const {
  return version;
}


void HttpRequest::setVersion(std::string aVersion) {
  version = aVersion;
  updateTitle();
}


std::optional<std::string> HttpRequest::getParam(std::string_view key) const {
  try {
    return params.at(key.data());
  } catch (std::out_of_range not_found) {
    return std::nullopt;
  }
}


std::map<std::string, std::string> HttpRequest::listParams(void) const {
  return params;
}


void HttpRequest::setParam(std::string_view key, std::string_view value) {
  params.insert_or_assign(key.data(), value);
}
