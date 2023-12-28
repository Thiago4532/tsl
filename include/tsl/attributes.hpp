#ifndef _TSL_ATTRIBUTES_HPP
#define _TSL_ATTRIBUTES_HPP

#ifdef __has_attribute
#define TSL_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define TSL_HAS_ATTRIBUTE(x) 0
#endif

#ifdef __has_cpp_attribute
#define TSL_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define TSL_HAS_CPP_ATTRIBUTE(x) 0
#endif

#if TSL_HAS_CPP_ATTRIBUTE(clang::lifetimebound)
#define TSL_ATTR_LIFETIMEBOUND [[clang::lifetimebound]]
#elif TSL_HAS_ATTRIBUTE(lifetimebound)
#define TSL_ATTR_LIFETIMEBOUND __attribute__((lifetimebound))
#else
#define TSL_ATTR_LIFETIMEBOUND
#endif

#endif // _TSL_ATTRIBUTES_HPP
