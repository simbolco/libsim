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

        typedef struct Sim_BacktraceInfo {
            const void* function_address;
            char* function_name;

            size_t line_number;
            char* file_name;
        } Sim_BacktraceInfo;

        extern SIM_API Sim_ReturnCode C_CALL sim_get_backtrace_info(
            Sim_BacktraceInfo backtrace_array[],
            size_t            backtrace_size,
            size_t            skip_frames,
            size_t *const     out_num_frames
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        typedef C_API::Sim_BacktraceInfo BacktraceInfo;

        extern SIM_API size_t get_backtrace_info(
            BacktraceInfo backtrace_array[],
            size_t        backtrace_size,
            size_t        skip_frames = 0
        );

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_DEBUG_H_ */
