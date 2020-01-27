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

        typedef void* Sim_LibraryHandle;

        extern SIM_API Sim_ReturnCode C_CALL sim_load_library(
            const char* filename,
            Sim_LibraryHandle *const out_library_handle
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_unload_library(
            Sim_LibraryHandle library_handle
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_find_symbol(
            Sim_LibraryHandle library_handle,
            const char* symbol_name,
            void* *const out_symbol_value
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
