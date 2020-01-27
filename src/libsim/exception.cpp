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

Exception::Exception(const char* message) noexcept : _message(message) {}
Exception::Exception(const Exception& other) noexcept : Exception(other._message) {}
Exception::Exception(Exception&& other) noexcept : Exception(other._message) {}

Exception::Exception(ReturnCode rc) noexcept {
    switch (rc){
    case ReturnCode::ERR_NULLPTR:
        _message = "Null pointer error";
        break;
    case ReturnCode::ERR_OUTOFMEM:
        _message = "Out of memory";
        break;
    case ReturnCode::ERR_ZERODIV:
        _message = "Zero division error";
        break;
    case ReturnCode::ERR_INVALARG:
        _message = "Invalid argument";
        break;
    case ReturnCode::ERR_OUTOFBND:
        _message = "Out of bounds error";
        break;
    case ReturnCode::ERR_UNSUPRTD:
        _message = "Unsupported operation";
        break;
    case ReturnCode::ERR_NOTFOUND:
        _message = "Item not found in container";
        break;

    case ReturnCode::FAILURE:
    case ReturnCode::SUCCESS:
        break;
    }

    if (
        rc &&
        get_backtrace_info(_backtrace_array, _BACKTRACE_SIZE, 1) != (size_t)-1
    )
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

Exception& Exception::operator=(const Exception& other) noexcept {
    _message = other._message;
    memcpy(_backtrace_array, other._backtrace_array, sizeof(_backtrace_array));
    _has_backtrace = other._has_backtrace;
    return *this;
}

const char* Exception::what() noexcept {
    return _message;
}

#endif /* SIMSOFT_EXCEPTION_CPP_ */
