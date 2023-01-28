#pragma once

#include <stdexcept>

namespace dsac::domain {

class cluster_exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class not_found_latest_value final : public cluster_exception {
public:
  not_found_latest_value()
    : cluster_exception("Couldn't extract the current register value") {
  }
};

class not_found_latest_timestamp final : public cluster_exception {
public:
  not_found_latest_timestamp()
    : cluster_exception("Couldn't extract the current register timestamp") {
  }
};

}  // namespace dsac::domain