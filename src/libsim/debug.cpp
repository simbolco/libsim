/**
 * @file debug.cpp
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief C++ source file/implementation for simsoft/debug.h
 * @version 0.1
 * @date 2020-01-12
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_DEBUG_CPP_
#define SIMSOFT_DEBUG_CPP_

#include "simsoft/debug.h"
#include "simsoft/exception.hpp"
#include "./_internal.h"

using namespace SimSoft;

size_t SimSoft::get_backtrace_info(
    size_t        backtrace_size,
    BacktraceInfo backtrace_array[],
    size_t        skip_frames
) {
    size_t num_frames = 0;

    num_frames = C_API::sim_debug_get_backtrace_info(
        backtrace_size,
        backtrace_array,
        skip_frames
    );

    return num_frames;
}

#endif /* SIMSOFT_DEBUG_CPP_ */
