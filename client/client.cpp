#include <array>
#include <format>
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
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) {
    die("Cannot create socket");
  }

  struct sockaddr_in addr {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);

  if (const int err =
          connect(client_socket, (const struct sockaddr *)&addr, sizeof(addr));
      err < 0) {
    die("Cannot connect to port 1234");
  }

  std::array<char, 12> wbuf = {"Hello there"};
  if (const auto err = send(client_socket, wbuf.data(), wbuf.size(), 0);
      err < 0) {
    die("send failed");
  }

  std::array<char, 15> resp;
  if (const auto err = recv(client_socket, resp.data(), resp.size(), 0);
      err < 0) {
    die("recv failed");
  }

  std::cout << resp.data() << std::endl;
  close(client_socket);
}