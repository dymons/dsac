#include <catch2/catch.hpp>

#include <examples/dist.registry.replication/src/domains/register/application/commands/replica/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register_repository.hpp>

using dsac::application::command::replica::write_register_command;
using dsac::application::command::replica::write_register_command_handler;
using dsac::domain::register_repository_base;
using dsac::domain::register_timestamp;
using dsac::domain::register_value;
using dsac::domain::register_value_object;

namespace {

class fake_repository final : public register_repository_base {
public:
  auto write(register_value_object const& object) -> void override {
    register_.emplace(object);
  };

  auto reset() -> void override {
    register_.reset();
  };

  auto read() -> std::optional<register_value_object> override {
    return register_;
  };

  static auto make() -> dsac::domain::register_repository_ref {
    return dsac::make_shared<fake_repository>();
  }

private:
  std::optional<register_value_object> register_;
};

}  // namespace

TEST_CASE(
    "Given empty register when write a user value then user value has been saved to the register",
    "[replica][write-register][happy-path]"
) {
  // Arrange
  auto repository = fake_repository::make();
  REQUIRE_FALSE(repository->read().has_value());

  // Act
  SECTION("Write a user value to the register") {
    write_register_command command{
        .new_register_value = register_value_object{
            register_value{1},
            register_timestamp{1UL},
        }};

    write_register_command_handler command_handler{repository};
    command_handler.handle(command);

    // Assert
    SECTION("Check that the user value has been saved") {
      REQUIRE(repository->read() == command.new_register_value);
    }
  }
}