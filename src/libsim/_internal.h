/**
 * @file _internal.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header reserved for internal use
 * @version 0.2
 * @date 2020-07-26
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT__INTERNAL_H_
#define SIMSOFT__INTERNAL_H_

#include "simsoft/macro.h"
#include "simsoft/typedefs.h"

// == OS-specific error reporting ==================================================================

#ifdef _WIN32
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif

#   include <windows.h>
#   include <strsafe.h>

#   ifdef DEBUG
#       include <stdarg.h>
#       include <stdio.h>

        extern LOCAL DWORD _sim_win32_print_last_error(const char* err_format_str, ...);
#   else
#       define _sim_win32_print_last_error(...)
#   endif
#elif defined(unix) || defined(__unix__) || defined(__unix)
#   include <unistd.h>
#   ifndef __unix__ /* Make it easier to separate Unix-like-specific things */
#       define __unix__
#   endif

#   ifdef DEBUG
#       include <stdarg.h>
#       include <stdio.h>

        extern LOCAL void _sim_unix_print_error(const char* err_format_str, ...);
#   else
#       define _sim_unix_print_error(...)
#   endif
#endif

// == C99 vsnprintf for old MSVC versions ==========================================================

#if defined(_MSC_VER) && _MSC_VER < 1900
#   include <stdarg.h>

    extern LOCAL int _sim_vsnprintf(char* buffer, size_t count, const char* format, va_list args);
#   define vsnprintf _sim_vsnprintf
#endif

// == SIMD extensions ==============================================================================

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

#endif // SIMSOFT__INTERNAL_H_
