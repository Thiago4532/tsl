#include "tsl/linux/subprocess.hpp"
#include <system_error>
#include <unistd.h>

namespace tsl {

namespace internal_subprocess {

void file_actions_init(posix_spawn_file_actions_t& file_actions) {
    if (posix_spawn_file_actions_init(&file_actions) != 0)
        throw std::system_error(errno, std::system_category(), "posix_spawn_file_actions_init");
}

void file_actions_destroy(posix_spawn_file_actions_t& file_actions) {
    if (posix_spawn_file_actions_destroy(&file_actions) != 0)
        throw std::system_error(errno, std::system_category(), "posix_spawn_file_actions_destroy");
}

void file_actions_add_open(posix_spawn_file_actions_t& file_actions, int fd, const char* path, int flags, mode_t mode) {
    if (posix_spawn_file_actions_addopen(&file_actions, fd, path, flags, mode) != 0)
        throw std::system_error(errno, std::system_category(), "posix_spawn_file_actions_addopen");
}

void file_actions_add_close(posix_spawn_file_actions_t& file_actions, int fd) {
    if (posix_spawn_file_actions_addclose(&file_actions, fd) != 0)
        throw std::system_error(errno, std::system_category(), "posix_spawn_file_actions_addclose");
}

void file_actions_add_dup2(posix_spawn_file_actions_t& file_actions, int fd, int new_fd) {
    if (posix_spawn_file_actions_adddup2(&file_actions, fd, new_fd) != 0)
        throw std::system_error(errno, std::system_category(), "posix_spawn_file_actions_adddup2");
}

} // namespace internal_subprocess

void subprocess::spawn_impl(cstring_ref exe, char* const args[], file_actions const& actions) {
    pid_t pid;
    posix_spawn_file_actions_t* actionsp = actions.initialized_
        ? const_cast<posix_spawn_file_actions_t*>(&actions.actions_)
        : nullptr;

    if (posix_spawnp(&pid, exe.get(), actionsp, nullptr, args, environ) != 0) 
        throw std::system_error(errno, std::system_category(), "posix_spawnp");

    pid_ = pid;
}

} // namespace tsl
