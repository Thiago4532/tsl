#ifndef _TSL_MACROS_HPP
#define _TSL_MACROS_HPP

#include <cstdlib>
#include "tsl/internal/abort.hpp"
#include "tsl/config.hpp"

#ifdef __has_builtin
#define TSL_HAVE_BUILTIN(x) __has_builtin(x)
#else
#define TSL_HAVE_BUILTIN(x) 0
#endif

// Macros to help with boolean expressions
// The C way of doing this is to use !!(x), but the operator ! may be overloaded
#define TSL_BOOL(x) (static_cast<bool>(x))
#define TSL_NOT(x)  (!static_cast<bool>(x))

// TODO: Replace usages of TSL_EXPECT to [[likely]] and [[unlikely]]
//       Tests in GCC 13.2.1 performed better using __builtin_expect instead
//       of the standard one, so we are going to use this until it's fixed.
#if TSL_HAVE_BUILTIN(__builtin_expect)
#define TSL_EXPECT_TRUE(x) (__builtin_expect(TSL_BOOL(x), true))
#define TSL_EXPECT_FALSE(x) (__builtin_expect(TSL_BOOL(x), false))
#else
#define TSL_EXPECT_TRUE(x) TSL_BOOL(x)
#define TSL_EXPECT_FALSE(x) TSL_BOOL(x)
#endif

// `TSL_FAST_ABORT()` aborts the program as fast as possible. Without
// printing a message or doing anything else.
#if TSL_HAVE_BUILTIN(__builtin_trap)
#define TSL_FAST_ABORT() __builtin_trap()
#elif defined(_MSC_VER)
#define TSL_FAST_ABORT() ::__debugbreak()
#else
#define TSL_FAST_ABORT() ::std::abort()
#endif

#define TSL_ABORT(msg) \
    (::tsl::internal::abort_message(msg), TSL_FAST_ABORT())

#define TSL_ABORT2(msg, location) \
    (::tsl::internal::abort_message(msg, location), TSL_FAST_ABORT())

#define TSL_INTERNAL_ASSERT_FAIL(expr) \
    TSL_ABORT("Assertion '" #expr "' failed.")

#ifndef NDEBUG
#define TSL_ASSERT(expr) \
    (TSL_EXPECT_TRUE(expr) ? static_cast<void>(0) \
                           : TSL_INTERNAL_ASSERT_FAIL(expr))
#else
#define TSL_ASSERT(expr) static_cast<void>(0)
#endif

// TSL_HARDENING_ASSERT()
//
// Like `TSL_ASSERT()`, but implement runtime assertions in hardening
// builds even if `NDEBUG` is defined. Sometimes may be disabled when `NDEBUG`
// is not defined, to improve performance of debug builds.
//
// Hardening builds are not implemented yet. So it's the same as `TSL_ASSERT()`.
#define TSL_HARDENING_ASSERT(expr) TSL_ASSERT(expr)

#if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
#define TSL_INTERNAL_UNREACHABLE() ::std::unreachable()
#elif TSL_HAVE_BUILTIN(__builtin_unreachable)
#define TSL_INTERNAL_UNREACHABLE() __builtin_unreachable()
#elif TSL_HAVE_BUILTIN(__builtin_assume)
#define TSL_INTERNAL_UNREACHABLE() __builtin_assume(false)
#elif defined(_MSC_VER)
#define TSL_INTERNAL_UNREACHABLE() __assume(false)
#else
#define TSL_INTERNAL_UNREACHABLE()
#endif

#define TSL_UNREACHABLE() do { \
    TSL_ASSERT(false && "TSL_UNREACHABLE reached"); \
    TSL_INTERNAL_UNREACHABLE(); \
} while (0)

#ifndef NDEBUG
#define TSL_ASSUME(expr) do { TSL_ASSERT(expr); } while(0)
#elif TSL_HAVE_BUILTIN(__builtin_assume)
#define TSL_ASSUME(expr) do { __builtin_assume(expr); } while(0)
#elif defined(_MSC_VER)
#define TSL_ASSUME(expr) do { __assume(expr); } while(0)
#else
#define TSL_ASSUME(expr) do { \
    if (TSL_NOT(expr)) { \
        TSL_INTERNAL_UNREACHABLE(); \
    } \
} while (0)
#endif

// TSL_THROW()
//
// Throw an exception, if exceptions are disabled, calls `TSL_ABORT()`.
// The exception must be a subclass of `std::exception` (or implement `what()`).
//
// TODO: Decide if this should be a macro or a function. Also,
// this may be a ODR violations, but I'm not sure yet, read Abseil throw_delegate.h
// for more information.
//
// TODO: Detect if what() is implemented.
#if TSL_HAS_EXCEPTIONS
#define TSL_THROW(exception) static_cast<void>(throw exception)
#else
#define TSL_THROW(exception) static_cast<void>(TSL_ABORT(exception.what()))
#endif

// TSL_ASSERT_NONNULL()
//
// Assert that a pointer is not null. Using hardening assertions, because
// null pointer dereferences are undefined behavior.
//
// The reason why this is a macro and not a function is that we want to
// guarantee that copy-ellision optimizations are applied.
// Copy-ellision may not be important to raw pointers, but it's important to smart pointers.
#define TSL_ASSERT_NONNULL(ptr) \
    (TSL_HARDENING_ASSERT(ptr != nullptr), ptr)

// The following macros are used to avoid repetition of keywords,
// making the code more readable.
#define TSL_REQUIRES requires requires
#define TSL_NOEXCEPT(expr) noexcept(noexcept(expr))

#endif // _TSL_MACROS_HPP
