#include <format>
#include <map>
#include <stdexcept>
#include "net/http/method.hpp"


static const std::map<std::string, Method::Code> methods {
  { "GET",  Method::GET   },
  { "HEAD", Method::HEAD  },
};


static Method::Code lut_method(std::string name) {
  auto result = methods.find(name);

  if (result == methods.end()) {
    throw Method::unknown_method(name);
  }

  return result->second;
}


Method::Method(std::string name):
  method(lut_method(name))
{}


bool Method::operator==(Method::Code code) const {
  return this->method == code;
}


Method::operator std::string(void) const {
  for (auto available: methods) {
    if (available.second == method) {
      return available.first;
    }
  }

  throw unknown_method(std::format("Unknown method: {}", (char) method));
}


std::ostream& operator<<(std::ostream& outs, const Method& method) {
  outs << (method.operator std::string());
  return outs;
}


Method::unknown_method::unknown_method(std::string name):
  std::invalid_argument(name)
{}
