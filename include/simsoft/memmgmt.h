/**
 * @file memmgmt.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for cross-platform memory management.
 * @version 0.1
 * @date 2020-03-17
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_MEMMGMT_H_
#define SIMSOFT_MEMMGMT_H_

#include "./common.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

        /**
         * @enum Sim_MemoryAccess
         * @headerfile memmgmt.h "simsoft/memmgmt.h"
         * @brief C enumeration for memory access permissions.
         * 
         * @var Sim_MemoryAccess::SIM_MEMACCESS_NONE
         *     No access permissions.
         * @var Sim_MemoryAccess::SIM_MEMACCESS_READABLE
         *     Allowed to read from a given region.
         * @var Sim_MemoryAccess::SIM_MEMACCESS_WRITABLE
         *     Allowed to write to a given region.
         * @var Sim_MemoryAccess::SIM_MEMACCESS_EXECUTABLE
         *     Allowed to execute code in the given region.
         */
        typedef enum Sim_MemoryAccess {
            SIM_MEMACCESS_NONE = 0,
            SIM_MEMACCESS_READABLE   = 0x1,
            SIM_MEMACCESS_WRITABLE   = 0x2,
            SIM_MEMACCESS_EXECUTABLE = 0x4
        } Sim_MemoryAccess;

        /**
         * @fn void* sim_memmgmt_map_file_ptr(FILE*, size_t, size_t, Sim_MemoryAccess)
         * @headerfile memmgmt.h "simsoft/memmgmt.h"
         * @brief Maps a standard C FILE* into the virtual address space.
         * 
         * @param[in] file_ptr         Standard C FILE* to map to the address space.
         * @param[in] length           The size of the memory region.
         * @param[in] offset           Offset into the file to start reading from.
         * @param[in] mem_access_flags Permissions regarding the newly created memory region.
         * 
         * @returns Starting address of the newly created memory region for the given file;
         *          @c (void*)-1 on error (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_BADFILE  if @e file_ptr is an invalid file pointer or @c NULL;
         *     @b SIM_RC_ERR_INVALARG if @e length, @e offset, or @e mem_access_flags are invalid
         *                            (TODO: more specific description of what "invalid" means);
         *     @b SIM_RC_ERR_UNSUPRTD if the operation isn't supported by the OS;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void* C_CALL sim_memmgmt_map_file_ptr(
            FILE*            file_ptr,
            size_t           length,
            size_t           offset,
            Sim_MemoryAccess mem_access_flags
        );

        /**
         * @fn void* sim_memmgmt_map_fixed_file_ptr(void*, FILE*, size_t, size_t, Sim_MemoryAccess)
         * @headerfile memmgmt.h "simsoft/memmgmt.h"
         * @brief Maps a standard C FILE* into the virtual address space at a fixed address.
         * 
         * @param[in] starting_address The starting address of the mapped memory region.
         * @param[in] file_ptr         Standard C FILE* to map to the address space.
         * @param[in] length           The size of the memory region.
         * @param[in] offset           Offset into the file to start reading from.
         * @param[in] mem_access_flags Permissions regarding the newly created memory region.
         * 
         * @returns Starting address of the newly created memory region for the given file;
         *          @c (void*)-1 on error (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_BADFILE  if @e file_ptr is an invalid file pointer or @c NULL;
         *     @b SIM_RC_ERR_INVALARG if @e length, @e offset, or @e mem_access_flags are invalid
         *                            (TODO: more specific description of what "invalid" means) or
         *                            if @e starting_address isn't a multiple of the OS page size;
         *     @b SIM_RC_ERR_UNSUPRTD if the operation isn't supported by the OS;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void* C_CALL sim_memmgmt_map_fixed_file_ptr(
            void*            starting_address,
            FILE*            file_ptr,
            size_t           length,
            size_t           offset,
            Sim_MemoryAccess mem_access_flags
        );

        /**
         * @fn bool sim_memmgmt_unmap(void*, size_t)
         * @headerfile memmgmt.h "simsoft/memmgmt.h"
         * @brief Unmaps a memory region created by one of the sim_memmgmt_map_* functions.
         * 
         * @param[in] mapped_address A memory-mapped address.
         * @param[in] length         The size of the mapped region.
         * 
         * @returns @c true on success; @c false otherwise (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_INVALARG if @e length == 0 or @e mapped_address doesn't correspond to
         *                             a mapped address;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT bool C_CALL sim_memmgmt_unmap(
            void*  mapped_address,
            size_t length
        );

        /**
         * @fn bool sim_memmgmt_protect(void*, size_t, Sim_MemoryAccess)
         * @headerfile memmgmt.h "simsoft/memmgmt.h"
         * @brief Protects a memory region by assigning memory access permissions.
         * 
         * @param[in] starting_address The starting address of the memory region to protect.
         * @param[in] length           Size of the memory region to protect.
         * @param[in] mem_access_flags Memory access permissions for the memory region.
         * 
         * @returns @c true on success; @c false otherwise (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_INVALARG if @e length == 0, @e starting_address isn't aligned to
         *                             the OS page size, or if @e mem_access_flags cannot be
         *                             applied to the specified memory region;
         *     @b SIM_RC_ERR_UNSUPRTD if the operation isn't supported by the OS;
         *     @b SIM_RC_FAILURE      on some other OS-specific failure;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT bool C_CALL sim_memmgmt_protect(
            void*            starting_address,
            size_t           length,
            Sim_MemoryAccess mem_access_flags
        );

        /**
         * @fn bool sim_memmgmt_sync(void*, size_t)
         * @headerfile memmgmt.h "simsoft/memmgmt.h"
         * @brief Flushes a given memory region's write operations.
         * 
         * @param[in] starting_address The starting address of the memory region to synchronize.
         * @param[in] length           Size of the memory region to synchronize.
         * 
         * @returns @c true on success; @c false otherwise (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_INVALARG if @e length == 0 or @e starting_address isn't aligned to
         *                             the OS page size.
         *     @b SIM_RC_FAILURE      on some other OS-specific failure;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT bool C_CALL sim_memmgmt_sync(
            void*  starting_address,
            size_t length
        );

        /**
         * @fn bool sim_memmgmt_lock(void*, size_t)
         * @headerfile memmgmt.h "simsoft/memmgmt.h"
         * @brief Locks access to a memory region.
         * 
         * @param[in] starting_address The starting address of the memory region to lock.
         * @param[in] length           Size of the memory region to lock.
         * 
         * @returns @c true on success; @c false otherwise (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_INVALARG if @e length == 0 or @e starting_address isn't aligned to
         *                             the OS page size.
         *     @b SIM_RC_FAILURE      on some other OS-specific failure;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT bool C_CALL sim_memmgmt_lock(
            void*  starting_address,
            size_t length
        );

        /**
         * @fn bool sim_memmgmt_unlock(void*, size_t)
         * @headerfile memmgmt.h "simsoft/memmgmt.h"
         * @brief Unlocks access to a memory region.
         * 
         * @param[in] starting_address The starting address of the memory region to unlock.
         * @param[in] length           Size of the memory region to unlock.
         * 
         * @returns @c true on success; @c false otherwise (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_INVALARG if @e length == 0 or @e starting_address isn't aligned to
         *                             the OS page size.
         *     @b SIM_RC_FAILURE      on some other OS-specific failure;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT bool C_CALL sim_memmgmt_unlock(
            void*  starting_address,
            size_t length
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        //

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_MEMMGMT_H_ */
