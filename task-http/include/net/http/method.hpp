#pragma once
#ifndef _NET_HTTP_METHOD_HPP_
#define _NET_HTTP_METHOD_HPP_

#include <stdexcept>
#include <ostream>

struct Method {

  enum Code: char {
    GET,
    HEAD
  };
  Code method;

  Method(std::string name);
  operator std::string(void) const;
  bool operator==(Code c) const;

  friend std::ostream& operator<<(std::ostream&, const Method& method);

  struct unknown_method: std::invalid_argument {
    unknown_method(std::string name);
  };
};

#endif//_NET_HTTP_METHOD_HPP_