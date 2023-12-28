#ifndef _TSL_LINUX_SUBPROCESS_HPP
#define _TSL_LINUX_SUBPROCESS_HPP

#include <initializer_list>
#include <string>
#include <sys/types.h>
#include "tsl/cstring_ref.hpp"

namespace tsl {

class subprocess {
public:
    void spawn(cstring_ref program, std::initializer_list<cstring_ref> args);
private:
    pid_t pid_;
};

}

#endif // _TSL_LINUX_SUBPROCESS_HPP
