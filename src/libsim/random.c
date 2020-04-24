/**
 * @file random.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/random.h
 * @version 0.1
 * @date 2020-01-23
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_RANDOM_C_
#define SIMSOFT_RANDOM_C_

#include "simsoft/random.h"
#include "./_internal.h"

#ifdef _WIN32
#   include "simsoft/dynlib.h"

    static Sim_LibraryHandle _SIM_WIN32_ADVAPI32_LIBHANDLE = NULL;
    static BOOLEAN (*RtlGenRandom)(
        PVOID random_buffer,
        ULONG random_buffer_length
    ) = NULL;

    // 
    static void _sim_win32_onexit_clean_advapi32(void) {
        sim_dynlib_unload(_SIM_WIN32_ADVAPI32_LIBHANDLE);
    }

#endif

// sim_random_bytes(2): Fill a buffer with a random sequence of bytes.
void sim_random_bytes(
    size_t buffer_size,
    void* buffer_ptr
) {
    if (!buffer_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

#   ifdef _WIN32
        // get library handle to Advapi32.dll
        if (!_SIM_WIN32_ADVAPI32_LIBHANDLE) {
            if (!(_SIM_WIN32_ADVAPI32_LIBHANDLE = sim_dynlib_load("Advapi32.dll")))
                THROW(SIM_RC_ERR_UNSUPRTD);
            
            // clean up library handle on exit
            atexit(_sim_win32_onexit_clean_advapi32);
        }
        
        // get function pointer if available
        if (
            !RtlGenRandom &&
            !(RtlGenRandom = sim_dynlib_find_symbol(
                _SIM_WIN32_ADVAPI32_LIBHANDLE,
                "SystemFunction036"
            ))
        )
            THROW(SIM_RC_ERR_UNSUPRTD);
        
        // grab random bytes
        if (!RtlGenRandom(buffer_ptr, (ULONG)buffer_size))
            RETURN(SIM_RC_FAILURE,);
        
        RETURN(SIM_RC_SUCCESS,);
#   elif defined(unix) || defined(__unix__) || defined(__unix)
        // open dev/random
        FILE* urandom_handle = fopen("/dev/urandom", "r");
        if (!urandom_handle)
            THROW(SIM_RC_ERR_UNSUPRTD);
        
        // read random bytes from dev/random
        if (fread(buffer_ptr, 1, buffer_size, urandom_handle) < buffer_size) {
            // TODO: check ferror()
            fclose(urandom_handle);
            RETURN(SIM_RC_FAILURE,);;
        }

        fclose(urandom_handle);
        RETURN(SIM_RC_SUCCESS,);
#   else
#       warning("sim_random_bytes(2) is unsupported")
        (void)buffer_ptr; (void)buffer_size;
        THROW(SIM_RC_ERR_UNSUPRTD);
#   endif
}



#endif /* SIMSOFT_RANDOM_C_ */
