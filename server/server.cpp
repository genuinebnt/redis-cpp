#include <array>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

[[noreturn]] void die(std::string_view message) {
  std::cerr << message << ": " << strerror(errno) << std::endl;
  exit(EXIT_FAILURE);
}

int main() {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    die("Failed to initialize socket");
  }

  int val = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&val,
             sizeof(val));

  struct sockaddr_in addr {};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(1234);

  int err = bind(server_socket, (const struct sockaddr *)&addr, sizeof(addr));
  if (err < 0) {
    die("Failed to bind to port 1234");
  }

  err = listen(server_socket, SOMAXCONN);
  if (err < 0) {
    die("Failed to listen");
  }

  while (true) {
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);

    int client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &socklen);

    std::array<char, 12> rbuf;

    if (const ssize_t rc = read(client_socket, rbuf.data(), 12); rc < 0) {
      std::cerr << "Failed to read from socket" << std::endl;
    }

    std::cout << rbuf.data() << std::endl;

    if (strncmp(rbuf.data(), "Hello there", 12) == 0) {
      if (const ssize_t wc = write(client_socket, "general kenobi", 15);
          wc < 0) {
        std::cerr << "write error" << std::endl;
      }
    }

    close(server_socket);
  }
}