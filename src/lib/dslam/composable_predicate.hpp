#ifndef included_f87937c9_92ae_4503_a931_270f48992518
#define included_f87937c9_92ae_4503_a931_270f48992518

#include <functional>

namespace dslam {

template <typename>
class composable_predicate;

// NOTE(C++11): Variadic templates
template <typename... ArgTypes>
class composable_predicate<bool(ArgTypes...)> {
public:
    using predicate_type = std::function<bool(ArgTypes...)>;

    composable_predicate() = default;

    composable_predicate(predicate_type const& predicate) : predicate_(predicate) {}

    // NOTE(C++11): rvalue references
    composable_predicate(predicate_type&& predicate) : predicate_(predicate) {}

    ~composable_predicate() = default;

    // NOTE(C++11): Argument packs and std::forward
    bool operator()(ArgTypes&&... args) const {
        return predicate_(std::forward<ArgTypes>(args)...);
    }

private:
    predicate_type predicate_;
};

// NOTE(C++11): All of the above, plus lambdas and captures :)
template <typename... ArgTypes>
composable_predicate<bool(ArgTypes...)> operator or(
    composable_predicate<bool(ArgTypes...)> const& l,
    composable_predicate<bool(ArgTypes...)> const& r) {
    return composable_predicate<bool(ArgTypes...)>([l, r](ArgTypes&&... args) {
        return l(std::forward<ArgTypes>(args)...) or r(std::forward<ArgTypes>(args)...);
    });
}

template <typename... ArgTypes>
composable_predicate<bool(ArgTypes...)> operator and(
    composable_predicate<bool(ArgTypes...)> const& l,
    composable_predicate<bool(ArgTypes...)> const& r) {
    return composable_predicate<bool(ArgTypes...)>([l, r](ArgTypes&&... args) {
        return l(std::forward<ArgTypes>(args)...) and r(std::forward<ArgTypes>(args)...);
    });
}

template <typename... ArgTypes>
composable_predicate<ArgTypes...> operator not(composable_predicate<ArgTypes...> const& p) {
    return composable_predicate<ArgTypes...>(
        [p](ArgTypes&&... args) { return not p(std::forward<ArgTypes>(args)...); });
}

}  // namespace dslam

#endif  // included_f87937c9_92ae_4503_a931_270f48992518
