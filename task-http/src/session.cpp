#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <new>
#include <string>
#include <unistd.h>
#include "server/session.hpp"
#include "cgihandler.hpp"
#include "net/http/request.hpp"
#include "net/http/response.hpp"
#include "net/http/status.hpp"
#include "config.hpp"


std::string get_date(auto time) {
  return std::format("{:%a, %d %b %Y %T} GMT", time);
}


HttpResponse process_request(const HttpRequest& request, const Socket& sock) {
  std::cout << request.getURI() << std::endl;
  if (request.getURI().starts_with("/cgi-bin")) {
    return handle_cgi_request(request, sock);
  }

  HttpResponse response(OK);

  std::string current = std::filesystem::current_path(), path = request.getURI();
  std::ifstream filestream(current + path);
  if (!filestream.is_open()) {
    return HttpResponse(NOT_FOUND, "Not found");
  }

  /* Body */
  if (request.getMethod() != Method::HEAD) {
    std::ostringstream contents;
    contents << filestream.rdbuf();
    response.setBody(contents.str());
  }

  /* Headers */
  response["Content-Length"] = std::to_string(response.getBody().size());
  response["Last-Modified"] = get_date(std::filesystem::last_write_time(current + path));
  response["Allow"] = "GET,HEAD";

  /* Content-Type */
  if (path.ends_with("html")) {
    response["Content-Type"] = "text/html";
  } else if (path.ends_with("jpg") || path.ends_with("jpeg")) {
    response["Content-Type"] = "image/jpeg";
  } else {
    response["Content-Type"] = "text/plain";
  }

  return response;
}


void session(Socket& socket) {
  std::string contents;
  bool running = true;

  while (running) {
    /* Read HTTP client */
    try {
      contents.clear();
      char buffer[1024];
      ssize_t length = 0;
      while ((length = socket.recv(buffer, sizeof(buffer), 0)) > 0) {
        contents.append(buffer, length);
        if (length < sizeof(buffer)) break;
      }
    } catch (Socket::socket_error e) {
      std::cerr << "Error occurred while recv() from client:" << e.what() << std::endl;
      return;
    }

    /* Handle HTTP requests */
    if (contents.size() == 0) return;

    HttpResponse response;
    try {
      HttpRequest request(contents);
      // std::cout << "Method: " << request.getMethod() << std::endl;
      // std::cout << "URI: " << request.getURI() << std::endl;
      // std::cout << "Version: " << request.getVersion() << std::endl;
      // for (auto kv: request.listParams()) {
      //   std::cout << "Param \"" << kv.first << "\" = \"" << kv.second << '"' << std::endl;
      // }

      response = process_request(request, socket);
    } catch (Method::unknown_method e) {
      response = HttpResponse(NOT_IMPLEMENTED, "Not implemented");
    } catch (std::bad_alloc) {
      response = HttpResponse(SERVICE_UNAVAILABLE, "Unavailable");
    } catch (...) {
      /* Probably a syntax error */
      response = HttpResponse(BAD_REQUEST, "Bad request");
    }


    /* Common headers */
    response["Date"] = get_date(std::chrono::system_clock::now());
    response["Content-Length"] = std::to_string(response.getBody().length());
    response["Server"] = SERVER_NAME;
    if (response["Content-Type"].empty()) {
      response["Content-Type"] = "text/plain";
    }


    std::string output = response.toString();
    socket.send(output.data(), output.size(), 0);
  }
}
