#include "tsl/linux/file.hpp"

#include <unistd.h>
#include "tsl/macros.hpp"

namespace tsl {

ssize_t ifile_handle::read(void* buf, size_t count) {
    return ::read(desc(), buf, count);
}

ssize_t ofile_handle::write(const void* buf, size_t count) {
    return ::write(desc(), buf, count);
}

// bool file::close() noexcept {
//     return (::close(*handle_) == 0);
// }

}


