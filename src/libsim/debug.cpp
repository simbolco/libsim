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
    BacktraceInfo backtrace_array[],
    size_t        backtrace_size,
    size_t        skip_frames
) {
    size_t num_frames = 0;

    num_frames = C_API::sim_debug_get_backtrace_info(
        backtrace_array,
        backtrace_size,
        skip_frames
    );

    if (_SIM_RETURN_CODE)
        throw Exception((ReturnCode)_SIM_RETURN_CODE);

    return num_frames;
}

const char* SimSoft::get_return_code_string(
    ReturnCode return_code
) {
    return C_API::sim_debug_get_return_code_string(
        (C_API::Sim_ReturnCode)return_code
    );
}

#endif /* SIMSOFT_DEBUG_CPP_ */
