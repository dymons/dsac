#include <examples/dist.consensus.abd/consensus/server.hpp>

int main() {
  httplib::Server svr;
  svr.listen("0.0.0.0", 8080);
}
