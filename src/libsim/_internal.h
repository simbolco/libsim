/**
 * @file _internal.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header reserved for internal use
 * @version 0.1
 * @date 2020-01-12
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT__INTERNAL_H_
#define SIMSOFT__INTERNAL_H_

#include "simsoft/common.h"

// == OS-specific error reporting =================================================================
#ifdef _WIN32
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif

#   include <windows.h>
#   include <strsafe.h>

#   ifdef DEBUG
        extern DWORD _sim_win32_print_last_error(const char* err_format_str, ...);
#   else
#       define _sim_win32_print_last_error(...)
#   endif
#elif defined(unix) || defined(__unix__) || defined(__unix)
#   include <unistd.h>
#   ifndef __unix__ /* Make it easier to separate Unix-like-specific things */
#       define __unix__
#   endif

#   ifdef DEBUG
        extern void _sim_unix_print_error(const char* err_format_str, ...);
#   else
#       define _sim_unix_print_error(...)
#   endif
#endif

// == SIMD extensions =============================================================================

#if defined(_MSC_VER)
#   include <intrin.h> // Microsoft IA-32/AMD64 MMX/SSE extensions
#   define ARCH_X86
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#   include <x86intrin.h> // IA-32/AMD64 MMX/SSE extensions
#   define ARCH_X86
#elif defined(__GNUC__) && defined(__ARM_NEON__)
#   include <arm_neon.h> // ARM NEON
#   define ARCH_ARM_NEON
#elif defined(__GNUC__) && defined(__IWMMXT__)
#   include <mmintrin.h> // ARM WMMX
#   define ARCH_ARM_MMX
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))
#   include <altivec.h> // PowerPC VMX/VSX
#   define ARCH_PPC_VMX
#elif defined(__GNUC__) && defined(__SPE__)
#   include <spe.h> // PowerPC SPE
#   define ARCH_PPC_SPE
#endif

// == Prime number functions ======================================================================

extern bool _sim_is_prime(const size_t num);
extern size_t _sim_next_prime(size_t num);
extern size_t _sim_prev_prime(size_t num);

// == SipHash =====================================================================================

#ifdef __GNUC__
    typedef uint64 _Sim_HashKey __attribute__((vector_size (16)));
#else
    typedef uint64 _Sim_HashKey[2];
#endif

extern size_t _sim_siphash(
    const byte*        data_ptr,
    const size_t       data_size,
    const _Sim_HashKey key
);

// Hash keys for hash fallback + double hashing
#define SIPHASH_KEY1 0x90d6346e7b77f546ULL
#define SIPHASH_KEY2 0x1e0a6097372b5de5ULL
#define SIPHASH_KEY3 0x62d76395429756a9ULL
#define SIPHASH_KEY4 0xe26534637479058cULL

// == Thread-local return code ====================================================================

#ifdef __cplusplus
#   define RC_T SimSoft::C_API::Sim_ReturnCode
#else
#   define RC_T Sim_ReturnCode
#endif
#ifndef SIMSOFT__INTERNAL_C_
    extern THREAD_LOCAL RC_T _SIM_RETURN_CODE;
#else
    THREAD_LOCAL RC_T _SIM_RETURN_CODE;
#endif
#undef RC_T

#define RETURN_IF(cond, ret_code, return_value) \
    if (cond) {                                 \
        _SIM_RETURN_CODE = ret_code;            \
        return return_value;                    \
    }
#define RETURN(ret_code, return_value) { \
    _SIM_RETURN_CODE = ret_code;         \
    return return_value;                 \
}

#endif /* SIMSOFT__INTERNAL_H_ */
