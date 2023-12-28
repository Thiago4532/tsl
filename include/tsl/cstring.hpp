// Implementations of cstring functions along with constexpr versions.
// Fallback to cstring if not constexpr.

#ifndef _TSL_STRING_LIB_HPP
#define _TSL_STRING_LIB_HPP

#include <cstring>
#include <type_traits>

namespace tsl {

constexpr std::size_t strlength(const char* str) {
    if (std::is_constant_evaluated()) {
        std::size_t len = 0;
        while (str[len] != '\0') {
            ++len;
        }
        return len;
    } else {
        return std::strlen(str);
    }
}

constexpr char* strcopy(char* dest, const char* src) {
    if (std::is_constant_evaluated()) {
        std::size_t i = 0;
        while (src[i] != '\0') {
            dest[i] = src[i];
            ++i;
        }
        dest[i] = '\0';
        return dest;
    } else {
        return std::strcpy(dest, src);
    }
}

}

#endif // _TSL_STRING_LIB_HPP
