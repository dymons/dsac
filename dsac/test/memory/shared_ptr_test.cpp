#include "catch2/catch.hpp"

#include "shared_ptr.hpp"
#include <functional>

TEST_CASE("Создание умного указателя SharedPtr", "[shared_ptr_build]") {
  using namespace algo::pointers;
  SECTION("Проверка корректности для конструкторов и операторов пустого SharedPtr") {
    SharedPtr<int> shared;
    REQUIRE(shared.UseCount() == 0);

    SharedPtr<int> shared_copy_constructor = shared;
    REQUIRE(shared.UseCount() == 0);
    REQUIRE(shared_copy_constructor.UseCount() == 0);

    SharedPtr<int> shared_move_constructor = std::move(shared);
    REQUIRE(shared.UseCount() == 0);
    REQUIRE(shared_move_constructor.UseCount() == 0);

    SharedPtr<int> shared_copy_operator;
    shared_copy_operator = shared;
    REQUIRE(shared.UseCount() == 0);
    REQUIRE(shared_copy_operator.UseCount() == 0);

    SharedPtr<int> shared_move_operator;
    shared_move_operator = std::move(shared);
    REQUIRE(shared.UseCount() == 0);
    REQUIRE(shared_move_operator.UseCount() == 0);
  }

  SECTION("Конструктор копирования SharedPtr") {
    SharedPtr<int> shared{new int{1}};
    SharedPtr<int> copy_shared = shared;
    REQUIRE(shared.UseCount() == 2);
    REQUIRE(copy_shared.UseCount() == 2);
    REQUIRE(copy_shared.GetData() == shared.GetData());
  }

  SECTION("Оператор копирования SharedPtr") {
    SharedPtr<int> shared{new int{1}};
    SharedPtr<int> copy_shared;
    copy_shared = shared;

    REQUIRE(shared.UseCount() == 2);
    REQUIRE(copy_shared.UseCount() == 2);
    REQUIRE(copy_shared.GetData() == shared.GetData());

    class Local final {
      bool* destroyed_{nullptr};

     public:
      Local() noexcept = default;
      explicit Local(bool* destroyed) noexcept : destroyed_(destroyed) {
      }
      ~Local() noexcept {
        if (destroyed_ != nullptr) {
          *destroyed_ = true;
        }
      }
    };

    bool is_destroyed = false;
    SharedPtr<Local> shared_local_1{new Local{&is_destroyed}};
    SharedPtr<Local> shared_local_2{new Local{}};
    REQUIRE(shared_local_1.UseCount() == 1);
    REQUIRE(shared_local_2.UseCount() == 1);
    shared_local_1 = shared_local_2;
    REQUIRE(is_destroyed);
    REQUIRE(shared_local_1.UseCount() == 2);
    REQUIRE(shared_local_2.UseCount() == 2);
    REQUIRE(shared_local_1.GetData() == shared_local_2.GetData());
  }

  SECTION("Конструктор перемещения SharedPtr") {
    SharedPtr<int> shared{new int{1}};
    SharedPtr<int> copy_shared = std::move(shared);
    REQUIRE(shared.UseCount() == 0);
    REQUIRE(copy_shared.UseCount() == 1);
    REQUIRE(copy_shared.GetData() != shared.GetData());
  }

  SECTION("Оператор перемещения SharedPtr") {
    SharedPtr<int> shared{new int{1}};
    SharedPtr<int> move_shared;
    move_shared = std::move(shared);

    REQUIRE(shared.UseCount() == 0);
    REQUIRE(move_shared.UseCount() == 1);
    REQUIRE(move_shared.GetData() != shared.GetData());

    class Local final {
      bool* destroyed_{nullptr};

     public:
      Local() noexcept = default;
      explicit Local(bool* destroyed) noexcept : destroyed_(destroyed) {
      }
      ~Local() noexcept {
        if (destroyed_ != nullptr) {
          *destroyed_ = true;
        }
      }
    };

    bool is_destroyed = false;
    SharedPtr<Local> shared_local_1{new Local{&is_destroyed}};
    SharedPtr<Local> shared_local_2{new Local{}};
    REQUIRE(shared_local_1.UseCount() == 1);
    REQUIRE(shared_local_2.UseCount() == 1);
    shared_local_1 = std::move(shared_local_2);
    REQUIRE(is_destroyed);
    REQUIRE(shared_local_1.UseCount() == 1);
    REQUIRE(shared_local_2.UseCount() == 0);
    REQUIRE(shared_local_1.GetData() != shared_local_2.GetData());
  }
}