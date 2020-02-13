/**
 * @file random.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for random number generation functions
 * @version 0.1
 * @date 2020-01-23
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_RANDOM_H_
#define SIMSOFT_RANDOM_H_

#include "./common.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */
        
        /**
         * @fn Sim_ReturnCode sim_random_bytes(2)
         * @brief Fill a buffer with a random sequence of bytes.
         * 
         * @param[in,out] buffer      Pointer to the buffer.
         * @param[in]     buffer_size The size of the buffer in bytes.
         *
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e buffer is @c NULL ;
         *     @b SIM_RC_FAILURE     if the function failed to entirely fill the buffer;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT void C_CALL sim_random_bytes(
            void* buffer,
            size_t buffer_size
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        //

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_RANDOM_H_ */
