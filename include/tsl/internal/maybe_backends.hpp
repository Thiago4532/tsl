// Internal header. Do not include directly.
#pragma once

// IWYU pragma: private: include "tsl/maybe.hpp"

#include "tsl/maybe.hpp"
#include "tsl/types/contracts.hpp"

namespace tsl {

/// Backend for contract types

template<typename T, T sentinel>
class maybe_backend_sentinel {
public:
    using value_type = T;
    static constexpr bool allow_unchecked_value = true;

    constexpr maybe_backend_sentinel() noexcept:
        value_(sentinel) { }

    template<typename... Args>
    constexpr maybe_backend_sentinel(Args&&... args):
        value_(std::forward<Args>(args)...) { }

    template<typename... Args>
    constexpr void construct(Args&&... args) {
        std::construct_at(std::addressof(value_), std::forward<Args>(args)...);
    }

    constexpr bool has_value() const noexcept {
        return value_ != sentinel;
    }

    constexpr std::size_t& get() noexcept {
        return value_;
    }

    constexpr std::size_t const& get() const noexcept {
        return value_;
    }

    constexpr void destruct() noexcept {
        value_ = sentinel;
    }

private:
    T value_;
};

template<typename T> class maybe_backend_contract {
public:
    using value_type = T;
    static constexpr bool allow_unchecked_value = true;

    constexpr maybe_backend_contract() noexcept:
        value_(contract_breach) { }

    template<typename... Args>
    constexpr maybe_backend_contract(Args&&... args):
        value_(std::forward<Args>(args)...) { }

    template<typename... Args>
    constexpr void construct(Args&&... args) {
        std::construct_at(std::addressof(value_), std::forward<Args>(args)...);
    }

    constexpr bool has_value() const noexcept {
        return value_.is_valid();
    }

    constexpr T& get() noexcept {
        return value_;
    }

    constexpr T const& get() const noexcept {
        return value_;
    }

    constexpr void destruct() noexcept {
        value_ = contract_breach;
    }

private:
    T value_;
};

template<ContractType T>
struct maybe_backend_default_t<T> {
    using type = maybe_backend_contract<T>;
};

}
