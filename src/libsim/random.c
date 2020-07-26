/**
 * @file random.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/random.h
 * @version 0.2
 * @date 2020-07-04
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_RANDOM_C_
#define SIMSOFT_RANDOM_C_

#include "simsoft/random.h"

#include "./_internal.h"
#include "simsoft/except.h"

#ifdef _WIN32
#   include "simsoft/dynlib.h"

    static Sim_LibraryHandle _SIM_WIN32_ADVAPI32_LIBHANDLE = NULL;
    static BOOLEAN (*RtlGenRandom)(
        PVOID random_buffer,
        ULONG random_buffer_length
    ) = NULL;

    // 
    static void _win32_onexit_clean_advapi32(void) {
        sim_dynlib_unload(_SIM_WIN32_ADVAPI32_LIBHANDLE);
    }
#elif defined(__unix__)
#   include <errno.h>
#   include <stdio.h>
#   include <string.h>
#endif

// sim_random_bytes(2): Fill a buffer with a random sequence of bytes.
void sim_random_bytes(
    size_t buffer_size,
    void* buffer_ptr
) {
    if (!buffer_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);

#   ifdef _WIN32
        // get library handle to Advapi32.dll
        if (!_SIM_WIN32_ADVAPI32_LIBHANDLE) {
            if (!(_SIM_WIN32_ADVAPI32_LIBHANDLE = sim_dynlib_load("Advapi32.dll")))
                THROW(SIM_ERR_UNSUPRTD, "Couldn't load Advapi32.dll");
            
            // clean up library handle on exit
            atexit(_win32_onexit_clean_advapi32);
        }
        
        // get function pointer if available
        if (
            !RtlGenRandom &&
            !(RtlGenRandom = sim_dynlib_find_symbol(
                _SIM_WIN32_ADVAPI32_LIBHANDLE,
                "SystemFunction036"
            ))
        )
            THROW(SIM_ERR_UNSUPRTD, "Couldn't load RtlGenRandom (aka 'SystemFunction036')");
        
        // grab random bytes
        (*RtlGenRandom)(buffer_ptr, (ULONG)buffer_size);
#   elif defined(unix) || defined(__unix__) || defined(__unix)
        // open dev/random
        FILE* urandom_handle = fopen("/dev/urandom", "r");
        if (!urandom_handle)
            THROW(SIM_ERR_UNSUPRTD, "Couldn't open /dev/urandom: %s", strerror(errno));
        
        // read random bytes from dev/random
        fread(buffer_ptr, 1, buffer_size, urandom_handle) < buffer_size;
        fclose(urandom_handle);
        return;
#   else
#       error("sim_random_bytes(2) is unsupported")
#   endif
}

#endif // SIMSOFT_RANDOM_C_
