/**
 * @file _internal.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation of _internal.h
 * @version 0.2
 * @date 2020-07-26
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT__INTERNAL_C_
#define SIMSOFT__INTERNAL_C_

#include "./_internal.h"

// Debug print functions
#ifdef DEBUG
#   include <stdarg.h>

#   ifdef _WIN32
        DWORD _sim_win32_print_last_error(const char* err_format_str, ...) {
            DWORD err = GetLastError();

            WCHAR err_msg_buffer[512];
            FormatMessageW(
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                err_msg_buffer,
                512,
                NULL
            );

            {
                va_list args;
                va_start(args, err_format_str);
                vfprintf(stderr, err_format_str, args);
                va_end(args);
            }

            fprintf(stderr, " returned error %ld: %S\n", err, err_msg_buffer);
            return err;
        }
#   elif defined(__unix__)
        void _sim_unix_print_error(const char* err_format_str, ...) {
            {
                va_list args;
                va_start(args, err_format_str);
                vfprintf(stderr, err_format_str, args);
                va_end(args);
            }
            fprintf(stderr, "\n");
        }
#   endif
#endif // end DEBUG

#if defined(_MSC_VER) && _MSC_VER < 1900
    int _sim_vsnprintf(char* buffer, size_t count, const char* format, va_list args) {
        va_list args_copy;
        int count = -1;
        
        if (size != 0) {
            args_copy = args;
            count = _vsnprintf_s(buffer, size, _TRUNCATE, format, args_copy);
            va_end(args_copy);
        }
        if (count == -1) {
            args_copy = args;
            count = _vscprintf(format, args_copy);
            va_end(args_copy);
        }
        return count;
    }
#endif // end _MSC_VER < 1900

#endif // SIMSOFT__INTERNAL_C_
