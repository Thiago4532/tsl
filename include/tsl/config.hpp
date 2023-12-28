#ifndef _TSL_CONFIG_HPP
#define _TSL_CONFIG_HPP

#if !__cpp_exceptions
#define TSL_HAS_EXCEPTIONS 0
#elif !defined(TSL_HAS_EXCEPTIONS)
#define TSL_HAS_EXCEPTIONS 1
#endif

#endif // _TSL_CONFIG_HPP
