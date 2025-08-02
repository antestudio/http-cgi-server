#include <sys/socket.h>
#include <unistd.h>
#include "net/socket.hpp"


void Socket::check_status(int status, std::string caller) const {
  if (status < 0) {
    throw Socket::socket_error(caller + strerror(errno), errno);
  }
}


void Socket::check_socket(void) const {
  if (socket == SOCKET_CLOSED) {
    throw Socket::socket_error("Socket was closed", -1);
  }
}


Socket::Socket(int new_socket): socket(new_socket) {}


Socket::Socket(int domain, int type, int protocol) {
  socket = ::socket(domain, type, protocol);
  check_status(socket, "socket(): ");
}


Socket::~Socket(void) noexcept {
  close();
}


Socket::Socket(Socket&& temp): socket(SOCKET_CLOSED) {
  std::swap(socket, temp.socket);
}


Socket& Socket::operator=(Socket&& temp) {
  close();
  std::swap(socket, temp.socket);
  return *this;
}


ssize_t Socket::send(const void* buffer, size_t length, int flags) const {
  check_socket();
  int status = ::send(socket, buffer, length, flags);
  check_status(status, "send(): ");
  return status;
}


ssize_t Socket::recv(void* buffer, size_t length, int flags) const {
  check_socket();
  int status = ::recv(socket, buffer, length, flags);
  check_status(status, "recv(): ");
  return status;
}


void Socket::close(void) {
  if (socket >= 0) ::close(socket);
  socket = SOCKET_CLOSED;
}