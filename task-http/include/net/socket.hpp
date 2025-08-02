#pragma once
#ifndef _NET_SOCKET_HPP_
#define _NET_SOCKET_HPP_

#include <stdexcept>
#include <string>
#include <sys/socket.h>

#define SOCKET_CLOSED -1

class Socket {

protected:

  int socket;

  void check_status(int status, const std::string prefix = nullptr) const;
  void check_socket(void) const;

public:

  Socket(int socket_fd);

  Socket(int domain, int type, int protocol);
  ~Socket(void) noexcept;

  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  Socket(Socket&&);
  Socket& operator=(Socket&&);

  ssize_t send(const void* buffer, size_t length, int flags) const;
  ssize_t recv(void* buffer, size_t length, int flags) const;

  void close(void);

  template<typename sockaddr_struc>
  sockaddr_struc getpeername(void) const {
    sockaddr_struc sockaddr;
    socklen_t sockaddr_len = sizeof(sockaddr_struc);
    int status = ::getpeername(socket, (struct sockaddr*) &sockaddr, &sockaddr_len);
    check_status(status, "getpeername(): ");
    return sockaddr;
  }

  /** Socket exception type **/
  struct socket_error: public std::logic_error {
    int error_code;
    socket_error(std::string what, int code):
      std::logic_error(what),
      error_code(code)
    {}
  };

};

#endif//_NET_SOCKET_HPP_
