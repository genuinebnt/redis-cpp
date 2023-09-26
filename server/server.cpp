#include "server.hpp"
#include "connection.hpp"
#include "utils.hpp"

#include <poll.h>

int Server::read_full(int client_socket, char *rbuf, size_t len) {
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

int Server::write_full(int client_socket, const char *wbuf, size_t len) {
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

int Server::process_one_request(int client_socket) {
  std::vector<char> rbuf(4 + K_MAX_SIZE + 1);

  int err = read_full(client_socket, rbuf.data(), 4);
  if (err != 0) {
    std::cerr << "read error " << std::endl;
    return -1;
  }

  uint32_t len = 0;
  std::memcpy(&len, rbuf.data(), 4);
  if (len > K_MAX_SIZE) {
    std::cerr << "message too large" << std::endl;
    return -1;
  }

  err = read_full(client_socket, rbuf.data() + 4, len);
  if (err < 0) {
    std::cerr << "read error" << std::endl;
    return -1;
  }
  return 0;
}

void Server::bind_server() const {
  int val = 1;
  setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));

  struct sockaddr_in addr {};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(_port);

  int err = bind(_socket, (const struct sockaddr *)&addr, sizeof(addr));
  if (err < 0) {
    throw ServerBindException("Cannot bind to socket with port " +
                              std::to_string(_port));
  }
}

void Server::listen_on_server() const {
  int err = listen(_socket, SOMAXCONN);
  if (err < 0) {
    throw ServerListenException("Cannot listen to socket with port " +
                                std::to_string(_port));
  }
}

void Server::connection_io(Connection *conn) {
  if (conn->state == State::STATE_REQ) {
    state_req(conn);
  } else if (conn->state == State::STATE_RES) {
    state_res(conn);
  } else {
    assert(0);
  }
}

void Server::try_fill_buffer(Connection *conn) {
  assert(conn->r_buf_size < sizeof(conn->rbuf));
}

void Server::state_req(Connection *conn) {
  while (try_fill_buffer(conn)) {
  }
}

void conn_put(std::vector<Connection *> &fd2conn, struct Connection *conn) {
  if (fd2conn.size() <= (size_t)conn->fd) {
    fd2conn.resize(conn->fd + 1);
  }

  fd2conn[conn->fd] = conn;
}

int32_t Server::accept_new_connections(std::vector<Connection *> &fd2conn,
                                       int fd) {
  struct sockaddr_in addr = {};
  socklen_t addrlen = sizeof(addr);
  int connfd = accept(fd, (struct sockaddr *)&addr, &addrlen);
  if (connfd < 0) {
    std::cerr << "Accept error: " << strerror(errno) << std::endl;
    return -1;
  }

  fd_set_nb(fd);

  struct Connection *conn =
      (struct Connection *)malloc(sizeof(struct Connection));
  if (!conn) {
    close(connfd);
    return -1;
  }

  conn->fd = connfd;
  conn->state = State::STATE_REQ;
  conn->r_buf_size = 0;
  conn->wbuf_size = 0;
  conn->wbuf_send = 0;
  conn_put(fd2conn, conn);
  return 0;
}

int Server::start() const {
  bind_server();
  listen_on_server();

  // map of all client connections
  std::vector<Connection *> fd2conn;

  // set socket to non-blocking mode
  if (int err = fd_set_nb(_socket); err < 0) {
    std::cerr << "Failed to set socket to non-blocking mode" << std::endl;
    return -1;
  }

  // event loop
  std::vector<struct pollfd> poll_args;
  while (true) {
    poll_args.clear();

    struct pollfd pfd = {_socket, POLLIN, 0};
    poll_args.push_back(pfd);

    for (Connection const *conn : fd2conn) {
      if (!conn) {
        continue;
      }

      struct pollfd pfd = {};
      pfd.fd = conn->fd;
      pfd.events = (conn->state == State::STATE_REQ) ? POLLIN : POLLOUT;
      pfd.events = pfd.events | POLLERR;
      poll_args.push_back(pfd);
    }

    if (int rv = poll(poll_args.data(), poll_args.size(), 1000); rv < 0) {
      std::cerr << "Error polling" << std::endl;
      return -1;
    }

    for (const auto &args : poll_args) {
      if (args.revents) {
        Connection *conn = fd2conn[args.fd];
        connection_io(conn);
        if (conn->state == State::STATE_END) {
          fd2conn[conn->fd] = nullptr;
          close(conn->fd);
          free(conn);
        }
      }
    }

    // first argument is the listening fd
    if (poll_args.front().revents) {
      accept_new_connection(fd2conn, _socket);
    }
  }
  return 0;
}

int main() {
  Server svr(1234);
  svr.start();
}