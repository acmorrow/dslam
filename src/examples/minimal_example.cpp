#include <cstdlib>
#include <pion/http/plugin_server.hpp>
#include <dslam/service.hpp>

// Minimal server that replies with "OK\n" to all requests.
int main(int argc, char* argv[]) {
    pion::http::plugin_server server(std::atoi(argv[1]));

    dslam::service ok_server{
        {true, [](dslam::service::context_ptr const&) -> dslam::route_result { return "OK\n"; }}};

    server.add_service("/", &ok_server);
    server.start();
    pause();
    server.stop();

    return EXIT_SUCCESS;
}
