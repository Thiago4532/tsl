#ifndef _TSL_DEFER_HPP
#define _TSL_DEFER_HPP

#include <utility>


namespace tsl::internal {

template <typename F>
class deferrer {
public:
    deferrer(F const& f) : f_(f) {}
    deferrer(F&& f) : f_(std::move(f)) {}

    ~deferrer() { f_(); }
private:
    F f_;
};

} // namespace tsl::internal

#define TSL_INTERNAL_DEFER_CONCAT_IMPL(x, y) x##y
#define TSL_INTERNAL_DEFER_CONCAT(x, y) TSL_INTERNAL_DEFER_CONCAT_IMPL(x, y)

// TSL_DEFER
//
// A macro to execute a block of code at the end of the current scope, similar to `defer` in Go.
// Use it like this: `TSL_DEFER { /* code to defer */ };`.
#define TSL_DEFER \
    ::tsl::internal::deferrer TSL_INTERNAL_DEFER_CONCAT(_tsl_defer__, __LINE__) = [&]()

#endif // _TSL_DEFER_HPP
