#include <catch2/catch.hpp>

#include <dsac/memory/shared_ptr.hpp>

TEST_CASE("Construct shared_ptr with default constructor", "[shared_ptr][default-constructor]") {
  dsac::shared_ptr<int> shared;
  REQUIRE(shared.use_count() == 0);
}

TEST_CASE("Construct shared_ptr with std::nullptr_t constructor", "[shared_ptr][nullptr-default-constructor]") {
  dsac::shared_ptr<int> shared = std::nullptr_t{};
  REQUIRE(shared.use_count() == 0);
}

TEST_CASE("Construct shared_ptr with raw pointer", "[shared_ptr][raw-pointer-ownership]") {
  class local_object final {  // NOLINT(cppcoreguidelines-special-member-functions)
    std::reference_wrapper<bool> was_destructed_;

  public:
    explicit local_object(bool& was_destructed)
      : was_destructed_(was_destructed) {
      was_destructed_.get() = false;
    }

    ~local_object() {
      was_destructed_.get() = true;
    }
  };

  bool          was_destructed_local_object = false;
  local_object* raw_pointer = new local_object{was_destructed_local_object};  // NOLINT(cppcoreguidelines-owning-memory)
  REQUIRE_FALSE(was_destructed_local_object);

  {
    dsac::shared_ptr<local_object> shared{raw_pointer};
    REQUIRE(shared.unique());
  }

  REQUIRE(was_destructed_local_object);
}

TEST_CASE("Construct shared_ptr with base/derived classes", "[shared_ptr][base-derived-constructor]") {
  class base {  // NOLINT(cppcoreguidelines-special-member-functions)
    std::reference_wrapper<bool> was_base_class_destructed_;

  public:
    explicit base(bool& was_destructed)
      : was_base_class_destructed_{was_destructed} {
      was_base_class_destructed_.get() = false;
    }

    virtual ~base() {
      was_base_class_destructed_.get() = true;
    }
  };

  class derived final : public base {  // NOLINT(cppcoreguidelines-special-member-functions)
    std::reference_wrapper<bool> was_derived_class_destructed_;

  public:
    explicit derived(bool& was_base_class_destructed, bool& was_derived_class_destructed)
      : base(was_base_class_destructed)
      , was_derived_class_destructed_{was_derived_class_destructed} {
      was_derived_class_destructed_.get() = false;
    }

    ~derived() final {
      was_derived_class_destructed_.get() = true;
    }
  };

  bool was_base_class_destructed    = false;
  bool was_derived_class_destructed = false;
  {
    // clang-format off
    dsac::shared_ptr<base> base_data = dsac::shared_ptr(new derived{was_base_class_destructed, was_derived_class_destructed});
    // clang-format on
    REQUIRE_FALSE(was_base_class_destructed);
    REQUIRE_FALSE(was_derived_class_destructed);
  }

  REQUIRE(was_base_class_destructed);
  REQUIRE(was_derived_class_destructed);
}

TEST_CASE("Execute shared_ptr with base/derived classes", "[shared_ptr][base-derived-execution]") {
  class base {  // NOLINT(cppcoreguidelines-special-member-functions)
  public:
    [[nodiscard]] virtual std::string get_name_class() const {
      return "base";
    }
    virtual ~base() = default;
  };

  class derived final : public base {  // NOLINT(cppcoreguidelines-special-member-functions)
  public:
    [[nodiscard]] std::string get_name_class() const final {
      return "derived";
    }
  };

  dsac::shared_ptr<base> base_class = dsac::shared_ptr(new derived{});
  REQUIRE(base_class->get_name_class() == "derived");
}

TEST_CASE("Construct shared_ptr with copy constructor", "[shared_ptr][default-copy-constructor]") {
  dsac::shared_ptr<int> shared = dsac::shared_ptr<int>{new int{}};
  REQUIRE(shared.unique());

  {
    dsac::shared_ptr<int> shared_copy = shared;  // NOLINT(performance-unnecessary-copy-initialization)
    REQUIRE_FALSE(shared.unique());
    REQUIRE_FALSE(shared_copy.unique());
    REQUIRE(shared.use_count() == 2);
    REQUIRE(shared_copy.use_count() == 2);
  }

  REQUIRE(shared.unique());
}

TEST_CASE("Construct shared_ptr with nullptr copy constructor", "[shared_ptr][nullptr-copy-constructor]") {
  dsac::shared_ptr<int> shared = std::nullptr_t{};
  REQUIRE(shared.use_count() == 0);

  {
    dsac::shared_ptr<int> shared_copy = shared;  // NOLINT(performance-unnecessary-copy-initialization)
    REQUIRE(shared_copy.use_count() == 0);
    REQUIRE(shared.use_count() == 0);
  }

  REQUIRE(shared.use_count() == 0);
}

TEST_CASE("Construct shared_ptr with move constructor", "[shared_ptr][default-move-constructor]") {
  dsac::shared_ptr<int> shared = dsac::shared_ptr<int>{new int{}};
  REQUIRE(shared.unique());

  {
    dsac::shared_ptr<int> shared_copy = std::move(shared);
    REQUIRE(shared.use_count() == 0);  // NOLINT(bugprone-use-after-move)
    REQUIRE(shared_copy.unique());
  }

  REQUIRE(shared.use_count() == 0);
}

TEST_CASE("Construct shared_ptr with nullptr move constructor", "[shared_ptr][nullptr-move-constructor]") {
  dsac::shared_ptr<int> shared = std::nullptr_t{};
  REQUIRE(shared.use_count() == 0);

  {
    dsac::shared_ptr<int> shared_copy = std::move(shared);
    REQUIRE(shared.use_count() == 0);  // NOLINT(bugprone-use-after-move)
    REQUIRE(shared_copy.use_count() == 0);
  }

  REQUIRE(shared.use_count() == 0);
}
