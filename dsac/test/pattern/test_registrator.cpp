#include <catch2/catch.hpp>

#include <dsac/pattern/registrator/registrator.hpp>

class executor {
public:
  using factory = dsac::factory<executor>;

  executor()                               = default;
  executor(const executor&)                = default;
  executor(executor&&) noexcept            = default;
  executor& operator=(const executor&)     = default;
  executor& operator=(executor&&) noexcept = default;
  virtual ~executor()                      = default;
};

class static_thread_pool final : public executor {
  static const inline factory::registractor<static_thread_pool> kRegistractor;

public:
  static std::string get_type_name() {
    return "static_thread_pool";
  }
};

TEST_CASE("", "") {
  REQUIRE(executor::factory::contains(static_thread_pool::get_type_name()));

  auto static_thread_pool = executor::factory::construct(static_thread_pool::get_type_name());
  REQUIRE(static_thread_pool != nullptr);

  std::set<std::string> const keys = executor::factory::get_registered_keys();
  REQUIRE(keys == std::set<std::string>{static_thread_pool::get_type_name()});
}