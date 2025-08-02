#pragma once
#include <sys/socket.h>
#ifndef _NET_SERVERSOCKET_HPP_
#define _NET_SERVERSOCKET_HPP_

#include "net/socket.hpp"

class ServerSocket: public Socket {

public:

  ServerSocket(int domain, int type, int protocol):
    Socket(domain, type, protocol)
  {}


  template<typename sockaddr_struc>
  void bind(const sockaddr_struc *address) const {
    check_socket();
    int status = ::bind(socket, (sockaddr*) address, sizeof(sockaddr_struc));
    check_status(status, "bind(): ");
  }


  void listen(int backlog) const {
    check_socket();
    int status = ::listen(socket, backlog);
    check_status(status, "listen(): ");
  }


  template<typename sockaddr_struc>
  Socket accept(sockaddr_struc *address) const {
    check_socket();
    int new_socket = ::accept(socket, address, sizeof(sockaddr_struc));
    check_status(new_socket, "accept(): ");
    return new_socket;
  }


  /** Specialization of accept() when client's address is needless **/
  Socket accept(nullptr_t addr) const {
    check_socket();
    int new_socket = ::accept(socket, NULL, 0);
    check_status(new_socket, "accept(null): ");
    return new_socket;
  }

};

#endif//_NET_SERVERSOCKET_HPP_