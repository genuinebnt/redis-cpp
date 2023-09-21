#include <array>
#include <format>
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

int write_full(int fd, const char *wbuf, size_t len) {
  while (len > 0) {
    auto len_written = write(fd, wbuf, len);
    if (len_written < 0) {
      return -1;
    }
    len -= len_written;
    wbuf += len_written;
  }

  return 0;
}

int read_full(int fd, char *rbuf, size_t len) {
  while (len > 0) {
    rbuf += 4;
    auto len_read = read(fd, rbuf, len);
    if (len_read < 0) {
      return -1;
    }
    len -= len_read;
    rbuf += len_read;
  }
  return 0;
}

int query(int fd, std::string msg) {
  size_t len = msg.size();
  std::cout << len << ":" << k_max_size << std::endl;
  // if (len > k_max_size) {
  //   std::cerr << "Message too long" << std::endl;
  //   return -1;
  // }

  std::vector<char> wbuf;
  wbuf.reserve(4 + len);

  memcpy(wbuf.data(), &len, 4);
  memcpy(wbuf.data() + 4, msg.data(), len);

  if (const int err = write_full(fd, wbuf.data(), wbuf.size()); err < 0) {
    std::cerr << "cannot write to socket" << std::endl;
  }

  return 0;
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

  std::string msg = "Hello, world!";
  if (const int err = query(client_socket, msg); err < 0) {
    std::cerr << "Error cannot query given message: " << msg << std::endl;
  }

  std::vector<char> rbuf;
  const int len = read_full(client_socket, rbuf.data(), 4);
  if (len < 0) {
    std::cerr << "Error cannot read from server" << std::endl;
  }

  if (const int err = read_full(client_socket, rbuf.data() + 4, len); err < 0) {
    std::cerr << "Error cannot read from server" << std::endl;
  }

  return 0;
}