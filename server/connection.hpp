#pragma once

#include <array>
#include <iostream>

#include "server.hpp"

struct Connection {
  int fd = -1;
  State state = State::STATE_REQ;
  size_t r_buf_size = 0;
  std::array<char, 4 + K_MAX_SIZE> rbuf;
  size_t wbuf_size = 0;
  std::array<char, 4 + K_MAX_SIZE> wbuf;
};