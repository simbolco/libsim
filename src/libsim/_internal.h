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

#define _OBJECT_FREE_FUNCTION(TYPE, obj_family)         \
Sim_ReturnCode obj_family ##_free (                     \
    TYPE* obj_ptr                                       \
) {                                                     \
    Sim_ReturnCode rc = obj_family ##_destroy(obj_ptr); \
    if (rc == SIM_RC_SUCCESS)                           \
        free(obj_ptr);                                  \
                                                        \
    return rc;                                          \
}

extern bool _sim_is_prime(const size_t num);
extern size_t _sim_next_prime(size_t num);
extern size_t _sim_prev_prime(size_t num);
extern size_t _sim_siphash(
    const byte*  data_ptr,
    const size_t data_size,
    const uint64 k0,
    const uint64 k1
);

// Hash keys for hash fallback + double hashing
#define SIPHASH_KEY1 0x90d6346e7b77f546ULL
#define SIPHASH_KEY2 0x1e0a6097372b5de5ULL
#define SIPHASH_KEY3 0x62d76395429756a9ULL
#define SIPHASH_KEY4 0xe26534637479058cULL

#endif /* SIMSOFT__INTERNAL_H_ */
