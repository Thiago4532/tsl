#ifndef _TSL_TYPE_TRAITS_HPP
#define _TSL_TYPE_TRAITS_HPP

namespace tsl {

template<typename T, typename... U>
struct type_list {
    using head = T;
    using tail = type_list<U...>;
};

template<typename T>
struct type_list<T> {
    using head = T;
};

}

#endif // _TSL_TYPE_TRAITS_HPP
