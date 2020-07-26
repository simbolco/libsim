/**
 * @file debug.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for debugging functions
 * @version 0.2
 * @date 2020-07-04
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_DEBUG_H_
#define SIMSOFT_DEBUG_H_

#include <stddef.h>

#include "macro.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API
        
        /**
         * @brief Struct containing information pertaining to a call in the stack backtrace.
         */
        typedef struct Sim_BacktraceInfo {
            const void* function_address; ///< The address of a function found on the stack.
            char* function_name;          ///< The symbol name of the function sound on the stack.

            size_t line_number;           ///< The line number the function was defined.
            char* file_name;              ///< The name of the file the function was defined in.
        } Sim_BacktraceInfo;

        /**
         * @brief Retrieve stack backtrace information.
         * 
         * @param[in]     backtrace_size  Size of @e backtrace_array.
         * @param[in,out] backtrace_array Array of Sim_BacktraceInfo structs to populate.
         * @param[in]     skip_frames     How many frames to skip from the top of the stack.
         * 
         * @returns The number of frames in the array that were filled; 0 on failure.
         * 
         * @throw SIM_ERR_OUTOFMEM if there isn't enough memory to allocate symbol strings.
         *
         * @remarks Build libsim in DEBUG mode to get platform-specific error information if you
         *          find elements in @e backtrace_array to not be as populated as you'd expect.
         * 
         * @remarks Strings in @e backtrace_array must be freed after use.
         */
        DYNAPI_PROC(size_t, sim_debug_get_backtrace_info,,
            size_t            backtrace_size,
            Sim_BacktraceInfo backtrace_array[],
            size_t            skip_frames
        );
    
    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus // start C++ API
        
        struct BacktraceInfo : public C_API::Sim_BacktraceInfo { };

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft) // end SimSoft namespace

#endif // SIMSOFT_DEBUG_H_
