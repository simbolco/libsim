/**
 * @file _internal.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation of _internal.h
 * @version 0.1
 * @date 2020-01-16
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT__INTERNAL_C_
#define SIMSOFT__INTERNAL_C_

#include "./_internal.h"
#include "simsoft/util.h"

// sim_return_code(0): Gets the return code from SimSoft library functions.
Sim_ReturnCode sim_return_code() {
    return _SIM_RETURN_CODE;
}

// Debug print functions
#ifdef DEBUG
#   ifdef _WIN32
        DWORD _sim_win32_print_last_error(const char* err_format_str, ...) {
            DWORD err = GetLastError();

            CHAR err_msg_buffer[512];
            FormatMessageA(
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

            fprintf(stderr, " returned error %ld: %s\n", err, err_msg_buffer);
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
#endif /* end DEBUG */

bool _sim_is_prime(const size_t num) {
    if (num < 2 || num % 2 == 0)
        return false;
    if (num < 4)
        return true;
    
    size_t threshold = (size_t)floor(sqrt((double)num));
    for (size_t i = 3; i < threshold; i += 2)
        if (num % i == 0)
            return false;
    
    return true;
}

size_t _sim_next_prime(size_t num) {
    if (num % 2 == 0)
        num++;

    while (!_sim_is_prime(num))
        num += 2;

    return num;
}

size_t _sim_prev_prime(size_t num) {
    if (num < 2)
        return 0;
    else if (num < 4)
        return num - 1;
    
    if (num % 2 == 0)
        num--;
    
    while (!_sim_is_prime(num))
        num -= 2;
    
    return num;
}

#endif /* SIMSOFT__INTERNAL_C_ */
