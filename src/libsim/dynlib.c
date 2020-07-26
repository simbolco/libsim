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

#include <stddef.h>

#include "./_internal.h"
#if defined(__unix__)
#   include <dlfcn.h>
#endif

#include "simsoft/except.h"

// sim_dynlib_load(1): Loads a dynamic library via filename during runtime.
Sim_LibraryHandle sim_dynlib_load(
    const char* filename
) {
    if (!filename)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
        
#   ifdef _WIN32
        HMODULE library_handle = LoadLibraryA(filename);
        if (!library_handle) {
            _sim_win32_print_last_error(
                "LoadLibraryA(\"%s\")",
                filename
            );
            return NULL;
        }

        return (Sim_LibraryHandle)library_handle;
#   elif defined(__unix__)
        void* library_handle = dlopen(filename, RTLD_LAZY);
        if (!library_handle) {
            _sim_unix_print_error(
                "dlopen(\"%s\", RTLD_LAZY) failed with error: \"%s\"",
                filename,
                dlerror()
            );
            return NULL;
        }

        return (Sim_LibraryHandle)library_handle;
#   else
#       error("sim_dynlib_load(2) is unsupported")
#   endif
}

// sim_dynlib_unload(1): Unloads a dynamic library handle.
void sim_dynlib_unload(
    Sim_LibraryHandle library_handle
) {
    if (!library_handle)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    
#   ifdef _WIN32
        if (!FreeLibrary((HMODULE)library_handle)) {
            _sim_win32_print_last_error(
                "FreeLibrary(%p)",
                library_handle
            );
        }
#   elif defined(__unix__)
        if (dlclose((void*)library_handle)) {
            _sim_unix_print_error(
                "dlclose(%p) failed with error: \"%s\"",
                (void*)library_handle,
                dlerror();
            );
        }
#   else
#       error("sim_dynlib_unload(1) is unsupported")
#   endif
}

// sim_dynlib_find_symbol(2): Retrieves a function from a dynamic library via a symbol.
void* sim_dynlib_find_symbol(
    Sim_LibraryHandle library_handle,
    const char*       symbol_name
) {
    if (!library_handle)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    if (!symbol_name)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);
    
#   ifdef _WIN32
        void* symbol_value = (void*)GetProcAddress(
            (HMODULE)library_handle,
            symbol_name
        );
        if ((uintptr_t)symbol_value <= (uintptr_t)2) {
            _sim_win32_print_last_error(
                "GetProcAddress(%p, \"%s\")",
                library_handle,
                symbol_name
            );
            return NULL;
        }

        return symbol_value;
#   elif defined(__unix__)
        void* symbol_value = dlsym(library_handle, symbol_name);
        char* error_string = dlerror();

        if (error_string != NULL) {
            _sim_unix_print_error(
                "dlsym(%p, \"%s\") failed with error \"%s\"",
                library_handle,
                symbol_name,
                error_string
            );
            return;
        }

        return symbol_value;
#   else
#       error("sim_dynlib_find_symbol(2) is unsupported")
#   endif
}

#endif // SIMSOFT_DYNLIB_C_
