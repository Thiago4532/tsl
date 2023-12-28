#include "tsl/internal/abort.hpp"

#include <cstdio>

namespace tsl {
namespace internal {

void abort_message(const char *message, std::source_location const& location) {
    std::fprintf(stderr,
            "%s:%d: %s: %s\n",
            location.file_name(),
            static_cast<int>(location.line()),
            location.function_name(),
            message);
    std::fflush(stderr);
}

// void assert_fail(const char *message, std::source_location location) {
//     std::fprintf(stderr,
//             "%s:%d: %s: Assertion '%s' failed.\n",
//             location.file_name(),
//             static_cast<int>(location.line()),
//             location.function_name(),
//             message);
//     std::fflush(stderr);
// }

}}
