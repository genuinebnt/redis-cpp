#pragma once

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>

#include <memory>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

class server {
private:
  tcp::socket socket_;
  boost::system::error_code ec_;
  tcp::acceptor acceptor_;

  void do_accept();

public:
  explicit server(asio::io_context &ioc, const tcp::endpoint endpoint)
      : socket_(ioc), acceptor_(ioc, endpoint) {
    BOOST_LOG_TRIVIAL(info) << "Starting server";
    do_accept();
  }

  ~server() = default;
};