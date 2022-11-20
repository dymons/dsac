#include <catch2/catch.hpp>

#include <dsac/pattern/registrator/registrator.hpp>

namespace {

class consensus {
public:
  using factory = dsac::factory<consensus>;

  consensus()                                = default;
  consensus(const consensus&)                = default;
  consensus(consensus&&) noexcept            = default;
  consensus& operator=(const consensus&)     = default;
  consensus& operator=(consensus&&) noexcept = default;
  virtual ~consensus()                       = default;
};

class paxos final : public consensus {
  static const inline factory::registractor<paxos> kRegistractor;

public:
  static std::string get_type_name() {
    return "consensus/paxos";
  }
};

class raft final : public consensus {
  static const inline factory::registractor<raft> kRegistractor;

public:
  static std::string get_type_name() {
    return "consensus/raft";
  }
};

class pbft final : public consensus {
  static const inline factory::registractor<pbft> kRegistractor;

public:
  static std::string get_type_name() {
    return "consensus/pbft";
  }
};

}  // namespace

TEST_CASE("", "[registrator][contains]") {
  REQUIRE(consensus::factory::contains(paxos::get_type_name()));
  REQUIRE(consensus::factory::contains(raft::get_type_name()));
  REQUIRE(consensus::factory::contains(pbft::get_type_name()));
}

TEST_CASE("", "[registrator][construct]") {
  REQUIRE(consensus::factory::construct(paxos::get_type_name()) != nullptr);
  REQUIRE(consensus::factory::construct(raft::get_type_name()) != nullptr);
  REQUIRE(consensus::factory::construct(pbft::get_type_name()) != nullptr);
}

TEST_CASE("", "[registrator][keys]") {
  std::set<std::string> const keys = consensus::factory::get_registered_keys();
  REQUIRE(keys == std::set<std::string>{paxos::get_type_name(), raft::get_type_name(), pbft::get_type_name()});
}