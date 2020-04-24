/**
 * @file exception.cpp
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/exception.hpp
 * @version 0.1
 * @date 2020-01-21
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */
#ifndef SIMSOFT_EXCEPTION_CPP_
#define SIMSOFT_EXCEPTION_CPP

#include <cstring>

#include "simsoft/exception.hpp"
#include "simsoft/debug.h"

using namespace SimSoft;

Exception::Exception(const char* message) noexcept :
    _message(message)
{
    if (get_backtrace_info(_BACKTRACE_SIZE, _backtrace_array, 1) != (size_t)-1)
        _has_backtrace = true;
}
Exception::Exception(const Exception& other) noexcept :
    _has_backtrace(other._has_backtrace),
    _message(other._message)
{
    if (_has_backtrace)
        memcpy(_backtrace_array, other._backtrace_array, sizeof(_backtrace_array));
}
Exception::Exception(Exception&& other) noexcept :
    _has_backtrace(other._has_backtrace),
    _message(other._message)
{
    if (_has_backtrace)
        memcpy(_backtrace_array, other._backtrace_array, sizeof(_backtrace_array));
}
Exception::Exception(ReturnCode rc) noexcept : _message(get_return_code_string(rc)) {
    if (get_backtrace_info(_BACKTRACE_SIZE, _backtrace_array, 1) != (size_t)-1)
        _has_backtrace = true;
}

Exception::~Exception() noexcept {
    // free backtrace
    if (_has_backtrace) {
        for (size_t i = 0; i < _BACKTRACE_SIZE; i++) {
            free(_backtrace_array[i].function_name);
            free(_backtrace_array[i].file_name);
        }
    }

    // free message string
    if (_owns_message)
        free((void*)const_cast<char*>(_message));
}

const char* Exception::what() noexcept {
    return _message;
}

#define __ENUMERATE_EXCEPTION(class_name, return_code)                                     \
    class_name ##Exception::class_name ##Exception() noexcept : Exception(return_code) { } \
    class_name ##Exception::class_name ##Exception(const char* message) noexcept :         \
        class_name ##Exception()                                                           \
    {                                                                                      \
        _extended_message = message;                                                       \
    }                                                                                      \
                                                                                           \
    class_name ##Exception::~class_name ##Exception() noexcept {                           \
        if (_cached_message)                                                               \
            free((void*)_cached_message);                                                  \
    }                                                                                      \
                                                                                           \
    const char* class_name ##Exception::what() noexcept {                                  \
        /* Cache return message to not inconvenience user */                               \
        if (!_cached_message) {                                                            \
            _cached_message = (char*)malloc(                                               \
                strlen(_message) + (_extended_message ? strlen(_extended_message) : 0) + 1 \
            );                                                                             \
            if (_cached_message) {                                                         \
                strcat(_cached_message, _message);                                         \
                strcat(_cached_message, _extended_message);                                \
            }                                                                              \
        }                                                                                  \
                                                                                           \
        return _cached_message;                                                            \
    }

    ENUMERATE_EXCEPTIONS
#undef __ENUMERATE_EXCEPTION

#endif /* SIMSOFT_EXCEPTION_CPP_ */
