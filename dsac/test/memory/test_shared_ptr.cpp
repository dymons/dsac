#include <catch2/catch.hpp>

#include <dsac/memory/shared_ptr.hpp>

TEST_CASE("Construction of a smart pointer shared_ptr", "[shared_ptr][build]") {
  SECTION("Construction of a smart pointer using default constructor") {
    dsac::shared_ptr<int> shared;
    REQUIRE(shared.use_count() == 0);

    dsac::shared_ptr<int> shared_copy_constructor = shared;
    REQUIRE(shared.use_count() == 0);
    REQUIRE(shared_copy_constructor.use_count() == 0);

    dsac::shared_ptr<int> shared_move_constructor = std::move(shared);
    REQUIRE(shared.use_count() == 0);
    REQUIRE(shared_move_constructor.use_count() == 0);

    dsac::shared_ptr<int> shared_copy_operator;
    shared_copy_operator = shared;
    REQUIRE(shared.use_count() == 0);
    REQUIRE(shared_copy_operator.use_count() == 0);

    dsac::shared_ptr<int> shared_move_operator;
    shared_move_operator = std::move(shared);
    REQUIRE(shared.use_count() == 0);  // NOLINT(bugprone-use-after-move)
    REQUIRE(shared_move_operator.use_count() == 0);
  }

  SECTION("Construction of a smart pointer using copy constructor") {
    dsac::shared_ptr<int> shared{new int{1}};
    dsac::shared_ptr<int> copy_shared = shared;  // NOLINT(performance-unnecessary-copy-initialization)
    REQUIRE(shared.use_count() == 2);
    REQUIRE(copy_shared.use_count() == 2);
    REQUIRE(copy_shared.get() == shared.get());
  }

  SECTION("Construction of a smart pointer using copy assignment") {
    dsac::shared_ptr<int> shared{new int{1}};
    dsac::shared_ptr<int> copy_shared;
    copy_shared = shared;

    REQUIRE(shared.use_count() == 2);
    REQUIRE(copy_shared.use_count() == 2);
    REQUIRE(copy_shared.get() == shared.get());

    class Local final {
      bool* destroyed_{nullptr};

    public:
      Local() noexcept = default;
      explicit Local(bool* destroyed) noexcept
        : destroyed_(destroyed) {
      }
      ~Local() noexcept {
        if (destroyed_ != nullptr) {
          *destroyed_ = true;
        }
      }
    };

    bool                    is_destroyed = false;
    dsac::shared_ptr<Local> shared_local_1{new Local{&is_destroyed}};
    dsac::shared_ptr<Local> shared_local_2{new Local{}};
    REQUIRE(shared_local_1.use_count() == 1);
    REQUIRE(shared_local_2.use_count() == 1);
    shared_local_1 = shared_local_2;
    REQUIRE(is_destroyed);
    REQUIRE(shared_local_1.use_count() == 2);
    REQUIRE(shared_local_2.use_count() == 2);
    REQUIRE(shared_local_1.get() == shared_local_2.get());
  }

  SECTION("Construction of a smart pointer using move constructor") {
    dsac::shared_ptr<int> shared{new int{1}};
    dsac::shared_ptr<int> copy_shared = std::move(shared);
    REQUIRE(shared.use_count() == 0);
    REQUIRE(copy_shared.use_count() == 1);
    REQUIRE(copy_shared.get() != shared.get());
  }

  SECTION("Construction of a smart pointer using move assignment") {
    dsac::shared_ptr<int> shared{new int{1}};
    dsac::shared_ptr<int> move_shared;
    move_shared = std::move(shared);

    REQUIRE(shared.use_count() == 0);
    REQUIRE(move_shared.use_count() == 1);
    REQUIRE(move_shared.get() != shared.get());

    class Local final {
      bool* destroyed_{nullptr};

    public:
      Local() noexcept = default;
      explicit Local(bool* destroyed) noexcept
        : destroyed_(destroyed) {
      }
      ~Local() noexcept {
        if (destroyed_ != nullptr) {
          *destroyed_ = true;
        }
      }
    };

    bool                    is_destroyed = false;
    dsac::shared_ptr<Local> shared_local_1{new Local{&is_destroyed}};
    dsac::shared_ptr<Local> shared_local_2{new Local{}};
    REQUIRE(shared_local_1.use_count() == 1);
    REQUIRE(shared_local_2.use_count() == 1);
    shared_local_1 = std::move(shared_local_2);
    REQUIRE(is_destroyed);
    REQUIRE(shared_local_1.use_count() == 1);
    REQUIRE(shared_local_2.use_count() == 0);
    REQUIRE(shared_local_1.get() != shared_local_2.get());
  }
}

TEST_CASE("Construction of a smart pointer through inheritance", "[shared_ptr][inheritance]") {
  class base {
  public:
    base()                           = default;
    base(const base&)                = default;
    base(base&&) noexcept            = default;
    base& operator=(const base&)     = default;
    base& operator=(base&&) noexcept = default;
    virtual ~base()                  = default;

    virtual std::string heavy_computing() const noexcept {
      return "base";
    }
  };

  class derived : public base {
  public:
    std::string heavy_computing() const noexcept override {
      return "derived";
    }
  };

  dsac::shared_ptr<base> base_data = dsac::make_shared<derived>();
  REQUIRE(base_data->heavy_computing() == "derived");
}

TEST_CASE("Resource management", "[shared_ptr][memory]") {
  class base {
  public:
    base()                           = default;
    base(const base&)                = default;
    base(base&&) noexcept            = default;
    base& operator=(const base&)     = default;
    base& operator=(base&&) noexcept = default;
    virtual ~base()                  = default;
  };

  class derived : public base {
    std::reference_wrapper<bool> is_destroyed_;

  public:
    explicit derived(bool& is_destroyed)
      : is_destroyed_{is_destroyed} {
    }
    derived(const derived&)                = default;
    derived(derived&&) noexcept            = default;
    derived& operator=(const derived&)     = default;
    derived& operator=(derived&&) noexcept = default;
    ~derived() override {
      is_destroyed_.get() = true;
    }
  };

  bool is_destroyed = false;
  { dsac::shared_ptr<base> base_data = dsac::make_shared<derived>(is_destroyed); }
  REQUIRE(is_destroyed);
}