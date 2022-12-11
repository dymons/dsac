#pragma once

#include <stdexcept>

namespace dsac::presentation {

class exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class invalid_argument : public exception {
public:
  using exception::exception;
};

class not_found : public exception {
public:
  using exception::exception;
};

class service_unavailable : public exception {
public:
  using exception::exception;
};

}  // namespace dsac::presentation