#include "tsl/util/exception_type_name.hpp"

#include <exception>
#include "tsl/defer.hpp"

// TODO: Create a feature test macro to disable this if the user wants to. I am not sure
// if it's compatible to use abi:: functions if the program is using both libc++ and libstdc++.
#if TSL_HAS_INCLUDE(<cxxabi.h>)
#include <cxxabi.h>
#include <cstdlib>
#include <typeinfo>
#define TSL_HAS_CXXABI_H 1
#else
#define TSL_HAS_CXXABI_H 0
#endif

namespace tsl {

maybe<std::string> exception_type_name(std::exception_ptr ex) {
    if (!ex)
        return {};

    try {
        std::rethrow_exception(ex);
    } catch(...) {
        return current_exception_type_name();
    }
}

#if TSL_HAS_CXXABI_H
maybe<std::string> current_exception_type_name() {
    int status;
    std::type_info* ti = abi::__cxa_current_exception_type();
    if (ti == nullptr)
        return {};
    char* name = abi::__cxa_demangle(ti->name(), 0, 0, &status);
    if (name == nullptr)
        return {};
    TSL_DEFER { std::free(name); };
    return std::string(name);
}
#else
maybe<std::string> current_exception_type_name() {
    return {};
}
#endif

}

