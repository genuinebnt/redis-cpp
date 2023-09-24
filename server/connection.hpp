#include <array>
#include <iostream>

struct Connection {
  int fd = -1;
  uint32_t state = 0;
  size_t r_buf_size = 0;
  std::array<char, 4 + K_MAX_SIZE> rbuf;
  size_t wbuf_size = 0;
  std::array<char, 4 + K_MAX_SIZE> wbuf;
};