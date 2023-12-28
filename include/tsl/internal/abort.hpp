#ifndef _TSL_INTERNAL_ABORT_HPP
#define _TSL_INTERNAL_ABORT_HPP

#include <source_location>

namespace tsl {
namespace internal {

void abort_message(const char *message, std::source_location const& location
        = std::source_location::current());

}}

#endif // _TSL_INTERNAL_ABORT_HPP
