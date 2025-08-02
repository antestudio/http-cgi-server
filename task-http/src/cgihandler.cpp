#include "config.hpp"
#include "net/http/request.hpp"
#include "net/http/response.hpp"
#include "net/http/status.hpp"
#include "cgihandler.hpp"

#include <filesystem>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

static std::string optional(
  const std::map<std::string, std::string>& map,
  std::string_view key
) {
  try {
    return map.at(key.data());
  } catch (std::out_of_range) {
    return "";
  }
}


HttpResponse handle_cgi_request(
  const HttpRequest& request,
  const Socket& socket
) {
  /* Harvest environment variables */
  std::map<std::string, std::string> envvars;
  envvars.insert({"SCRIPT_NAME",        request.getURI()});
  envvars.insert({"DOCUMENT_ROOT",      std::filesystem::current_path()});
  envvars.insert({"SCRIPT_FILENAME",    envvars["DOCUMENT_ROOT"] + envvars["SCRIPT_NAME"]});

  std::string cgipath = envvars["SCRIPT_FILENAME"]; // remove prefix slash
  if (! std::filesystem::exists(cgipath)) {
    return HttpResponse(NOT_FOUND, "CGI script not found");
  }

  /* Continue gathering envvars */
  envvars.insert({"CONTENT_TYPE",       "text/plain"});
  envvars.insert({"GATEWAY_INTERFACE",  "CGI/1.1"});
  envvars.insert({"SERVER_PORT",        std::to_string(DEFAULT_PORT)});
  envvars.insert({"SERVER_PROTOCOL",    "HTTP/1.0"});
  envvars.insert({"SERVER_SOFTWARE",    SERVER_NAME});
  envvars.insert({"SERVER_NAME",        "localhost"});
  envvars.insert({"HTTP_REFERER",       optional(request.getHeaders(), "Referer")});
  envvars.insert({"HTTP_USER_AGENT",    optional(request.getHeaders(), "User-Agent")});

  sockaddr_in peer = socket.getpeername<sockaddr_in>();
  envvars.insert({"REMOTE_PORT",        std::to_string(peer.sin_port)});
  envvars.insert({"REMOTE_ADDR",        inet_ntoa(peer.sin_addr)});

  /* Prepare for CGI script execution */
  int pipefd[2];
  pipe(pipefd);

  pid_t pid = fork();
  if (pid == 0) {
    /* I will exec CGI */
    close(pipefd[0]);
    dup2(pipefd[1], 1);

    for (auto env: envvars) {
      setenv(env.first.c_str(), env.second.c_str(), 1 /* overwrite */);
    }

    int status = execl(cgipath.c_str(), cgipath.c_str(), NULL);
    return HttpResponse(
      INTERNAL_ERROR,
      std::format("Internal error: execl('{}') = {}", cgipath.c_str(), status)
    );
  } else if (pid < 0) {
    close(pipefd[0]);
    close(pipefd[1]);
    return HttpResponse(
      SERVICE_UNAVAILABLE,
      std::format("Unavailable: fork() = {}", pid)
    );
  } else {
    /* I will wait for CGI to finish */
    close(pipefd[1]);
    waitpid(pid, NULL, 0);

    /* Read CGI response from pipe */
    std::string cgi_response;
    char buf[1024];
    ssize_t len = 1024;
    while ((len = read(pipefd[0], buf, len)) > 0) {
      cgi_response.append(buf, len);
    }
    close(pipefd[0]);

    return HttpResponse(cgi_response);
  }
}