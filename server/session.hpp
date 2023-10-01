#pragma once

#include <boost/asio.hpp>
#include <memory>

using tcp = boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
private:
  tcp::socket socket_;
  enum { max_length = 1024 };
  std::array<char, max_length> data_;

  void do_read();
  void do_write(std::size_t length);

public:
  explicit session(tcp::socket socket) : socket_(std::move(socket)) {}
  void start();
};