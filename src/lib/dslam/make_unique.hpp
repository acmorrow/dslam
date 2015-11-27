#ifndef included_469f8eba_765e_4ebb_8827_83dda417d2d7
#define included_469f8eba_765e_4ebb_8827_83dda417d2d7

#include <memory>

namespace dslam {

// Like make shared, but really just sugar
// NOTE(C++11): More variadics, plus argument packs, plus std forward.
template <typename T, typename... ArgTypes>
auto make_unique(ArgTypes&&... args) -> std::unique_ptr<T> {
    return std::unique_ptr<T>(new T(std::forward<ArgTypes>(args)...));
}

}  // namespace dslam

#endif  // included_469f8eba_765e_4ebb_8827_83dda417d2d7
