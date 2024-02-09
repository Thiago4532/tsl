// Contract types, used by tsl::maybe to allow optimizations

#ifndef _TSL_TYPES_CONTRACTS_HPP
#define _TSL_TYPES_CONTRACTS_HPP

#include <concepts>
#include <utility>
#include "tsl/types/unchecked.hpp"

namespace tsl {

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
    requires std::derived_from<T, contract_base>;
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

}

#endif // _TSL_TYPES_CONTRACTS_HPP
