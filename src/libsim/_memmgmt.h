/**
 * @file _memmgmt.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Internal header for simsoft/memmgmt.h implementations
 * @version 0.1
 * @date 2020-03-17
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT__MEMMGMT_H_
#define SIMSOFT__MEMMGMT_H_

#include "./_internal.h"
#include "simsoft/memmgmt.h"

extern void* _sim_sys_memmgmt_map_file_ptr(
    void*            starting_address,
    FILE*            file_ptr,
    size_t           length,
    size_t           offset,
    Sim_MemoryAccess mem_access_flags
);

extern bool _sim_sys_memmgmt_unmap(void* mapped_address, size_t length);

extern bool _sim_sys_memmgmt_protect(
    void*            starting_address,
    size_t           length,
    Sim_MemoryAccess mem_access_flags
);

extern bool _sim_sys_memmgmt_sync(void* starting_address, size_t length);

extern bool _sim_sys_memmgmt_lock(void* starting_address, size_t length);

extern bool _sim_sys_memmgmt_unlock(void* starting_address, size_t length);

#endif /* SIMSOFT__MEMMGMT_H_ */
