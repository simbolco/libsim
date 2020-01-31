/**
 * @file dynlib.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for dynamic library/shared object loading functions
 * @version 0.1
 * @date 2020-01-23
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_DYNLIB_H_
#define SIMSOFT_DYNLIB_H_

#include "./common.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */
        
        /**
         * @typedef Sim_LibraryHandle
         * @brief Dynamic library handle; varies based on platform
         */
        typedef void* Sim_LibraryHandle;

        /**
         * @fn Sim_ReturnCode sim_load_library(2)
         * @brief Loads a dynamic library via filename during runtime.
         * 
         * @param[in]  filename           Filename of the dynamic library.
         * @param[out] out_library_handle Pointer to fill with resulting library handle.
         * 
         * @return @b SIM_RC_ERR_UNSUPRTD if not supported using your build configuration;
         *         @b SIM_RC_ERR_NULLPTR  if @e filename or @e out_library_handle are @c NULL ;
         *         @b SIM_RC_FAILURE      if library could not be loaded
         *             (TODO: Use more specific error codes in future);
         *         @b SIM_RC_SUCCESS      otherwise.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_load_library(
            const char*              filename,
            Sim_LibraryHandle *const out_library_handle
        );

        /**
         * @fn Sim_ReturnCode sim_unload_library(1)
         * @brief Unloads a dynamic library handle.
         * 
         * @param[in] library_handle Handle of a dynamic library to unload.
         * 
         * @return @b SIM_RC_ERR_UNSUPRTD if not supported using your build configuration;
         *         @b SIM_RC_ERR_NULLPTR  if @e library_handle is @c NULL ;
         *         @b SIM_RC_FAILURE      if library could not be unloaded
         *             (TODO: Use more specific error codes in future);
         *         @b SIM_RC_SUCCESS      otherwise.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_unload_library(
            Sim_LibraryHandle library_handle
        );

        /**
         * @fn Sim_ReturnCode sim_find_symbol(3)
         * @brief Retrieves a function from a dynamic library via a symbol.
         * 
         * @param[in]  library_handle   Handle of a dynamic library.
         * @param[in]  symbol_name      The symbol to look for in the dynamic library.
         * @param[out] out_symbol_value The value corresponding to the given symbol name.
         * 
         * @return @b SIM_RC_ERR_UNSUPRTD if not supported using your build configuration;
         *         @b SIM_RC_ERR_NULLPTR  if @e library_handle, @e symbol_name, or
         *                                @e out_symbol_value are @c NULL ;
         *         @b SIM_RC_FAILURE      if the symbol couldn't be found/resolved
         *             (TODO: Use more specific error codes in future);
         *         @b SIM_RC_SUCCESS      otherwise.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_find_symbol(
            Sim_LibraryHandle library_handle,
            const char*       symbol_name,
            void* *const      out_symbol_value
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        class SIM_API Library {
        private:
            C_API::Sim_LibraryHandle _library_handle;

        public:
            Library(const char* filename);
            Library(const Library& other);
            Library(Library&& other);
            virtual ~Library();

            Library& operator=(const Library& other);
            Library& operator=(Library&& other);

            void* find_symbol(const char* symbol_name);
        };

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_DYNLIB_H_ */
