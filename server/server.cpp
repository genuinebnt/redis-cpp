#include <array>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

const size_t k_max_size = 4096;

[[noreturn]] void die(std::string_view message) {
  std::cerr << message << ": " << strerror(errno) << std::endl;
  exit(EXIT_FAILURE);
}

int read_full(int client_socket, char *rbuf, size_t len) {
  while (len > 0) {
    const ssize_t len_read = read(client_socket, rbuf, len);
    if (len_read == -1) {
      return -1;
    }
    len -= len_read;
    rbuf += len_read;
  }

  return 0;
}

int write_full(int client_socket, const char *wbuf, size_t len) {
  while (len > 0) {
    const ssize_t len_written = write(client_socket, wbuf, len);
    if (len_written == -1) {
      return -1;
    }
    len -= len_written;
    wbuf += len_written;
  }
  return 0;
}

int process_one_request(int client_socket) {
  std::vector<char> rbuf(4 + k_max_size + 1);

  int err = read_full(client_socket, rbuf.data(), 4);
  if (err != 0) {
    std::cerr << "read error " << std::endl;
    return -1;
  }

  uint32_t len = 0;
  std::memcpy(&len, rbuf.data(), 4);
  if (len > k_max_size) {
    std::cerr << "message too large" << std::endl;
    return -1;
  }

  err = read_full(client_socket, rbuf.data() + 4, len);
  if (err < 0) {
    std::cerr << "read error" << std::endl;
    return -1;
  }

  for (const auto &i : rbuf) {
    std::cout << i;
  }
  std::cout << '\n';

  const std::string msg = "general kenobi";
  const size_t msg_len = msg.size();

  std::vector<char> wbuf(4 + msg.size());

  std::memcpy(wbuf.data(), &msg_len, 4);
  std::memcpy(wbuf.data() + 4, msg.data(), msg_len);

  err = write_full(client_socket, wbuf.data(), wbuf.size());
  if (err < 0) {
    std::cerr << "write failed" << std::endl;
    return -1;
  }

  return 0;
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

    if (client_socket < 0) {
      continue;
    }

    err = process_one_request(client_socket);
    if (err < 0) {
      break;
    }

    close(server_socket);
  }
}