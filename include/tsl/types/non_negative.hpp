#ifndef _TSL_TYPES_NON_NEGATIVE_HPP
#define _TSL_TYPES_NON_NEGATIVE_HPP

#include "tsl/types/contracts.hpp"
#include "tsl/macros.hpp"

namespace tsl {

namespace internal_types {

template<std::signed_integral T>
class non_negative_impl : public contract_base {
public:
    using type = T;
    constexpr non_negative_impl() = default;

    template<std::constructible_from<T> U>
    constexpr non_negative_impl(U&& v) : val_(std::forward<U>(v)) {
        TSL_HARDENING_ASSERT(is_valid());
    }

    template<std::constructible_from<T> U>
    constexpr non_negative_impl(unchecked_t, U&& v) : val_(std::forward<U>(v)) {}

    constexpr non_negative_impl(contract_breach_t) : val_(-1) {}

    constexpr bool is_valid() const {
        return val_ >= 0;
    }

    constexpr T& raw() & {
        return val_;
    }

    constexpr T const& raw() const& {
        return val_;
    }

    constexpr T&& raw() && {
        return std::move(val_);
    }

    constexpr T const&& raw() const&& {
        return std::move(val_);
    }

    operator T&() & {
        return val_;
    }

    operator T const&() const& {
        return val_;
    }

    operator T&&() && {
        return std::move(val_);
    }

    operator T const&&() const&& {
        return std::move(val_);
    }
private:
    T val_;
};

}

template<std::signed_integral T> requires ContractType<internal_types::non_negative_impl<T>>
using non_negative = internal_types::non_negative_impl<T>;

}

#endif // _TSL_TYPES_NON_NEGATIVE_HPP
