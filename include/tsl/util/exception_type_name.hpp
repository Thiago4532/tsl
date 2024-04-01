#ifndef _TSL_UTIL_EXCEPTION_TYPE_NAME_HPP
#define _TSL_UTIL_EXCEPTION_TYPE_NAME_HPP

#include <exception>
#include <string>
#include "tsl/maybe.hpp"

namespace tsl {

// exception_type_name
// Returns the type name of the exception, if the compiler supports it.
// If the compiler does not support it, returns a empty maybe.
maybe<std::string> exception_type_name(std::exception_ptr ex);
maybe<std::string> current_exception_type_name();

}

#endif // _TSL_UTIL_EXCEPTION_TYPE_NAME_HPP
