/**
 * @file memmgmt.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/memmgmt.h
 * @version 0.1
 * @date 2020-03-17
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_MEMMGMT_C_
#define SIMSOFT_MEMMGMT_C_

#include "./_memmgmt.h"

// sim_memmgmt_map_file_ptr(4): Maps a standard C FILE* into the virtual address space.
void* sim_memmgmt_map_file_ptr(
    FILE*            file_ptr,
    size_t           length,
    size_t           offset,
    Sim_MemoryAccess mem_access_flags
) {
    RETURN_IF(length == 0, SIM_RC_ERR_INVALARG, (void*)-1);

    return _sim_sys_memmgmt_map_file_ptr((void*)-1, file_ptr, length, offset, mem_access_flags);
}

// sim_memmgmt_map_fixed_file_ptr(5): Maps a standard C FILE* into the virtual address space at a
//                                    fixed address.
void* sim_memmgmt_map_fixed_file_ptr(
    void*            starting_address,
    FILE*            file_ptr,
    size_t           length,
    size_t           offset,
    Sim_MemoryAccess mem_access_flags
) {
    RETURN_IF(!file_ptr, SIM_RC_ERR_BADFILE, (void*)-1);
    RETURN_IF(length == 0, SIM_RC_ERR_INVALARG, (void*)-1);

    return _sim_sys_memmgmt_map_file_ptr(
        starting_address,
        file_ptr,
        length,
        offset,
        mem_access_flags
    );
}

// sim_memmgmt_unmap(2): Unmaps a memory region created by one of the sim_memmgmt_map_* functions.
bool sim_memmgmt_unmap(void* mapped_address, size_t length) {
    return _sim_sys_memmgmt_unmap(mapped_address, length);
}

// sim_memmgmt_protect(3): Protects a memory region by assigning memory access permissions.
bool sim_memmgmt_protect(
    void*            starting_address,
    size_t           length,
    Sim_MemoryAccess mem_access_flags
) {
    RETURN_IF(length == 0, SIM_RC_ERR_INVALARG, false);

    return _sim_sys_memmgmt_protect(starting_address, length, mem_access_flags);
}

// sim_memmgmt_sync(2): Flushes a given memory region's write operations.
bool sim_memmgmt_sync(void* starting_address, size_t length) {
    RETURN_IF(length == 0, SIM_RC_ERR_INVALARG, false);

    return _sim_sys_memmgmt_sync(starting_address, length);
}

// sim_memmgmt_lock(2): Locks access to a memory region.
bool sim_memmgmt_lock(void* starting_address, size_t length) {
    RETURN_IF(length == 0, SIM_RC_ERR_INVALARG, false);

    return _sim_sys_memmgmt_lock(starting_address, length);
}

// sim_memmgmt_unlock(2): Unlocks access to a memory region.
bool sim_memmgmt_unlock(void* starting_address, size_t length) {
    RETURN_IF(length == 0, SIM_RC_ERR_INVALARG, false);

    return _sim_sys_memmgmt_unlock(starting_address, length);
}

#endif /* SIMSOFT_MEMMGMT_C_ */
