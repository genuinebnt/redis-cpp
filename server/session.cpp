#include "session.hpp"

#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>

void session::start() {
  BOOST_LOG_TRIVIAL(info) << "Starting session";
  do_read();
}

void session::do_read() {
  auto self(shared_from_this());
  boost::asio::async_read(socket_, boost::asio::buffer(&len_, sizeof(len_)),
                          [this, self](const boost::system::error_code &ec,
                                       std::size_t /*length*/) {
                            if (!ec || ec != boost::asio::error::eof) {
                              BOOST_LOG_TRIVIAL(info) << "Size of message is: "
                                                      << std::to_string(len_);
                              self->read_message_content();
                            } else {
                              BOOST_LOG_TRIVIAL(info)
                                  << "Cannot read message length: "
                                  << ec.what();
                            }
                          });
}

void session::read_message_content() {
  auto self(shared_from_this());
  data_.resize(len_);
  boost::asio::async_read(
      socket_, boost::asio::buffer(data_, len_),
      [self](const boost::system::error_code &ec, std::size_t /*length*/) {
        if (!ec || ec == boost::asio::error::eof) {
          self->handle_message();
        } else {
          BOOST_LOG_TRIVIAL(info)
              << "Cannot read message content: " << ec.what();
        }
      });
}

void session::handle_message() {
  std::string message(data_.begin(), data_.end());
  BOOST_LOG_TRIVIAL(info) << "Message is: " << message;
  do_read();
}