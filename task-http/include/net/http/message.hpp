#pragma once
#ifndef _NET_HTTP_MESSAGE_HPP_
#define _NET_HTTP_MESSAGE_HPP_


#include <map>
#include <string>
#include <string_view>


class HttpMessage {

protected:
  std::string title;
  std::map<std::string, std::string> headers;
  std::string body;

  HttpMessage(void) = default;

public:
  HttpMessage(std::string_view string);

  std::string_view getTitle(void) const;
  void setTitle(std::string_view aTitle);

  std::string_view getBody(void) const;
  void setBody(std::string_view aBody);

  const std::map<std::string, std::string> getHeaders(void) const;
  std::string& operator[](std::string_view);

  /* Serialize */
  std::string toString(void) const;
};

#endif//_NET_HTTP_MESSAGE_HPP_