#include <dslam/predicates.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <pion/net/HTTPTypes.hpp>

#include <iostream>

namespace dslam {
  namespace predicates {

    namespace ba = boost::algorithm;
    auto request::make_resource_matcher(std::string const& resource_pattern) -> service::predicate {

      // If the :variable special form is present, convert to a regex.
      if (std::count(resource_pattern.begin(), resource_pattern.end(), ':') != 0) {

        std::vector<std::string> split_results;
        ba::split(split_results, resource_pattern, ba::is_any_of("/"));

        service::context::match_dictionary capture_names;
        size_t index = 1;
	for(auto& element : split_results) {
	  if (element[0] == ':') {
            element.erase(0, 1);
            capture_names.emplace(element, index++);
	    element = "(.*)";
	  }
	}

	std::string reformed_pattern = "^" + ba::join(split_results, "/");
        auto base_matcher = make_resource_matcher(std::regex(reformed_pattern));

        return service::predicate([base_matcher, capture_names](service::context_ptr const& context) -> bool {
          bool result = base_matcher(context);
          if (result) {
             context->set_match_dictionary(&capture_names);
          }
          return result;
        });
      }

      // No special forms, just match on string identity...
      return service::predicate([resource_pattern](service::context_ptr const& context) -> bool {
	bool const match = resource_pattern == context->resource();
	return match;
      });

    }

    auto request::make_resource_matcher(std::regex const& resource_pattern) -> service::predicate {
      return service::predicate([resource_pattern](service::context_ptr const& context) -> bool {
        // NOTE(C++11): Using regex_match to check for a match and capture captures.
	const auto matched = std::regex_match(context->resource(), context->matches(), resource_pattern);
	return matched;
      });
    }

  } // namespace predicates
} // namespace dslam
