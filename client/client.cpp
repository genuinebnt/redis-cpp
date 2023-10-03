#include <iostream>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

using tcp = boost::asio::ip::tcp;

int main() {
  boost::asio::io_context ioc{};

  tcp::socket socket{ioc};

  boost::system::error_code ec{};

  socket.connect(
      tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));

  const char *msg = "THISISASMALLMESSAGEIWANTEDTOSENT";
  uint32_t length = std::strlen(msg);

  std::vector<char> data(4 + length);

  std::memcpy(data.data(), &length, 4);
  std::memcpy(data.data() + 4, msg, length);

  boost::asio::write(socket, boost::asio::buffer(data));

  if (ec) {
    std::cerr << "Error sending failed" << std::endl;
  }

  socket.close();
  return 0;
}