#pragma once
#include <iostream>

class ServerSocketException : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class ServerBindException : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class ServerListenException : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};