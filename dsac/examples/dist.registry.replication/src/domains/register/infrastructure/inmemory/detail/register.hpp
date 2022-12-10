#pragma once

#include <dsac/memory/shared_ptr.hpp>

#include <cstdint>
#include <optional>

namespace dsac::infrastructure::inmemory::detail {

class register_ final {
public:
  struct value final {
    int         value;
    std::size_t timestamp;
  };

  register_();

  auto write(std::int32_t value, std::size_t timestamp) -> void;
  auto read() -> std::optional<value>;
  auto reset() -> void;

private:
  class pimpl;
  shared_ptr<pimpl> pimpl_;
};

}  // namespace dsac::infrastructure::inmemory::detail