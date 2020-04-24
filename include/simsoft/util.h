/**
 * @file util.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for utility functions
 * @version 0.1
 * @date 2020-01-23
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_UTIL_H_
#define SIMSOFT_UTIL_H_

#include "./common.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

        /**
         * @typedef Sim_HashKey
         * @headerfile util.h "simsoft/util.h"
         * @brief Hash key type for sim_siphash.
         */
#       ifdef __GNUC__
            typedef uint64 Sim_HashKey __attribute__((vector_size (16)));
#       else
            typedef uint64 Sim_HashKey[2];
#       endif

        /**
         * @fn Sim_HashType sim_siphash(const uint8*, const size_t, const Sim_HashKey)
         * @headerfile util.h "simsoft/util.h"
         * @brief SipHash-2-4 implementation hash function implementation.
         * 
         * @param[in] data_ptr  Pointer to data to create a hash for.
         * @param[in] data_size The size of the data pointed to by @e data_ptr.
         * @param[in] key       Key used in generating the hash.
         * 
         * @return A hash key; 0 if @e data_ptr is @c NULL .
         */
        extern EXPORT Sim_HashType C_CALL sim_siphash(
            const uint8*       data_ptr,
            const size_t       data_size,
            const Sim_HashKey key
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        //

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_UTIL_H_ */
