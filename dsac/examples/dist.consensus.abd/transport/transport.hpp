#pragma once

#include <stdexcept>
#include <string>

namespace dsac {

class transport_exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class transport_server_socket : public transport_exception {
public:
  using transport_exception::transport_exception;
};

class transport_base {
public:
  transport_base()                                     = default;
  transport_base(const transport_base&)                = default;
  transport_base(transport_base&&) noexcept            = default;
  transport_base& operator=(const transport_base&)     = default;
  transport_base& operator=(transport_base&&) noexcept = default;
  virtual ~transport_base()                            = default;

  virtual void serve(int port) = 0;
};

}  // namespace dsac
