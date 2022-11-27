#include <examples/dist.consensus.abd/consensus/server.hpp>
#include <examples/dist.consensus.abd/transport/transport_http/transport_http.hpp>

int main() {
  dsac::transport_http transport_http;
  transport_http.serve(8080);
//    httplib::Server svr;
//    svr.listen("0.0.0.0", 8080);
}
