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
         * @brief Struct containing information pertaining to a call in the stack backtrace.
         */
        typedef struct Sim_BacktraceInfo {
            const void* function_address;
            char* function_name;

            size_t line_number;
            char* file_name;
        } Sim_BacktraceInfo;

        /**
         * @fn Sim_ReturnCode sim_get_backtrace_info(4)
         * @brief Retrieve stack backtrace information.
         * 
         * @param[in,out] backtrace_array Array of Sim_BacktraceInfo structs to populate.
         * @param[in]     backtrace_size  Size of @e backtrace_array.
         * @param[in]     skip_frames     How many frames to skip from the top of the stack.
         * @param[out]    out_num_frames  How much the given array was filled up to; always
         *                                less than or equal to @e backtrace_size-skip_frames.
         *
         * @return @b SIM_RC_OUTOFMEM If there isn't enough memory to allocate symbol strings;
         *         @b SIM_RC_FAILURE  If the stack backtrace couldn't be retrieved;
         *         @b SIM_RC_SUCCSSS  otherwise.
         *
         * @remarks Build libsim in DEBUG mode to get platform-specific error information if you
         *          find elements in @e backtrace_array to not be as populated as you'd expect.
         * 
         *          @b SIM_RC_FAILURE is returned when the Win32 symbol handler fails to initialize
         *          on Windows.
         * 
         *          Strings in @e backtrace_array must be freed.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_get_backtrace_info(
            Sim_BacktraceInfo backtrace_array[],
            size_t            backtrace_size,
            size_t            skip_frames,
            size_t *const     out_num_frames
        );

        /**
         * @fn const char* sim_get_return_code_string(1)
         * @brief Convert a return code to a human-readable string.
         * 
         * @param[in] return_code Return code to convert to a string.
         * 
         * @return A string corresponding to the given return; @c NULL if @e return_code is
         *         @b SIM_RC_SUCCESS or an invalid value.
         */
        extern SIM_API const char* C_CALL sim_get_return_code_string(
            Sim_ReturnCode return_code
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        typedef C_API::Sim_BacktraceInfo BacktraceInfo;

        extern SIM_API size_t get_backtrace_info(
            BacktraceInfo backtrace_array[],
            size_t        backtrace_size,
            size_t        skip_frames = 0
        );

        extern SIM_API const char* get_return_code_string(
            ReturnCode return_code
        );

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_DEBUG_H_ */
