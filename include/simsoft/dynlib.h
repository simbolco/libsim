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
         * @headerfile dynlib.h "simsoft/dynlib.h"
         * @brief Dynamic library handle
         */
        typedef void* Sim_LibraryHandle;

        /**
         * @fn void sim_load_library(1)
         * @relates Sim_LibraryHandle
         * @brief Loads a dynamic library via filename during runtime.
         * 
         * @param[in]  filename           Filename of the dynamic library.
         * @param[out] out_library_handle Pointer to fill with resulting library handle.
         * 
         * @return @c NULL on error (see remarks); handle to dynamic library otherwise.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_UNSUPRTD if not supported using your build configuration;
         *     @b SIM_RC_ERR_NULLPTR  if @e filename is @c NULL ;
         *     @b SIM_RC_FAILURE      if library could not be loaded
         *         (TODO: Use more specific error codes in future);
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT Sim_LibraryHandle C_CALL sim_load_library(
            const char* filename
        );

        /**
         * @fn void sim_unload_library(1)
         * @relates Sim_LibraryHandle
         * @brief Unloads a dynamic library handle.
         * 
         * @param[in] library_handle Handle of a dynamic library to unload.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_UNSUPRTD if not supported using your build configuration;
         *     @b SIM_RC_ERR_NULLPTR  if @e library_handle is @c NULL ;
         *     @b SIM_RC_FAILURE      if library could not be unloaded
         *         (TODO: Use more specific error codes in future);
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern void C_CALL sim_unload_library(
            Sim_LibraryHandle library_handle
        );

        /**
         * @fn void* sim_find_symbol(2)
         * @relates Sim_LibraryHandle
         * @brief Retrieves a function from a dynamic library via a symbol.
         * 
         * @param[in]  library_handle   Handle of a dynamic library.
         * @param[in]  symbol_name      The symbol to look for in the dynamic library.
         * 
         * @return @c NULL on error (see remarks);
         *         pointer corresponding to the given symbol name otherwise.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_UNSUPRTD if not supported using your build configuration;
         *     @b SIM_RC_ERR_NULLPTR  if @e library_handle or @e symbol_name are @c NULL ;
         *     @b SIM_RC_FAILURE      if the symbol couldn't be found/resolved
         *         (TODO: Use more specific error codes in future);
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void* C_CALL sim_find_symbol(
            Sim_LibraryHandle library_handle,
            const char*       symbol_name
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        class EXPORT Library {
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
