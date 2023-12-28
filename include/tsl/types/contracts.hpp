// Contract types, used by tsl::maybe to allow optimizations

#ifndef _TSL_TYPES_CONTRACTS_HPP
#define _TSL_TYPES_CONTRACTS_HPP

#include <iostream>

#include <concepts>
#include <utility>
#include "tsl/macros.hpp"
#include "tsl/types/unchecked.hpp"

namespace tsl {

template<typename T>
struct contract_base {};

struct contract_breach_t {
    // Using the same trick as std::nullopt_t,
    // to prevent construction using only {}
    enum class _construct { _token };

    constexpr contract_breach_t(_construct) noexcept { }
};

inline constexpr contract_breach_t contract_breach { contract_breach_t::_construct::_token };

template<typename T>
concept ContractType = requires (T t) {
    requires std::derived_from<T, contract_base<T>>;
    typename T::type;

    T(contract_breach);
    T(std::declval<typename T::type>());
    T(unchecked, std::declval<typename T::type>());

    { t = contract_breach } -> std::same_as<T&>;

    { std::as_const(t).is_valid() } -> std::same_as<bool>;

    { t.raw() } -> std::same_as<typename T::type&>;
    { std::as_const(t).raw() } -> std::same_as<typename T::type const&>;
    { std::move(t).raw() } -> std::same_as<typename T::type&&>;
    { std::move(std::as_const(t)).raw() } -> std::same_as<typename T::type const&&>;
};

namespace internal_types {

template<std::signed_integral T>
class non_negative_impl : public contract_base<non_negative_impl<T>> {
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

#endif // _TSL_TYPES_CONTRACTS_HPP
