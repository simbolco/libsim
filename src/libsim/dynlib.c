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

// sim_dynlib_load(1): Loads a dynamic library via filename during runtime.
Sim_LibraryHandle sim_dynlib_load(
    const char* filename
) {
    RETURN_IF(!filename, SIM_RC_ERR_NULLPTR, NULL);

#   ifdef _WIN32
        HMODULE library_handle = LoadLibraryA(filename);
        if (!library_handle) {
            _sim_win32_print_last_error(
                "LoadLibraryA(\"%s\")",
                filename
            );
            RETURN(SIM_RC_FAILURE, NULL);
        }

        RETURN(SIM_RC_SUCCESS, library_handle);
#   elif defined(__unix__)
        void* library_handle = dlopen(filename, RTLD_LAZY);
        if (!library_handle) {
            _sim_unix_print_error(
                "dlopen(\"%s\", RTLD_LAZY) failed with error: \"%s\"",
                filename,
                dlerror()
            );
            RETURN(SIM_RC_FAILURE, NULL);
        }

        RETURN(SIM_RC_FAILURE, library_handle);
#   else
#       warning("sim_dynlib_load(2) is unsupported")
        (void)filename; (void)out_library_handle;
        RETURN(SIM_RC_ERR_UNSUPRTD, NULL);
#   endif
}

// sim_dynlib_unload(1): Unloads a dynamic library handle.
void sim_dynlib_unload(
    Sim_LibraryHandle library_handle
) {
    RETURN_IF(!library_handle, SIM_RC_ERR_NULLPTR,);
    
#   ifdef _WIN32
        if (!FreeLibrary((HMODULE)library_handle)) {
            _sim_win32_print_last_error(
                "FreeLibrary(%p)",
                library_handle
            );
            RETURN(SIM_RC_FAILURE,);
        }
        RETURN(SIM_RC_SUCCESS,);
#   elif defined(__unix__)
        if (dlclose((void*)library_handle)) {
            _sim_unix_print_error(
                "dlclose(%p) failed with error: \"%s\"",
                (void*)library_handle,
                dlerror();
            );
            RETURN(SIM_RC_FAILURE,);
        }
        RETURN(SIM_RC_SUCCESS,);
#   else
#       warning("sim_dynlib_unload(1) is unsupported")
        (void)library_handle;
        RETURN(SIM_RC_ERR_UNSUPRTD,);
#   endif
}

// sim_dynlib_find_symbol(2): Retrieves a function from a dynamic library via a symbol.
void* sim_dynlib_find_symbol(
    Sim_LibraryHandle library_handle,
    const char*       symbol_name
) {
    RETURN_IF(!library_handle || !symbol_name, SIM_RC_ERR_NULLPTR, NULL);

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
            RETURN(SIM_RC_FAILURE, NULL);
        }

        RETURN(SIM_RC_SUCCESS, symbol_value);
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
            RETURN(SIM_RC_FAILURE, NULL);
        }

        RETURN(SIM_RC_SUCCESS, symbol_value);
#   else
#       warning("sim_dynlib_find_symbol(2) is unsupported")
        (void)library_handle; (void)symbol_name; (void)out_symbol_value;
        RETURN(SIM_RC_ERR_UNSUPRTD, NULL); 
#   endif
}

#endif /* SIMSOFT_DYNLIB_C_ */
