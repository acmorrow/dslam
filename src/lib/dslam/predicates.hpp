#ifndef included_e367d4da_9139_43ed_bc6c_1d96a0dd2e78
#define included_e367d4da_9139_43ed_bc6c_1d96a0dd2e78

#include <regex>
#include <string>

#include <pion/http/types.hpp>

#include <dslam/export.hpp>
#include <dslam/service.hpp>

namespace dslam {
  namespace predicates {

    namespace http {
      namespace verbs {
	LIBDSLAM_EXPORT
        service::predicate const get([](service::context_ptr const& context) {
          return context->request()->get_method() == pion::http::types::REQUEST_METHOD_GET;
	});

	LIBDSLAM_EXPORT
	service::predicate const post([](service::context_ptr const& context) {
          return context->request()->get_method() == pion::http::types::REQUEST_METHOD_POST;
	});
      } // namespace verbs
    } // namespace http

    namespace request {

      LIBDSLAM_EXPORT
      auto make_resource_matcher(std::string const& resource_pattern) -> service::predicate;

      // NOTE(C++11): Builtin regular expressions
      LIBDSLAM_EXPORT
      auto make_resource_matcher(std::regex const& resource_pattern) -> service::predicate;

    } // namespace request

  } // namespace predicates
} // namespace dslam

#endif // included_e367d4da_9139_43ed_bc6c_1d96a0dd2e78
