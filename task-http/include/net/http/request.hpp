#pragma once
#ifndef _NET_HTTP_REQUEST_HPP_
#define _NET_HTTP_REQUEST_HPP_


#include "net/http/method.hpp"
#include "net/http/message.hpp"

class HttpRequest: public HttpMessage {

  Method method;
  std::string uri;
  std::string version;
  std::map<std::string, std::string> params;

  void updateTitle(void);

public:
  HttpRequest(std::string_view string);

  /* Remove HttpMessage's functions in favor of method, URI and version */
  std::string getTitle(void) const = delete;
  void setTitle(std::string_view aTitle) = delete;

  Method getMethod(void) const;
  void setMethod(std::string aName);

  std::string getURI(void) const;
  void setURI(std::string aURI);

  std::string getVersion(void) const;
  void setVersion(std::string aVersion);

  std::optional<std::string> getParam(std::string_view key) const;
  std::map<std::string, std::string> listParams(void) const;
  void setParam(std::string_view key, std::string_view value);
};

#endif//_NET_HTTP_REQUEST_HPP_