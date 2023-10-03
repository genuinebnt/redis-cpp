#include "server.hpp"
#include "session.hpp"

int main() {
  try {
    boost::asio::io_context ioc{};
    server server(ioc, tcp::endpoint(tcp::v4(), 8080));
    ioc.run();
  } catch (const std::exception &e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
    return -1;
  }
  return 0;
}

void server::do_accept() {
  acceptor_.async_accept(
      [this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
          std::make_shared<session>(std::move(socket))->start();
        }
        do_accept();
      });
}