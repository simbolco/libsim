/**
 * @file dynlib.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for dynamic library/shared object loading functions
 * @version 0.2
 * @date 2020-07-04
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_DYNLIB_H_
#define SIMSOFT_DYNLIB_H_

#include "macro.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API
        
        /**
         * @brief Dynamic library handle
         */
        typedef void* Sim_LibraryHandle;

        /**
         * @brief Loads a dynamic library via filename during runtime.
         * 
         * @param[in]  filename           Filename of the dynamic library.
         * @param[out] out_library_handle Pointer to fill with resulting library handle.
         * 
         * @returns @c NULL if the OS couldn't load the library;
         *          handle to dynamic library otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e filename is @c NULL;
         * @throw SIM_ERR_UNSUPRTD if the operation isn't supported by the OS.
         * 
         * TODO: Use more specific error codes in the future.
         */
        DYNAPI_PROC(Sim_LibraryHandle, sim_dynlib_load,,
            const char* filename
        );

        /**
         * @brief Unloads a dynamic library handle.
         * 
         * @param[in] library_handle Handle of a dynamic library to unload.
         * 
         * @throw SIM_ERR_NULLPTR  if @e library_handle is @c NULL;
         * @throw SIM_ERR_UNSUPRTD if the operation isn't supported by the OS.
         * 
         * TODO: Use more specific error codes in the future.
         */
        DYNAPI_PROC(void, sim_dynlib_unload,,
            Sim_LibraryHandle library_handle
        );

        /**
         * @brief Retrieves a function from a dynamic library via a symbol.
         * 
         * @param[in]  library_handle   Handle of a dynamic library.
         * @param[in]  symbol_name      The symbol to look for in the dynamic library.
         * 
         * @returns @c NULL if the symbol couldn't be resolved;
         *          pointer corresponding to the given symbol name otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e library_handle or @e symbol_name are @c NULL;
         * @throw SIM_ERR_UNSUPRTD if the operation isn't supported by the OS.
         * 
         * TODO: Use more specific error codes in the future.
         */
        DYNAPI_PROC(void*, sim_dynlib_find_symbol,,
            Sim_LibraryHandle library_handle,
            const char*       symbol_name
        );
    
    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus // start C++ API

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft) // end SimSoft namespace

#endif // SIMSOFT_DYNLIB_H_
