/**
 * @file util.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for utility functions
 * @version 0.2
 * @date 2020-07-06
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_UTIL_H_
#define SIMSOFT_UTIL_H_

#include "macro.h"
#include "typedefs.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API

        /**
         * @brief Hash key type for sim_siphash.
         */
#       ifdef __GNUC__
            typedef __attribute__((vector_size (16))) uint64 Sim_HashKey;
#       else
            typedef uint64 Sim_HashKey[2];
#       endif

        /**
         * @brief SipHash-2-4 implementation.
         * 
         * @param[in] data_ptr  Pointer to data to create a hash for.
         * @param[in] data_size The size of the data pointed to by @e data_ptr.
         * @param[in] key       Key used in generating the hash.
         * 
         * @returns A hash key; 0 if @e data_ptr is @c NULL .
         */
        DYNAPI_PROC(Sim_HashType, sim_util_siphash,,
            const uint8*      data_ptr,
            const size_t      data_size,
            const Sim_HashKey key
        );

    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus // start C++ API

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft) // end SimSoft namespace

#endif // SIMSOFT_UTIL_H_
