#ifndef _TSL_LINUX_SUBPROCESS_HPP
#define _TSL_LINUX_SUBPROCESS_HPP

#include <concepts>
#include <ranges>
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <spawn.h>
#include "tsl/cstring_ref.hpp"
#include "tsl/defer.hpp"

namespace tsl {

template <typename R>
concept subprocess_args_range =
    std::ranges::input_range<R> &&
    std::constructible_from<cstring_ref, std::ranges::range_reference_t<R>>;

namespace internal_subprocess {

void file_actions_init(posix_spawn_file_actions_t& file_actions);
void file_actions_destroy(posix_spawn_file_actions_t& file_actions);
void file_actions_add_open(posix_spawn_file_actions_t& file_actions, int fd, const char* path, int flags, mode_t mode);
void file_actions_add_close(posix_spawn_file_actions_t& file_actions, int fd);
void file_actions_add_dup2(posix_spawn_file_actions_t& file_actions, int fd, int new_fd);

}

class subprocess_exception : public std::exception {
public:
    subprocess_exception(const char* message) : message_(message) {}
    const char* what() const noexcept override { return message_; }
private:
    const char* message_;
};

class subprocess {
public:
    // TODO: Implement a better way to handle file_actions instead
    //       of relying on posix_spawn_file_actions_t, also implement
    //       a way to handle attributes.
    class file_actions {
    public:
        file_actions() noexcept : initialized_(false) {}

        bool destroy() {
            if (!initialized_)
                return false;

            internal_subprocess::file_actions_destroy(actions_);
            initialized_ = false;
            return true;
        }

        void add_open(int fd, cstring_ref path, int flags, mode_t mode) {
            ensure_initialized();
            internal_subprocess::file_actions_add_open(actions_, fd, path.get(), flags, mode);
        }

        void add_close(int fd) {
            ensure_initialized();
            internal_subprocess::file_actions_add_close(actions_, fd);
        }

        void add_dup2(int fd, int new_fd) {
            ensure_initialized();
            internal_subprocess::file_actions_add_dup2(actions_, fd, new_fd);
        }

        ~file_actions() {
            destroy();
        }
    private:
        void ensure_initialized() {
            if (!initialized_)
                return;
            internal_subprocess::file_actions_init(actions_);
            initialized_ = true;
        }

        posix_spawn_file_actions_t actions_;
        bool initialized_;
        friend class subprocess;
    };

public: 
    subprocess() noexcept : pid_(-1) {}
    subprocess(subprocess const&) = delete;

    template <subprocess_args_range R = std::initializer_list<cstring_ref>>
    subprocess(cstring_ref exe, R&& args, file_actions const& actions = file_actions()): subprocess() {
        spawn(exe, args, actions);
    }

    template <subprocess_args_range R = std::initializer_list<cstring_ref>>
    void spawn(cstring_ref exe, R&& args, file_actions const& actions = file_actions()) {
        char* _buf[16];

        size_t size = std::ranges::size(args) + 2;
        char** argv = size <= sizeof(_buf)
            ? _buf
            : new char*[size];
        TSL_DEFER {
            if (argv != _buf)
                delete[] argv;
        };

        argv[0] = const_cast<char*>(exe.get());
        size_t i = 1;
        for (auto&& arg : args)
            argv[i++] = const_cast<char*>(cstring_ref(arg).get());
        argv[i] = nullptr;

        spawn_impl(exe, argv, actions);
    }
private:
    pid_t pid_;

    void spawn_impl(cstring_ref exe, char* const args[], file_actions const& file_actions);
};

}

#endif // _TSL_LINUX_SUBPROCESS_HPP
