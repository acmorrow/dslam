#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <unordered_map>

#include <boost/asio/ip/tcp.hpp>
#include <boost/program_options.hpp>

#include <pion/http/plugin_server.hpp>
#include <pion/logger.hpp>

#include <dslam/make_unique.hpp>
#include <dslam/predicates.hpp>
#include <dslam/service.hpp>

namespace {

  // NOTE(C++11): New style function signature
  // NOTE(C++11): unique_ptr
  auto make_service(pion::scheduler& scheduler) -> std::unique_ptr<dslam::service> {

    // NOTE(C++11): New style typdefs.
    using context = dslam::service::context_ptr;
    using result = dslam::route_result;

    // NOTE(C++11): Type inference and 'auto' keyword
    auto const& get = dslam::predicates::http::verbs::get;
    auto const& post = dslam::predicates::http::verbs::post;

    struct hitcounter {
      void record(context const& context) {
	++hits[context->request()->get_resource()];
      }

      // NOTE(C++11): unordered_map
      using hitmap = std::unordered_map<std::string, uint64_t>;
      hitmap hits;
    };

    // NOTE(C++11): make_shared
    auto counter = std::make_shared<hitcounter>();
    auto resolver = std::make_shared<pion::stdx::asio::ip::tcp::resolver>(
      scheduler.get_io_service());

    return std::unique_ptr<dslam::service>(new dslam::service{

      // NOTE(C++11): brace initializers
      // NOTE(C++11): lambdas and captures
      { true, [counter](context const& context) -> result {
	  counter->record(context);
	  return dslam::next_handler;
      }},

      { "hello", get or post, [](context const& context) -> result {
	  return "Hello, World!\n";
      }},

      { "hitcounter", get, [counter](context const& context) -> result {
	  auto writer = context->writer();
	  // NOTE(C++11): New style for loop
	  for (auto const& hit : counter->hits)
	    writer << hit.first << ": " << hit.second << "\n";
	  return writer;
      }},

      // NOTE(C++11): Built in regular expressions
      // NOTE(C++11): Raw string literals
      { std::regex(R"(^rtest/(.*)/(.*))"), get, [](context const& context) -> result {
	  auto writer = context->writer();
	  writer << "First param: " << context->matches()[1] << "\n";
	  writer << "Second param: " << context->matches()[2] << "\n";
	  return writer;
      }},

      { "users/:name", get, [](context const& context) -> result {
	  auto writer = context->writer();
	  writer->write("Hello, ");
	  writer->write(context->match("name"));
	  writer->write("!\n");
	  return writer;
      }},

      // NOTE(acm): A misbehaving route that blocks the IO thread. Don't do this!
      // NOTE(C++11): std::chrono, and std::this_thread
      { "block/:duration", get, [](context const& context) -> result {
	  auto const duration = boost::lexical_cast<int>(context->match("duration"));
	  std::this_thread::sleep_for(std::chrono::seconds(duration));
	  return "Done sleeping\n";
      }},

      // NOTE(acm): Instead, do it from a separate thread.
      // NOTE(C++11): Starting a thread with std::thread constructor and a lambda
      { "block_in_thread/:duration", get, [](context const& context) -> result {
	  auto task = [context]() {
	    auto const duration = boost::lexical_cast<int>(context->match("duration"));
	    std::this_thread::sleep_for(std::chrono::seconds(duration));
	    context->dispatch_result("Done sleeping in thread\n");
	  };

	  std::thread t(task);
	  t.detach();

	  return dslam::defer_response;
      }},

      { "block_in_thread2/:duration", get, [](context const& context) -> result {
	  context->run_in_thread([context]() -> result {
	    auto const duration = boost::lexical_cast<int>(context->match("duration"));
	    std::this_thread::sleep_for(std::chrono::seconds(duration));
	    return "Done sleeping in thread\n";
	  });
	  return dslam::defer_response;
      }},

      { "chunks", get, [](context const& context) -> result {
	  auto writer = context->writer();
	  writer->write("Some stuff\n");
	  writer->send_chunk([context](pion::stdx::error_code const& error, std::size_t written) {
	    context->writer()->clear();
	    context->writer()->write("More stuff!\n");
	    context->writer()->send_final_chunk();
	  });

	  return dslam::defer_response;
      }},

      { "resolve/:host", get, [resolver](context const& context) -> result {
	  const std::string host = context->match("host");
	  context->writer() << "Resolving " << host << "\n";
          resolver->async_resolve(host, [context](
            pion::stdx::error_code const& err,
	    pion::stdx::asio::ip::tcp::resolver::iterator endpoint) {

              const pion::stdx::asio::ip::tcp::resolver::iterator done;
	      while (endpoint != done) {
		context->writer() << endpoint->endpoint() << "\n";
		++endpoint;
	      }
	      context->writer() << "Finished resolution: " << err.message() <<"\n";
	      context->writer()->send();
	  });

	  return dslam::defer_response;
      }},
    });
  }
} // namespace

int main(int argc, char* argv[]) {

    //pion::logger::m_priority = pion::logger::LOG_LEVEL_WARN;

  namespace po = boost::program_options;
  po::options_description options("Allowed options");
  options.add_options()
    ("help", "produce help message")
    ("port", po::value<std::uint16_t>(), "server listen port");

  po::variables_map variables;
  po::store(po::parse_command_line(argc, argv, options), variables);
  po::notify(variables);

  if (variables.count("help")) {
    std::cout << options << "\n";
    return EXIT_SUCCESS;
  }

  if (variables.count("port") == 0) {
    std::cout << "No listen port specified." << "\n" << options << "\n";
    return EXIT_FAILURE;
  }

  auto const port = variables["port"].as<std::uint16_t>();
  pion::stdx::asio::ip::tcp::endpoint const where(
    pion::stdx::asio::ip::tcp::v4(), port);

  // Explicitly single threaded server
  auto const scheduler = dslam::make_unique<pion::one_to_one_scheduler>();
  scheduler->set_num_threads(1);

  auto const server = dslam::make_unique<pion::http::plugin_server>(*scheduler, where);

  auto const service = make_service(*scheduler);
  server->add_service("/api", service.get());
  server->start();

  pause();

  server->stop();
  return EXIT_SUCCESS;
}
