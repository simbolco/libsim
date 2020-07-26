/**
 * @file random.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for random number generation functions
 * @version 0.2
 * @date 2020-07-04
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_RANDOM_H_
#define SIMSOFT_RANDOM_H_

#include "macro.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API
        
        /**
         * @brief Fill a buffer with a random sequence of bytes.
         * 
         * @param[in]     buffer_size The size of the buffer in bytes.
         * @param[in,out] buffer      Pointer to the buffer.
         * 
         * @throw SIM_ERR_NULLPTR if @e buffer is @c NULL.
         */
        DYNAPI_PROC(void, sim_random_bytes,,
            size_t buffer_size,
            void* buffer
        );
    
    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus // start C++ API
        
        //

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft) // end SimSoft namespace

#endif // SIMSOFT_RANDOM_H_
