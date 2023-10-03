#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>

using tcp = boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
private:
  tcp::socket socket_;
  uint32_t len_;
  std::vector<char> data_;

  void do_read();
  void read_message_content();
  void handle_message();

public:
  explicit session(tcp::socket socket) : socket_(std::move(socket)) {}
  void start();
};