#include <chrono>
#include <cstdlib>
#include <thread>
#include <unordered_map>

#include <boost/asio/ip/tcp.hpp>

#include <pion/net/WebServer.hpp>

#include <dslam/make_unique.hpp>
#include <dslam/predicates.hpp>
#include <dslam/service.hpp>

namespace {

  // NOTE(C++11): New style function signature
  // NOTE(C++11): unique_ptr
  auto make_service(pion::PionScheduler& scheduler) -> std::unique_ptr<dslam::service> {

    // NOTE(C++11): New style typdefs.
    using context = dslam::service::context_ptr;
    using result = dslam::route_result;

    // NOTE(C++11): Type inference and 'auto' keyword
    auto const& get = dslam::predicates::http::verbs::get;
    auto const& post = dslam::predicates::http::verbs::post;

    struct service_state {

      void record_hit(context const& context) {
	++hits[context->request()->getResource()];
      }

      // NOTE(C++11): unordered_map
      using hitmap = std::unordered_map<std::string, uint64_t>;
      hitmap hits;
    };

    // NOTE(C++11): make_shared
    auto state = std::make_shared<service_state>();
    auto resolver = std::make_shared<boost::asio::ip::tcp::resolver>(scheduler.getIOService());

    return std::unique_ptr<dslam::service>(new dslam::service{

      // NOTE(C++11): brace initializers
      // NOTE(C++11): lambdas and captures
      { true, [state](context const& context) -> result {
	  state->record_hit(context);
	  return dslam::next_handler;
      }},

      { "hello", get or post, [state](context const& context) -> result {
	  return "Hello, World!\n";
      }},

      { "hitcounter", get, [state](context const& context) -> result {
	  auto writer = context->writer();
	  // NOTE(C++11): New style for loop
	  for (auto const& hit : state->hits)
	    writer << hit.first << ": " << hit.second << "\n";
	  return writer;
      }},

      // NOTE(C++11): Built in regular expressions
      // NOTE(C++11): Raw string literals
      { std::regex(R"(^rtest/(.*)/(.*))"), get, [state](context const& context) -> result {
	  auto writer = context->writer();
	  writer << "First param: " << context->matches()[1] << "\n";
	  writer << "Second param: " << context->matches()[2] << "\n";
	  return writer;
      }},

      { "users/:name", get or post, [state](context const& context) -> result {
	  auto writer = context->writer();
	  writer->write("Hello, ");
	  writer->write(context->match("name"));
	  writer->write("!\n");
	  return writer;
      }},

      // NOTE(acm): A misbehaving route that blocks the IO thread. Don't do this!
      // NOTE(C++11): std::chrono, and std::this_thread
      { "block/:duration", get, [state](context const& context) -> result {
	  auto const duration = boost::lexical_cast<int>(context->match("duration"));
	  std::this_thread::sleep_for(std::chrono::seconds(duration));
	  return "Done sleeping\n";
      }},

      // NOTE(acm): Instead, do it from a separate thread.
      // NOTE(C++11): Starting a thread with std::thread constructor and a lambda
      { "block_in_thread/:duration", get, [state](context const& context) -> result {
	  auto task = [context]() {
	    auto const duration = boost::lexical_cast<int>(context->match("duration"));
	    std::this_thread::sleep_for(std::chrono::seconds(duration));
	    context->dispatch_result("Done sleeping in thread\n");
	  };

	  std::thread t(task);
	  t.detach();

	  return dslam::defer_response;
      }},

      { "block_in_thread2/:duration", get, [state](context const& context) -> result {
	  context->run_in_thread([context]() -> result {
	    auto const duration = boost::lexical_cast<int>(context->match("duration"));
	    std::this_thread::sleep_for(std::chrono::seconds(duration));
	    return "Done sleeping in thread\n";
	  });
	  return dslam::defer_response;
      }},

      { "chunks", get or post, [state](context const& context) -> result {
	  auto writer = context->writer();
	  writer->write("Some stuff\n");
	  writer->sendChunk([context](boost::system::error_code const& error, std::size_t written) {
	    context->writer()->clear();
	    context->writer()->write("More stuff!\n");
	    context->writer()->sendFinalChunk();
	  });

	  return dslam::defer_response;
      }},

      { "resolve/:host", get, [resolver](context const& context) -> result {
	  const std::string host = context->match("host");
	  context->writer() << "Resolving " << host << "\n";
          resolver->async_resolve(host, [context](
	    boost::system::error_code const& err,
	    boost::asio::ip::tcp::resolver::iterator endpoint) {

	      const boost::asio::ip::tcp::resolver::iterator done;
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

  pion::PionLogger::m_priority = pion::PionLogger::LOG_LEVEL_WARN;

  boost::asio::ip::tcp::endpoint const where(
    boost::asio::ip::tcp::v4(),
    atoi(argv[1])); // sue me

  // Explicitly single threaded server
  auto const scheduler = dslam::make_unique<pion::PionOneToOneScheduler>();
  scheduler->setNumThreads(1);

  auto const server = dslam::make_unique<pion::net::WebServer>(*scheduler, where);

  auto const service = make_service(*scheduler);
  server->addService("/api", service.get());
  server->start();

  pause();

  server->stop();
  return EXIT_SUCCESS;
}
