#pragma once
#ifndef _NET_HTTP_RESPONSE_HPP_
#define _NET_HTTP_RESPONSE_HPP_

#include "net/http/status.hpp"
#include "net/http/message.hpp"

class HttpResponse: public HttpMessage {

  /* Replace title with "version code comment" */
  std::string version;
  Status      status;
  std::string comment;

  bool is_raw = false;

  void updateTitle(void);

public:

  #define HTTP_VERSION "HTTP/1.0"

  HttpResponse(Status status = OK, std::string comment = "OK", std::string version = HTTP_VERSION);
  explicit HttpResponse(std::string_view raw);

  std::string getVersion(void) const;
  void setVersion(std::string aVersion);

  std::string getComment(void) const;
  void setComment(std::string aComment);

  Status getStatus(void) const;
  void setStatus(Status aStatus);

  /* Remove HttpMessage's functions in favor of method, URI and version */
  std::string getTitle(void) const = delete;
  void setTitle(std::string_view aTitle) = delete;

  /* Serialize */
  std::string toString(void) const;
};

#endif//_NET_HTTP_RESPONSE_HPP_