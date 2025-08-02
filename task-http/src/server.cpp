#include <iostream>
#include <csignal>
#include <unistd.h>
#include <netinet/in.h>

#include "config.hpp"
#include "server/session.hpp"
#include "net/serversocket.hpp"


static ServerSocket server(AF_INET, SOCK_STREAM, 0);
static const sockaddr_in address {
  .sin_family = AF_INET,
  .sin_len = sizeof(sockaddr_in),
  .sin_port = htons(DEFAULT_PORT),
  .sin_addr = { htonl(INADDR_ANY) }
};


std::vector<pid_t> clients;


void child_signal(int _) {
  pid_t child = wait(NULL);
  // std::cout << child << " process has finished" << std::endl;
  /* Remove child's PID from list of client children */
  clients.erase(std::find(clients.begin(), clients.end(), child));
}


void server_stop(int _) {
  // std::cout << "Terminating server" << std::endl;

  /* Terminate all children */
  for (pid_t client: clients) {
    kill(client, SIGTERM);
  }

  /* Close the server itself */
  server.close();
  std::exit(0);
}


int main(void) {
  /* Automatically remove "zombies" */
  signal(SIGCHLD, child_signal);

  /* Ways to close the server */
  signal(SIGINT, server_stop);
  signal(SIGQUIT, server_stop);
  signal(SIGTERM, server_stop);
  signal(SIGUSR1, server_stop);

  /* Close server on ENTER. Faster than Ctrl-C */
  if (fork() == 0) {
    signal(SIGCHLD, SIG_DFL);
    signal(SIGINT,  SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGUSR1, SIG_DFL);
    char sym;
    read(0, &sym, sizeof(char));
    kill(getppid(), SIGUSR1);
    exit(0);
  }

  /* Server setup */
  server.bind(&address);
  server.listen(32);

  /* Server main loop */
  std::cout << "Server listening on port " << DEFAULT_PORT << std::endl;
  while (true) {
    Socket conn = server.accept(nullptr);

    pid_t proc = fork();
    if (proc == 0) {
      /* Release server's resources */
      signal(SIGCHLD, SIG_DFL);
      signal(SIGINT,  SIG_DFL);
      signal(SIGQUIT, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
      signal(SIGUSR1, SIG_DFL);
      server.close();

      /* Process the client */
      session(conn);

      /* Finish the client handler */
      conn.close();
      std::exit(0);
    } else {
      clients.push_back(proc);
    }
  }
}
