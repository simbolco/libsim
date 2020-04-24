/**
 * @file debug.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for debugging functions
 * @version 0.1
 * @date 2020-01-12
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_DEBUG_H_
#define SIMSOFT_DEBUG_H_

#include "./common.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */
        
        /**
         * @struct Sim_BacktraceInfo
         * @headerfile debug.h "simsoft/debug.h"
         * @brief Struct containing information pertaining to a call in the stack backtrace.
         * 
         * @var Sim_BacktraceInfo::function_address
         *     The address of a function found on the stack.
         * @var Sim_BacktraceInfo::function_name
         *     The symbol name of the function found on the stack.
         * @var Sim_BacktraceInfo::line_number
         *     The line number the function was defined in.
         * @var Sim_BacktraceInfo::file_name
         *     The name of the file the function was defined in.
         */
        typedef struct Sim_BacktraceInfo {
            const void* function_address;
            char* function_name;

            size_t line_number;
            char* file_name;
        } Sim_BacktraceInfo;

        /**
         * @fn size_t sim_debug_get_backtrace_info(size_t, Sim_BacktraceInfo[], size_t)
         * @headerfile debug.h "simsoft/debug.h"
         * @brief Retrieve stack backtrace information.
         * 
         * @param[in]     backtrace_size  Size of @e backtrace_array.
         * @param[in,out] backtrace_array Array of Sim_BacktraceInfo structs to populate.
         * @param[in]     skip_frames     How many frames to skip from the top of the stack.
         * 
         * @return The number of frames in the array that were filled.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_OUTOFMEM If there isn't enough memory to allocate symbol strings;
         *     @b SIM_RC_FAILURE  If the stack backtrace couldn't be retrieved;
         *     @b SIM_RC_SUCCESS  otherwise.
         *
         * @remarks Build libsim in DEBUG mode to get platform-specific error information if you
         *          find elements in @e backtrace_array to not be as populated as you'd expect.
         * 
         * @remarks On Windows, @b SIM_RC_FAILURE is returned when the Win32 symbol handler fails
         *          to initialize.
         * 
         * @remarks Strings in @e backtrace_array must be freed.
         */
        extern EXPORT size_t C_CALL sim_debug_get_backtrace_info(
            size_t            backtrace_size,
            Sim_BacktraceInfo backtrace_array[],
            size_t            skip_frames
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        struct BacktraceInfo : public C_API::Sim_BacktraceInfo { };

        extern EXPORT size_t get_backtrace_info(
            size_t        backtrace_size,
            BacktraceInfo backtrace_array[],
            size_t        skip_frames = 0
        );

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_DEBUG_H_ */
