#ifndef included_6ca26eb5_9655_4282_993d_7c5753c45544
#define included_6ca26eb5_9655_4282_993d_7c5753c45544

// http://www.quickmeme.com/meme/3q0ztq/

#include <functional>
#include <future>
#include <initializer_list>
#include <memory>
#include <regex>
#include <unordered_map>
#include <vector>

#include <boost/variant/variant.hpp>

#include <pion/net/HTTPResponse.hpp>
#include <pion/net/HTTPRequest.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/TCPConnection.hpp>
#include <pion/net/WebService.hpp>

#include <dslam/composable_predicate.hpp>
#include <dslam/export.hpp>

namespace dslam {

  struct LIBDSLAM_EXPORT next_handler_tag {};

  using route_result = boost::variant<
    boost::blank,
    next_handler_tag,
    std::string,
    char const*,
    pion::net::HTTPResponseWriterPtr>;

  LIBDSLAM_EXPORT
  route_result const next_handler = next_handler_tag();

  LIBDSLAM_EXPORT
  route_result const defer_response = boost::blank();

  class LIBDSLAM_EXPORT service : public pion::net::WebService {
  public:

    class LIBDSLAM_EXPORT context;
    using context_ptr = std::shared_ptr<context>;

    using predicate = composable_predicate<bool(context_ptr const&)>;
    // NOTE(C++11): We now have std::function rather than boost::function.
    using handler = std::function<route_result(context_ptr const&)>;

    class LIBDSLAM_EXPORT route;
    // NOTE(C++11): An initializer list constructor.
    service(std::initializer_list<route> routes);

    virtual
    void operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection);

  private:
    LIBDSLAM_NO_EXPORT
    auto evaluate(route const&, context_ptr const& context) -> bool;

    using route_table = std::vector<route>;
    route_table const routes_;
  };

  class service::context {
    friend class service;

  public:
    using match_dictionary = std::unordered_map<std::string, std::size_t>;

    context(class service& service,
	    pion::net::HTTPRequestPtr& request,
	    pion::net::TCPConnectionPtr& connection);

    ~context();

    auto service() -> class service& { return service_; }
    auto service() const -> class service const& { return service_; }

    auto request() const -> pion::net::HTTPRequestPtr const& { return request_; }

    auto resource() const -> std::string const& { return resource_; }

    auto writer() const -> pion::net::HTTPResponseWriterPtr const& { return writer_; }

    auto matches() -> std::smatch& { return matches_; }
    auto matches() const -> std::smatch const& { return matches_; }

    void set_match_dictionary(match_dictionary const* dictionary) {
      match_dictionary_ = dictionary;
    }

    auto match(std::string const& name) const -> std::smatch::value_type const&;

    auto dispatch_result(route_result const& result) -> bool;

    using task = std::function<route_result()>;
    void run_in_thread(const task& task);

  private:
    class service& service_;
    pion::net::HTTPRequestPtr& request_;
    pion::net::TCPConnectionPtr& connection_;
    pion::net::HTTPResponseWriterPtr writer_;
    std::string resource_;
    std::smatch matches_;
    match_dictionary const* match_dictionary_;
  };

  class service::route {
  public:
    route(bool value, handler const& handler);
    route(predicate const& filter, handler const& handler);
    route(std::string const& resource, predicate const& filter, handler const& handler);
    route(std::regex const& resource, predicate const& filter, handler const& handler);

    auto accepts(context_ptr const& context) const -> bool;
    auto operator()(context_ptr const& context) const -> route_result;

  private:
    predicate const filter_;
    handler const handler_;
  };

} // namespace dslam

#endif // included_6ca26eb5_9655_4282_993d_7c5753c45544
