#ifndef _TSL_LITERAL_STRING_HPP
#define _TSL_LITERAL_STRING_HPP

#include <cstddef>
#include <algorithm>
#include "cstring.hpp"
#include "tsl/macros.hpp"

namespace tsl {

template<std::size_t N>
struct literal_string {
    char data[N];

    constexpr literal_string() noexcept : data {0} {}

    template<typename T> requires std::indirectly_copyable<std::ranges::iterator_t<T>, char*>
    constexpr literal_string(T&& str) {
        std::ranges::copy(std::forward<T>(str), data);
    }

    constexpr literal_string(const char* str, std::size_t len) {
        TSL_ASSUME(len < N);
        std::ranges::copy_n(str, len, data);
        data[len] = 0;
    }
};

template<std::size_t N>
literal_string(const char (&)[N]) -> literal_string<N>;

}

#endif // _TSL_LITERAL_STRING_HPP
