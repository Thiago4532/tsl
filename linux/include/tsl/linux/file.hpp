#ifndef _TSL_LINUX_FILE_HPP
#define _TSL_LINUX_FILE_HPP

#include "tsl/cstring_ref.hpp"

namespace tsl {

// TODO: Please document this :(

class file_handle;
class ifile_handle; class ofile_handle;
class iofile_handle;

class file_handle {
public:
    file_handle() noexcept : fd_(-1) {}
    explicit file_handle(int fd) noexcept : fd_(fd) { }
    iofile_handle promote() const noexcept;

    int desc() const noexcept {
        return fd_;
    }

    int operator*() const noexcept {
        return fd_;
    }

    bool is_valid() const {
        return fd_ != -1;
    }
private:
    int fd_;
};

class ifile_handle : public file_handle {
public:
    ifile_handle() noexcept : file_handle() {}
    explicit ifile_handle(int fd) noexcept : file_handle(fd) {}

    ssize_t read(void* buf, size_t count);

    template<typename T, std::size_t N>
    ssize_t read(T (&buf)[N]) {
        return read(buf, N * sizeof(T));
    }
};

class ofile_handle : public file_handle {
public:
    ofile_handle() noexcept : file_handle() {}
    explicit ofile_handle(int fd) noexcept : file_handle(fd) {}

    ssize_t write(const void* buf, size_t count);
};

class iofile_handle : public file_handle {
public:
    iofile_handle() noexcept : file_handle() {}
    explicit iofile_handle(int fd) noexcept : file_handle(fd) {}

    operator ifile_handle() const noexcept {
        return ifile_handle(desc());
    }

    operator ofile_handle() const noexcept {
        return ofile_handle(desc());
    }
    
    ssize_t read(void* buf, size_t count) {
        return ifile_handle(*this).read(buf, count);
    }

    template<typename T, std::size_t N>
    ssize_t read(T (&buf)[N]) {
        return read(buf, N * sizeof(T));
    }

    ssize_t write(const void* buf, size_t count) {
        return ofile_handle(*this).write(buf, count);
    }
};

inline iofile_handle file_handle::promote() const noexcept {
    return iofile_handle(desc());
}

}

#endif // _TSL_LINUX_FILE_HPP
