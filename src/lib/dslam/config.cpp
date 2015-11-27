#include <dslam/config.hpp>

namespace dslam {

void get_library_version(std::size_t* major, std::size_t* minor, std::size_t* patch) {
    if (major)
        *major = library_version_major;
    if (minor)
        *minor = library_version_minor;
    if (patch)
        *patch = library_version_patch;
}

}  // namespace dslam
