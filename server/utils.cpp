#include "utils.h"

int fd_set_nb(int fd) {
  int errno = 0;
  int flags = fcntl(fd, F_GETFL, 0);
  if (errno) {
    return -1;
  }

  flags |= O_NONBLOCK;

  errno = 0;
  fnctl(fd, F_SETFL, flags);
  if (errno) {
    return -1;
  }
}