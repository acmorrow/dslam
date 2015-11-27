#include <dslam/service.hpp>

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <dslam/predicates.hpp>

namespace dslam {

  using pion::http::request_ptr;
  using pion::http::response_writer;
  using pion::http::response_writer_ptr;
  using pion::http::writer;
  using pion::tcp::connection_ptr;

  namespace {
    void do_finish(connection_ptr& connection, pion::stdx::error_code const& error_code) {
      connection->finish();
    }
  } // namespace

  service::service(std::initializer_list<route> routes)
    : routes_(routes) {}

  void service::operator()(const request_ptr& request, const connection_ptr& connection) {
    auto ctx = std::make_shared<context>(*this, request, connection);
    for (auto const& route : routes_)
      if (route.accepts(ctx) and evaluate(route, ctx))
	return;

    // Nothing matched, return a 404.
    ctx->writer()->get_response().set_status_code(pion::http::types::RESPONSE_CODE_NOT_FOUND);
    ctx->writer()->get_response().set_status_message(pion::http::types::RESPONSE_MESSAGE_NOT_FOUND);
    ctx->writer()->send();
  }

  auto service::evaluate(route const& r, context_ptr const& c) -> bool {
    return c->dispatch_result(r(c));
  }

  service::context::context(class service& service,
			    const request_ptr& request,
			    const connection_ptr& connection)
    // NOTE(C++11): Brace initializers
    : service_{service}
    , request_{request}
    , connection_{connection}
    , writer_{response_writer::create(connection_, *request_, std::bind(&do_finish, connection_, std::placeholders::_1))}
    , resource_{service_.get_relative_resource(request_->get_resource())}
    , matches_{}
    , match_dictionary_{nullptr} {}

  // NOTE(C++11): Default destructor (note that it is out of line).
  service::context::~context() = default;

  auto service::context::match(const std::string& name) const -> std::smatch::value_type const& {
    // TODO(acm): This needs considerably more error handling.
    assert(match_dictionary_);
    auto where = match_dictionary_->find(name);
    assert(where != match_dictionary_->end());
    return matches_[where->second];
  }

  auto service::context::dispatch_result(route_result const& result) -> bool {

    struct visitor : public boost::static_visitor<bool> {
      visitor(context* c)
	: context_{c} {}

      auto operator()(boost::blank const&) const -> bool {
	// Route handled all IO, nothing to do.
	return true;
      }

      auto operator()(next_handler_tag const&) const -> bool {
	// Route is deferring to the next matching handler.
	return false;
      }

      auto operator()(std::string const& string) const -> bool {
	// Route has returned a string. We must copy send the data for
	// them, and finish the connection.
	context_->writer()->write(string);
	context_->writer()->send();
	return true;
      }

      auto operator()(char const* string) const -> bool {
	context_->writer()->write(string);
	context_->writer()->send();
	return true;
      }

      auto operator()(pion::http::response_writer_ptr const& writer) const -> bool {
	// They gave us back a writer, which we assume is properly
	// configured. We just take care of the call to send for them.
	writer->send();
	return true;
      }

      context const* context_;
    } const v(this);

    return boost::apply_visitor(v, result);
  }

  void service::context::run_in_thread(const task& task) {
    // NOTE(C++11): Launching a thread on a lambda, capturing this.
    std::thread t([this, task]{
      this->dispatch_result(task());
    });
    t.detach();
  }

  // NOTE(C++11): Delegating constructors.
  service::route::route(bool value, handler const& handler)
    : route(predicate([value](context_ptr const&){return value;}), handler) {}

  service::route::route(std::string const& resource, predicate const& filter, handler const& handler)
    : route(predicates::request::make_resource_matcher(resource) and filter, handler) {}

  service::route::route(std::regex const& resource, predicate const& filter, handler const& handler)
    : route(predicates::request::make_resource_matcher(resource) and filter, handler) {}

  service::route::route(predicate const& filter, handler const& handler)
    : filter_(filter)
    , handler_(handler) {}

  auto service::route::accepts(context_ptr const& context) const -> bool {
    return filter_(context);
  }

  auto service::route::operator()(context_ptr const& context) const -> route_result {
    return handler_(context);
  }

} // namespace dslam
