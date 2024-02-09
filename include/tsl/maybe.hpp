#ifndef _TSL_MAYBE_HPP
#define _TSL_MAYBE_HPP

#include <exception>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>
#include <compare>
#include <concepts>
#include "tsl/config.hpp"
#include "tsl/concepts.hpp"
#include "tsl/macros.hpp"
#include "tsl/types/contracts.hpp"

namespace tsl {

class bad_maybe_access : public std::exception {
public:
    bad_maybe_access() = default;
    ~bad_maybe_access() override = default;

    const char* what() const noexcept override {
        return "bad maybe access";
    }
};

template<typename BackendType>
class maybe_base;

namespace internal_maybe {

struct EmptyByte { };

template<typename T>
union Storage {
    constexpr Storage() noexcept : empty_() { }

    template<typename... Args>
    constexpr Storage(Args&&... args):
        value_(std::forward<Args>(args)...) { }

    constexpr ~Storage() = default;

    // User-defined destructors are required for non-trivially destructible types.
    constexpr ~Storage() requires(!trivially_destructible<T>) { }

    EmptyByte empty_;
    T value_;
};

template<typename T, typename U>
concept converts_from =
    std::constructible_from<T, U&>
    || std::constructible_from<T, U const&>
    || std::constructible_from<T, U&&>
    || std::constructible_from<T, U const&&>
    || std::convertible_to<U&, T>
    || std::convertible_to<U const&, T>
    || std::convertible_to<U&&, T>
    || std::convertible_to<U const&&, T>;

template<typename T, typename U>
concept assigns_from =
    std::assignable_from<T&, U&>
    || std::assignable_from<T&, U const&>
    || std::assignable_from<T&, U&&>
    || std::assignable_from<T&, U const&&>;

}

template<typename T>
class maybe_backend_general {
public:
    using value_type = T;
    static constexpr bool allow_unchecked_value = false;

    constexpr maybe_backend_general() noexcept:
        storage_(), engaged_(false) { }

    template<typename... Args>
    constexpr maybe_backend_general(Args&&... args):
        storage_(std::forward<Args>(args)...), engaged_(true) { }

    template<typename... Args>
    constexpr void construct(Args&&... args) {
        std::construct_at(std::addressof(storage_.value_), std::forward<Args>(args)...);
        engaged_ = true;
    }

    constexpr bool has_value() const noexcept {
        return engaged_;
    }

    constexpr T& get() noexcept {
        return storage_.value_;
    }

    constexpr T const& get() const noexcept {
        return storage_.value_;
    }

    constexpr void destruct() noexcept {
        engaged_ = false;
        storage_.value_.~T();
    }

private:
    internal_maybe::Storage<T> storage_;
    bool engaged_;
};

template<typename T>
struct maybe_backend_default_t {
    using type = maybe_backend_general<T>;
};

template<typename T>
using maybe_backend_default = typename maybe_backend_default_t<T>::type;

template<typename BackendType>
class maybe_base {
public:
    using T = BackendType::value_type;
    using value_type = T;
    static constexpr bool allow_unchecked_value = BackendType::allow_unchecked_value;

    constexpr maybe_base() { }

    // TODO: nullopt_t equivalent for maybe
    // constexpr maybe_base(nullopt_t) { }

    constexpr maybe_base(maybe_base const&) = delete;
    constexpr maybe_base(maybe_base const&)
        requires(trivially_copy_constructible<T>) = default;

    constexpr maybe_base(maybe_base const& rhs)
        noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires(std::copy_constructible<T>)
    {
        if (rhs.has_value())
            copy_construct(*rhs);
    }

    constexpr maybe_base(maybe_base&& rhs)
        requires(trivially_move_constructible<T>) = default;
    constexpr maybe_base(maybe_base&& rhs)
        noexcept(std::is_nothrow_move_constructible_v<T>)
        requires(std::move_constructible<T>)
    {
        if (rhs.has_value())
            move_construct(std::move(*rhs));
    }

    template<typename... Args>
    constexpr explicit maybe_base(std::in_place_t, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : backend_(std::forward<Args>(args)...) { }

    template<typename U, typename... Args>
    constexpr explicit maybe_base(std::in_place_t, std::initializer_list<U> il, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args...>)
        : backend_(il, std::forward<Args>(args)...) { }

    template<typename U = T>
    constexpr explicit(!std::convertible_to<U, T>) maybe_base(U&& value)
        noexcept(std::is_nothrow_constructible_v<T, U&&>)
        requires(std::constructible_from<T, U>
              && !std::same_as<std::remove_cvref_t<U>, std::in_place_t>
              && !std::same_as<std::remove_cvref_t<U>, maybe_base>)
        : backend_(std::forward<U>(value)) { }

    template<typename BackendTypeU, typename U = maybe_base<BackendTypeU>::value_type>
    constexpr explicit(!std::convertible_to<const U&, T>)
    maybe_base(maybe_base<BackendTypeU> const& rhs)
        noexcept(std::is_nothrow_constructible_v<T, const U&>)
        requires (std::constructible_from<T, const U&>
               && !internal_maybe::converts_from<T, maybe_base<BackendTypeU>>)
    {
        if (rhs.has_value())
            copy_construct(*rhs);
    }

    template<typename BackendTypeU, typename U = maybe_base<BackendTypeU>::value_type>
    constexpr explicit(!std::convertible_to<U, T>)
    maybe_base(maybe_base<BackendTypeU>&& rhs)
        noexcept(std::is_nothrow_constructible_v<T, U>)
        requires (std::constructible_from<T, U>
               && !internal_maybe::converts_from<T, maybe_base<BackendTypeU>>)
    {
        if (rhs.has_value())
            move_construct(std::move(*rhs));
    }

    constexpr ~maybe_base() = default;
    constexpr ~maybe_base() requires(!trivially_destructible<T>) {
        if (this->has_value())
            destruct();
    }

    constexpr maybe_base& operator=(maybe_base const& rhs) = delete;
    constexpr maybe_base& operator=(maybe_base const& rhs)
        noexcept(std::is_nothrow_copy_constructible_v<T>
              && std::is_nothrow_copy_assignable_v<T>)
        requires(trivially_copy_constructible<T> &&
                 trivially_copy_assignable<T> &&
                 trivially_destructible<T>) = default;
    constexpr maybe_base& operator=(maybe_base const& rhs)
        noexcept(std::is_nothrow_copy_constructible_v<T>
              && std::is_nothrow_copy_assignable_v<T>)
        requires(std::copy_constructible<T> &&
                 copy_assignable<T>)
    {
        if (this->has_value() && rhs.has_value())
            **this = *rhs;
        else if (this->has_value() && !rhs.has_value())
            this->destruct();
        else if (!this->has_value() && rhs.has_value())
            copy_construct(*rhs);
        return *this;
    }

    constexpr maybe_base& operator=(maybe_base&& rhs)
        noexcept(std::is_nothrow_move_constructible_v<T>
              && std::is_nothrow_move_assignable_v<T>)
        requires(trivially_move_constructible<T> &&
                 trivially_move_assignable<T> &&
                 trivially_destructible<T>) = default;
    constexpr maybe_base& operator=(maybe_base&& rhs)
        noexcept(std::is_nothrow_move_constructible_v<T>
              && std::is_nothrow_move_assignable_v<T>)
        requires(std::move_constructible<T> &&
                 move_assignable<T>)
    {
        if (this->has_value() && rhs.has_value())
            **this = std::move(*rhs);
        else if (this->has_value() && !rhs.has_value())
            this->destruct();
        else if (!this->has_value() && rhs.has_value())
            move_construct(std::move(*rhs));
        return *this;
    }

    template<typename U = T>
    constexpr maybe_base& operator=(U&& value)
        noexcept(std::is_nothrow_constructible_v<T, U&&>
              && std::is_nothrow_assignable_v<T&, U&&>)
        requires(std::constructible_from<T, U>
              && std::assignable_from<T&, U>
              && (!std::is_scalar_v<T> || std::same_as<T, std::decay_t<U>>) 
              && !std::same_as<std::remove_cvref_t<U>, maybe_base>)
    {
        if (this->has_value())
            **this = std::forward<U>(value);
        else
            backend_.construct(std::forward<U>(value));
        return *this;
    }

    template<typename BackendTypeU, typename U = maybe_base<BackendTypeU>::value_type>
    constexpr maybe_base& operator=(maybe_base<BackendTypeU> const& rhs)
        noexcept(std::is_nothrow_constructible_v<T, const U&>
              && std::is_nothrow_assignable_v<T&, const U&>)
        requires(std::constructible_from<T, const U&>
              && std::assignable_from<T&, const U&>
              && !internal_maybe::converts_from<T, maybe_base<BackendTypeU>>
              && !internal_maybe::assigns_from<T, maybe_base<BackendTypeU>>)
    {
        if (this->has_value() && rhs.has_value())
            **this = *rhs;
        else if (this->has_value() && !rhs.has_value())
            this->destruct();
        else if (!this->has_value() && rhs.has_value())
            copy_construct(*rhs);
        return *this;
    }

    template<typename BackendTypeU, typename U = maybe_base<BackendTypeU>::value_type>
    constexpr maybe_base& operator=(maybe_base<BackendTypeU>&& rhs)
        noexcept(std::is_nothrow_constructible_v<T, U>
              && std::is_nothrow_assignable_v<T&, U>)
        requires(std::constructible_from<T, U>
              && std::assignable_from<T&, U>
              && !internal_maybe::converts_from<T, maybe_base<BackendTypeU>>
              && !internal_maybe::assigns_from<T, maybe_base<BackendTypeU>>)
    {
        if (this->has_value() && rhs.has_value())
            **this = std::move(*rhs);
        else if (this->has_value() && !rhs.has_value())
            this->destruct();
        else if (!this->has_value() && rhs.has_value())
            move_construct(std::move(*rhs));
        return *this;
    }

    template<typename... Args>
    constexpr T& emplace(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
        requires(std::constructible_from<T, Args...>)
    {
        this->reset();
        backend_.construct(std::forward<Args>(args)...);
        return **this;
    }

    template<typename U, typename... Args>
    constexpr T& emplace(std::initializer_list<U> il, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args...>)
        requires(std::constructible_from<T, std::initializer_list<U>, Args...>)
    {
        this->reset();
        backend_.construct(il, std::forward<Args>(args)...);
        return **this;
    }

    constexpr void swap(maybe_base& rhs)
        noexcept(std::is_nothrow_move_constructible_v<T>
              && std::is_nothrow_swappable_v<T>)
        requires(std::move_constructible<T> && std::swappable<T>)
    {
        using std::swap;

        if (this->has_value() && rhs.has_value())
            swap(**this, *rhs);
        else if (this->has_value() && !rhs.has_value()) {
            rhs.move_construct(std::move(**this));
            this->destruct();
        } else if (!this->has_value() && rhs.has_value()) {
            this->move_construct(std::move(*rhs));
            rhs.destruct();
        }
    }

    friend constexpr void swap(maybe_base& lhs, maybe_base& rhs)
        noexcept(noexcept(lhs.swap(rhs)))
        requires(std::move_constructible<T> && std::swappable<T>)
    {
        lhs.swap(rhs);
    }

    constexpr const T* operator->() const {
        TSL_HARDENING_ASSERT(this->has_value());
        return std::addressof(backend_.get());
    }

    constexpr T* operator->() {
        TSL_HARDENING_ASSERT(this->has_value());
        return std::addressof(backend_.get());
    }

    constexpr T& operator*() & {
        TSL_HARDENING_ASSERT(this->has_value());
        return backend_.get();
    }

    constexpr T const& operator*() const& {
        TSL_HARDENING_ASSERT(this->has_value());
        return backend_.get();
    }

    constexpr T&& operator*() && {
        TSL_HARDENING_ASSERT(this->has_value());
        return std::move(backend_.get());
    }

    constexpr T const&& operator*() const&& {
        TSL_HARDENING_ASSERT(this->has_value());
        return std::move(backend_.get());
    }

    constexpr explicit operator bool() const noexcept {
        return this->has_value();
    }
    
    constexpr bool has_value() const noexcept {
        return backend_.has_value();
    }

    constexpr T& value() & {
        if (!this->has_value())
            TSL_THROW(bad_maybe_access());

        return backend_.get();
    }

    constexpr T const& value() const& {
        if (!this->has_value())
            TSL_THROW(bad_maybe_access());

        return backend_.get();
    }

    constexpr T&& value() && {
        if (!this->has_value())
            TSL_THROW(bad_maybe_access());

        return std::move(backend_.get());
    }

    constexpr T const&& value() const&& {
        if (!this->has_value())
            TSL_THROW(bad_maybe_access());

        return std::move(backend_.get());
    }

    constexpr T& unchecked_value() & noexcept requires(allow_unchecked_value) {
        return backend_.get();
    }

    constexpr T const& unchecked_value() const& noexcept requires(allow_unchecked_value) {
        return backend_.get();
    }

    constexpr T&& unchecked_value() && noexcept requires(allow_unchecked_value) {
        return std::move(backend_.get());
    }

    constexpr T const&& unchecked_value() const&& noexcept requires(allow_unchecked_value) {
        return std::move(backend_.get());
    }

    template<typename U>
    constexpr T value_or(U&& default_value) const&
        requires(std::convertible_to<U&&, T> && std::copy_constructible<T>)
    {
        if (this->has_value())
            return backend_.get();
        else
            return static_cast<T>(std::forward<U>(default_value));
    }

    template<typename U>
    constexpr T value_or(U&& default_value) &&
        requires(std::convertible_to<U&&, T> && std::move_constructible<T>)
    {
        if (this->has_value())
            return std::move(backend_.get());
        else
            return static_cast<T>(std::forward<U>(default_value));
    }

    constexpr void reset() noexcept {
        if (this->has_value())
            destruct();
    }

private:
    // TODO: Allow custom implementation of copy_construct in the backend
    constexpr void copy_construct(T const& rhs) {
        backend_.construct(rhs);
    }

    // TODO: Allow custom implementation of move_construct in the backend
    constexpr void move_construct(T&& rhs) {
        backend_.construct(std::move(rhs));
    }

    constexpr void destruct() noexcept {
        backend_.destruct();
    }

    BackendType backend_;
};

template<typename BT>
using maybe_base_underlying = typename maybe_base<maybe_backend_default<BT>>::value_type;

template<typename BT, typename BU>
inline constexpr bool operator==(maybe_base<BT> const& lhs, maybe_base<BU> const& rhs)
    TSL_REQUIRES({{ *lhs == *rhs } -> std::convertible_to<bool>;})
{
    if (lhs.has_value() && rhs.has_value())
        return *lhs == *rhs;

    return (lhs.has_value() == rhs.has_value());
}

template<typename BT, typename BU>
inline constexpr bool operator!=(maybe_base<BT> const& lhs, maybe_base<BU> const& rhs)
    TSL_REQUIRES({{ *lhs != *rhs } -> std::convertible_to<bool>;})
{
    if (lhs.has_value() && rhs.has_value())
        return *lhs != *rhs;

    return (lhs.has_value() != rhs.has_value());
}

template<typename BT, typename BU>
inline constexpr bool operator<(maybe_base<BT> const& lhs, maybe_base<BU> const& rhs)
    TSL_REQUIRES({{ *lhs < *rhs } -> std::convertible_to<bool>;})
{
    if (lhs.has_value() && rhs.has_value())
        return *lhs < *rhs;

    return (lhs.has_value() < rhs.has_value());
}

template<typename BT, typename BU>
inline constexpr bool operator>(maybe_base<BT> const& lhs, maybe_base<BU> const& rhs)
    TSL_REQUIRES({{ *lhs > *rhs } -> std::convertible_to<bool>;})
{
    if (lhs.has_value() && rhs.has_value())
        return *lhs > *rhs;

    return (lhs.has_value() > rhs.has_value());
}

template<typename BT, typename BU>
inline constexpr bool operator<=(maybe_base<BT> const& lhs, maybe_base<BU> const& rhs)
    TSL_REQUIRES({{ *lhs <= *rhs } -> std::convertible_to<bool>;})
{
    if (lhs.has_value() && rhs.has_value())
        return *lhs <= *rhs;

    return (lhs.has_value() <= rhs.has_value());
}

template<typename BT, typename BU>
inline constexpr bool operator>=(maybe_base<BT> const& lhs, maybe_base<BU> const& rhs)
    requires(requires{{ *lhs >= *rhs } -> std::convertible_to<bool>;})
{
    if (lhs.has_value() && rhs.has_value())
        return *lhs >= *rhs;

    return (lhs.has_value() >= rhs.has_value());
}

template<typename BT, typename BU>
    requires (std::three_way_comparable_with<maybe_base_underlying<BT>, maybe_base_underlying<BU>>)
inline constexpr std::compare_three_way_result<maybe_base_underlying<BT>, maybe_base_underlying<BU>>
    operator<=>(maybe_base<BT> const& lhs, maybe_base<BU> const& rhs)
{
    if (lhs.has_value() && rhs.has_value())
        return *lhs <=> *rhs;

    return (lhs.has_value() <=> rhs.has_value());
}

// TODO: Implement hash for maybe

template<typename T>
using maybe = maybe_base<maybe_backend_default<T>>;

template<typename T>
inline constexpr maybe<std::decay_t<T>> make_maybe(T&& value)
    TSL_NOEXCEPT(maybe<std::decay_t<T>>(std::forward<T>(value)))
{
    return maybe<std::decay_t<T>>(std::forward<T>(value));
}

template<typename T, typename... Args>
inline constexpr maybe<T> make_maybe(Args&&... args)
    TSL_NOEXCEPT(maybe<T>(std::forward<Args>(args)...))
{
    return maybe<T>(std::forward<Args>(args)...);
}

template<typename T, typename U, typename... Args>
inline constexpr maybe<T> make_maybe(std::initializer_list<U> il, Args&&... args)
    TSL_NOEXCEPT(maybe<T>(il, std::forward<Args>(args)...))
{
    return maybe<T>(il, std::forward<Args>(args)...);
}

}

// Header guard here is not necessary, but suppresses an error from clangd.
// It's probably a bug in clangd.
// Related: https://www.vexforum.com/t/in-included-file-main-file-cannot-be-included-recursively-when-building-a-preamble/105430/3
#ifndef _TSL_INTERNAL_MAYBE_BACKENDS_HPP
#include "tsl/internal/maybe_backends.hpp"
#endif

#endif // _TSL_MAYBE_HPP
