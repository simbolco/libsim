/**
 * @file memmgmt.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for cross-platform memory management.
 * @version 0.2
 * @date 2020-07-25
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_MEMMGMT_H_
#define SIMSOFT_MEMMGMT_H_

#include <stdio.h>

#include "macro.h"
#include "typedefs.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API

        /**
         * @brief C enumeration for memory access permissions.
         */
        typedef enum Sim_MemoryAccess {
            SIM_MEMACCESS_NONE = 0,         ///< No access permissions.
            SIM_MEMACCESS_READABLE   = 0x1, ///< Allowwed to read from a given region.
            SIM_MEMACCESS_WRITABLE   = 0x2, ///< Allowed to write to a given region.
            SIM_MEMACCESS_EXECUTABLE = 0x4  ///< Allowed to execute code in the given region.
        } Sim_MemoryAccess;

        /**
         * @brief Maps a standard C FILE* into the virtual address space.
         * 
         * @param[in] file_ptr         Standard C FILE* to map to the address space.
         * @param[in] length           The size of the memory region.
         * @param[in] offset           Offset into the file to start reading from.
         * @param[in] mem_access_flags Permissions regarding the newly created memory region.
         * 
         * @returns Starting address of the newly created memory region for the given file;
         *          @c (void*)-1 on error.
         * 
         * @throw SIM_ERR_BADFILE  if @e file_ptr is an invalid file pointer or @c NULL;
         * @throw SIM_ERR_INVALARG if @e length, @e offset, of @e mem_access_flags are invalid
         *                         (TODO: more specific description of what "invalid" means);
         * @throw SIM_ERR_UNSUPRTD if the operation isn't supported by the OS.
         */
        DYNAPI_PROC(void*, sim_memmgmt_map_file_ptr,,
            FILE*            file_ptr,
            size_t           length,
            size_t           offset,
            Sim_MemoryAccess mem_access_flags
        );

        /**
         * @brief Maps a standard C FILE* into the virtual address space at a fixed address.
         * 
         * @param[in] starting_address The starting address of the mapped memory region.
         * @param[in] file_ptr         Standard C FILE* to map to the address space.
         * @param[in] length           The size of the memory region.
         * @param[in] offset           Offset into the file to start reading from.
         * @param[in] mem_access_flags Permissions regarding the newly created memory region.
         * 
         * @returns Starting address of the newly created memory region for the given file;
         *          @c (void*)-1 on error.
         * 
         * @throw SIM_ERR_BADFILE  if @e file_ptr is an invalid file pointer or @c NULL;
         * @throw SIM_ERR_INVALARG if @e length, @e offset, of @e mem_access_flags are invalid
         *                         (TODO: more specific description of what "invalid" means) or
         *                         if @e starting_address isn't a multiple of the OS page size;
         * @throw SIM_ERR_UNSUPRTD if the operation isn't supported by the OS.
         */
        DYNAPI_PROC(void*, sim_memmgmt_map_fixed_file_ptr,,
            void*            starting_address,
            FILE*            file_ptr,
            size_t           length,
            size_t           offset,
            Sim_MemoryAccess mem_access_flags
        );

        /**
         * @brief Unmaps a memory region created by one of the sim_memmgmt_map_* functions.
         * 
         * @param[in] mapped_address A memory-mapped address.
         * @param[in] length         The size of the mapped region.
         * 
         * @throw SIM_ERR_INVALARG if @e length == 0 or @e mapped_address doesn't correspond to an
         *                         memmgmt_mapped address.
         */
        DYNAPI_PROC(void, sim_memmgmt_unmap,,
            void*  mapped_address,
            size_t length
        );

        /**
         * @brief Protects a memory region by assigning memory access permissions.
         * 
         * @param[in] starting_address The starting address of the memory region to protect.
         * @param[in] length           Size of the memory region to protect.
         * @param[in] mem_access_flags Memory access permissions for the memory region.
         * 
         * @returns @c true on success; @c false if the OS failed to perform the operation.
         * 
         * @throw SIM_ERR_INVALARG if @e length == 0, @e starting_address isn't aligned to the OS
         *                         page size, or if @e mem_access_flags cannot be applied to the
         *                         specified memory region;
         * @throw SIM_ERR_UNSUPRTD if the operation isn't supported by the OS.
         */
        DYNAPI_PROC(bool, sim_memmgmt_protect,,
            void*            starting_address,
            size_t           length,
            Sim_MemoryAccess mem_access_flags
        );

        /**
         * @brief Flushes a given memory region's write operations.
         * 
         * @param[in] starting_address The starting address of the memory region to synchronize.
         * @param[in] length           Size of the memory region to synchronize.
         * 
         * @returns @c true on success; @c false if the OS failed to perform the operation.
         * 
         * @throw SIM_ERR_INVALARG if @e length == 0 or @e starting_address isn't aligned to the
         *                         OS page size.
         */
        DYNAPI_PROC(bool, sim_memmgmt_sync,,
            void*  starting_address,
            size_t length
        );

        /**
         * @brief Locks access to a memory region.
         * 
         * @param[in] starting_address The starting address of the memory region to lock.
         * @param[in] length           Size of the memory region to lock.
         * 
         * @returns @c true on success; @c false if the OS failed to perform the operation.
         * 
         * @throw SIM_ERR_INVALARG if @e length == 0 or @e starting_address isn't aligned to the
         *                         OS page size.
         */
        DYNAPI_PROC(bool, sim_memmgmt_lock,,
            void*  starting_address,
            size_t length
        );

        /**
         * @brief Unlocks access to a memory region.
         * 
         * @param[in] starting_address The starting address of the memory region to unlock.
         * @param[in] length           Size of the memory region to unlock.
         * 
         * @returns @c true on success; @c false if the OS failed to perform the operation.
         * 
         * @throw SIM_ERR_INVALARG if @e length == 0 or @e starting_address isn't aligned to the
         *                         OS page size.
         */
        DYNAPI_PROC(bool, sim_memmgmt_unlock,,
            void*  starting_address,
            size_t length
        );
    
    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus // start C++ API
        
        //

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft) // end SimSoft namespace

#endif // SIMSOFT_MEMMGMT_H_
