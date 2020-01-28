/**
 * @file dynlib.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/dynlib.h
 * @version 0.1
 * @date 2020-01-23
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_DYNLIB_C_
#define SIMSOFT_DYNLIB_C_

#include "simsoft/dynlib.h"
#include "simsoft/hashmap.h"
#include "./_internal.h"

#ifdef __unix__
#   include <dlfcn.h>
#endif

// sim_load_library(2): Loads a dynamic library via filename during runtime.
Sim_ReturnCode sim_load_library(
    const char*              filename,
    Sim_LibraryHandle *const out_library_handle
) {
    if (!filename || !out_library_handle)
        return SIM_RC_ERR_NULLPTR;

#   ifdef _WIN32
        HMODULE library_handle = LoadLibraryA(filename);
        if (!library_handle) {
            _sim_win32_print_last_error(
                "LoadLibraryA(\"%s\")",
                filename
            );
            return SIM_RC_FAILURE;
        }

        *out_library_handle = (Sim_LibraryHandle)library_handle;
        return SIM_RC_SUCCESS;
#   elif defined(__unix__)
        void* library_handle = dlopen(filename, RTLD_LAZY);
        if (!library_handle) {
            _sim_unix_print_error(
                "dlopen(\"%s\", RTLD_LAZY) failed with error: \"%s\"",
                filename,
                dlerror()
            );
            return SIM_RC_FAILURE;
        }

        *out_library_handle = library_handle;
        return SIM_RC_SUCCESS;
#   else
#       warning("sim_load_library(2) is unsupported")
        (void)filename; (void)out_library_handle;
        return SIM_RC_ERR_UNSUPRTD;
#   endif
}

// sim_unload_library(1): Unloads a dynamic library handle.
Sim_ReturnCode sim_unload_library(
    Sim_LibraryHandle library_handle
) {
    if (!library_handle)
        return SIM_RC_ERR_NULLPTR;
    
#   ifdef _WIN32
        if (!FreeLibrary((HMODULE)library_handle)) {
            _sim_win32_print_last_error(
                "FreeLibrary(%p)",
                library_handle
            );
            return SIM_RC_FAILURE;
        }
        return SIM_RC_SUCCESS;
#   elif defined(__unix__)
        if (dlclose((void*)library_handle)) {
            _sim_unix_print_error(
                "dlclose(%p) failed with error: \"%s\"",
                (void*)library_handle,
                dlerror();
            );
            return SIM_RC_FAILURE;
        }
        return SIM_RC_SUCCESS;
#   else
#       warning("sim_unload_library(1) is unsupported")
        (void)library_handle;
        return SIM_RC_ERR_UNSUPRTD;    
#   endif
}

// sim_find_symbol(3): Retrieves a function from a dynamic library via a symbol.
Sim_ReturnCode sim_find_symbol(
    Sim_LibraryHandle library_handle,
    const char*       symbol_name,
    void* *const      out_symbol_value
) {
    if (!library_handle || !symbol_name || !out_symbol_value)
        return SIM_RC_ERR_NULLPTR;

#   ifdef _WIN32
        void* symbol_value = (void*)GetProcAddress(
            (HMODULE)library_handle,
            symbol_name
        );
        if (!symbol_value) {
            _sim_win32_print_last_error(
                "GetProcAddress(%p, \"%s\")",
                library_handle,
                symbol_name
            );
            return SIM_RC_FAILURE;
        }

        *out_symbol_value = symbol_value;
        return SIM_RC_SUCCESS;
#   elif defined(__unix__)
        void* symbol_value = dlsym(library_handle, symbol_name);
        char* error_string;

        if ((error_string = dlerror()) != NULL) {
            _sim_unix_print_error(
                "dlsym(%p, \"%s\") failed with error \"%s\"",
                library_handle,
                symbol_name,
                error_string
            );
            return SIM_RC_FAILURE;
        }

        *out_symbol_value = symbol_value;
        return SIM_RC_SUCCESS;
#   else
#       warning("sim_find_symbol(3) is unsupported")
        (void)library_handle; (void)symbol_name; (void)out_symbol_value;
        return SIM_RC_ERR_UNSUPRTD;    
#   endif
}

#endif /* SIMSOFT_DYNLIB_C_ */
