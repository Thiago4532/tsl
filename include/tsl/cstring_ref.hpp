// A C-String reference
// A lightweight string_view for C-Strings, it only encapsulates a pointer
// to the start of the string, operations like length() are computated in O(n) instead
// of a variable that keeps the information.
// Unlike std::string_view, strings must terminate with '\0'
#ifndef _TSL_CSTRING_VIEW_HPP
#define _TSL_CSTRING_VIEW_HPP

#include <cstring>
#include <string>
#include "tsl/attributes.hpp"
#include "tsl/macros.hpp"

namespace tsl {

class cstring_ref {
public:
    constexpr cstring_ref() noexcept: str_("") {}
    constexpr cstring_ref(const char* s TSL_ATTR_LIFETIMEBOUND)
        : str_(TSL_ASSERT_NONNULL(s)) {}
    constexpr cstring_ref(std::string const& s TSL_ATTR_LIFETIMEBOUND)
        : str_(s.c_str()) {}
    constexpr cstring_ref(std::nullptr_t) = delete;

    constexpr cstring_ref& operator=(const char* s) {
        str_ = TSL_ASSERT_NONNULL(s);
        return *this;
    }
    constexpr cstring_ref& operator=(std::nullptr_t) = delete;

    constexpr cstring_ref& operator=(std::string const& s) {
        str_ = s.c_str();
        return *this;
    }

    [[nodiscard]] constexpr const char* get() const noexcept {
        return str_;
    }

    // Computes the length of the view in O(n)
    [[nodiscard]] constexpr std::size_t length() const {
        if (std::is_constant_evaluated()) {
            std::size_t len = 0;
            while (str_[len] != '\0')
                ++len;
            return len;
        } else {
            return std::strlen(str_);
        }
    }
private:
    const char* str_;
};

}

#endif // _TSL_CSTRING_VIEW_HPP
