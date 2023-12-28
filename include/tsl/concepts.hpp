#ifndef _TSL_CONCEPTS_HPP
#define _TSL_CONCEPTS_HPP

#include <concepts>
#include <type_traits>
#include "tsl/type_traits.hpp"

namespace tsl {

// Implementing concept wrappers for certain STL type traits.

template<typename T>
concept trivially_destructible =
    std::destructible<T>
    && std::is_trivially_destructible_v<T>;

template<typename T>
concept trivially_copy_constructible =
    std::copy_constructible<T>
    && std::is_trivially_copy_constructible_v<T>;

template<typename T>
concept trivially_move_constructible =
    std::move_constructible<T>
    && std::is_trivially_move_constructible_v<T>;

template<typename T>
concept copy_assignable =
    std::assignable_from<std::add_lvalue_reference_t<T>,
                         std::add_lvalue_reference_t<const T>>;

template<typename T>
concept trivially_copy_assignable =
    copy_assignable<T> && std::is_trivially_copy_assignable_v<T>;

template<typename T>
concept move_assignable =
    std::assignable_from<std::add_lvalue_reference_t<T>,
                         std::add_rvalue_reference_t<T>>;

template<typename T>
concept trivially_move_assignable =
    move_assignable<T> && std::is_trivially_move_assignable_v<T>;

// End of concept wrappers.

}

#endif // _TSL_CONCEPTS_HPP
