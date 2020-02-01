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

#include "simsoft/exception.hpp"
#include "simsoft/debug.h"

using namespace SimSoft;

Exception::Exception(const char* message) noexcept : _message(message) {
    if (get_backtrace_info(_backtrace_array, _BACKTRACE_SIZE, 1) != (size_t)-1)
        _has_backtrace = true;
}
Exception::Exception(const Exception& other) noexcept :
    _message(other._message),
    _has_backtrace(other._has_backtrace)
{
    if (_has_backtrace)
        memcpy(_backtrace_array, other._backtrace_array, sizeof(_backtrace_array));
}
Exception::Exception(Exception&& other) noexcept :
    _message(other._message),
    _has_backtrace(other._has_backtrace)
{
    if (_has_backtrace)
        memcpy(_backtrace_array, other._backtrace_array, sizeof(_backtrace_array));
}
Exception::Exception(ReturnCode rc) noexcept : _message(get_return_code_string(rc)) {
    if (get_backtrace_info(_backtrace_array, _BACKTRACE_SIZE, 1) != (size_t)-1)
        _has_backtrace = true;
}

Exception::~Exception() noexcept {
    if (_has_backtrace) {
        for (size_t i = 0; i < _BACKTRACE_SIZE; i++) {
            free(_backtrace_array[i].function_name);
            free(_backtrace_array[i].file_name);
        }
    }
}

const char* Exception::what() noexcept {
    return _message;
}

#endif /* SIMSOFT_EXCEPTION_CPP_ */
