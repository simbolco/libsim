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

size_t _sim_siphash(
    const byte*        data_ptr,
    const size_t       data_size,
    const _Sim_HashKey key
) {
#   define ROTL(x, b) (uint64)(((x) << (b)) | ((x) >> (64 - (b))))

#   define U32TO8_LE(p, v)          \
        (p)[0] = (uint8)((v));       \
        (p)[1] = (uint8)((v) >> 8);  \
        (p)[2] = (uint8)((v) >> 16); \
        (p)[3] = (uint8)((v) >> 24);

#   define U64TO8_LE(p, v)                  \
        U32TO8_LE((p), (uint32)((v)));       \
        U32TO8_LE((p)+4, (uint32)((v) >> 32));

#   define U8TO64_LE(p) ( \
        ((uint64)((p)[0])) | \
        ((uint64)((p)[1]) << 8)  | \
        ((uint64)((p)[2]) << 16) | \
        ((uint64)((p)[3]) << 24) | \
        ((uint64)((p)[4]) << 32) | \
        ((uint64)((p)[5]) << 40) | \
        ((uint64)((p)[6]) << 48) | \
        ((uint64)((p)[7]) << 56)   \
    )

#   define SIPROUND {     \
        v0 += v1;          \
        v1 = ROTL(v1, 13); \
        v1 ^= v0;          \
        v0 = ROTL(v0, 32); \
        v2 += v3;          \
        v3 = ROTL(v3, 16); \
        v3 ^= v2;          \
        v0 += v3;          \
        v3 = ROTL(v3, 21); \
        v3 ^= v0;          \
        v2 += v1;          \
        v1 = ROTL(v1, 17); \
        v1 ^= v2;          \
        v2 = ROTL(v2, 32); \
    }

    // constants
    uint64 v0 = 0x736f6d6570736575ULL;
    uint64 v1 = 0x646f72616e646f6dULL;
    uint64 v2 = 0x6c7967656e657261ULL;
    uint64 v3 = 0x7465646279746573ULL;

    const byte* end_ptr = data_ptr + data_size - (data_size % sizeof(uint64));
    const int left = data_size & 7;
    
    uint64 b = ((uint64)data_size) << 56;

    v3 ^= key[1];
    v2 ^= key[0];
    v1 ^= key[1];
    v0 ^= key[0];

    uint64 m;
    for (; data_ptr != end_ptr; data_ptr += 8) {
        m = U8TO64_LE(data_ptr);
        v3 ^= m;

        SIPROUND;
        SIPROUND;

        v0 ^= m;
    }

    switch (left) {
    case 7:
        b |= ((uint64)data_ptr[6]) << 48;
    case 6:
        b |= ((uint64)data_ptr[5]) << 40;
    case 5:
        b |= ((uint64)data_ptr[4]) << 32;
    case 4:
        b |= ((uint64)data_ptr[3]) << 24;
    case 3:
        b |= ((uint64)data_ptr[2]) << 16;
    case 2:
        b |= ((uint64)data_ptr[1]) << 8;
    case 1:
        b |= ((uint64)data_ptr[0]);
        break;
    case 0:
        break;
    }

    v3 ^= b;

    SIPROUND;
    SIPROUND;

    v0 ^= b;
    v2 ^= 0xff;

    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;

    return (size_t)(v0 ^ v1 ^ v2 ^ v3);

#   undef SIPROUND
#   undef ROTL
#   undef U32TO8_LE
#   undef U64TO8_LE
#   undef U8TO64_LE
}


#endif /* SIMSOFT__INTERNAL_C_ */
