#pragma once
#include "exceptions.hpp"
#include "utils.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <vector>

enum class State {
  STATE_REQ = 0,
  STATE_RES = 1,
  STATE_END = 2,
};

const size_t K_MAX_SIZE = 4096;

class Server {
private:
  int _socket{-1};
  uint16_t _port;

public:
  explicit Server(uint16_t port) : _port(port) {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
      throw ServerSocketException("Error creating socket");
    }
  }

  ~Server() { close(_socket); }

  void bind_server() const;
  void listen_on_server() const;
  int start() const;

  int32_t accept_new_connections(std::vector<Connection *> &fd2conn, int fd);
  void conn_put(std::vector<Connection *> &fd2conn, struct Connection *conn);
  void connection_io(Connection *conn);
  void state_req(Connection *conn);
  void state_res(Connection *conn);
  bool try_fill_buffer(Connection *conn);

  static int read_full(int client_socket, char *rbuf, size_t len);
  static int write_full(int client_socket, const char *wbuf, size_t len);
  static int process_one_request(int client_socket);
};