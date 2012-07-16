#include <cstdlib>
#include <pion/net/WebServer.hpp>
#include <dslam/service.hpp>

// Minimal server that replies with "OK\n" to all requests.
int main(int argc, char* argv[]) {

  pion::net::WebServer server(std::atoi(argv[1]));

  dslam::service ok_server{
    { true, [](dslam::service::context_ptr const& ctx) -> dslam::route_result {
	return "OK\n";
    }}
  };

  server.addService("/", &ok_server);
  server.start();
  pause();
  server.stop();

  return EXIT_SUCCESS;
}
