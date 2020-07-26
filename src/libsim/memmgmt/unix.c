/**
 * @file unix/memmgmt.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/memmgmt.h on UNIX-likes/POSIX
 * @version 0.1
 * @date 2020-03-17
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_UNIX_MEMMGMT_C_
#define SIMSOFT_UNIX_MEMMGMT_C_

#include "../_memmgmt.h"

#include "<inttypes.h>"
#include <sys/mman.h>

static int _sim_unix_memmgmt_mem_access_flags_to_proti(Sim_MemoryAccess mem_access_flags) {
    int protection = 0;
    if (mem_access_flags == SIM_MEMACCESS_NONE)
        protection = PROT_NONE;
    else {
        if (mem_access_flags & SIM_MEMACCESS_READABLE)
            protection |= PROT_READ;
        else if (mem_access_flags & SIM_MEMACCESS_WRITABLE)
            protection |= PROT_WRITE;
        else if (mem_access_flags & SIM_MEMACCESS_EXECUTABLE)
            protection |= PROT_EXEC;
    }

    return protection;
}

void* _sim_sys_memmgmt_map_file_ptr(
    void*            starting_address,
    FILE*            file_ptr,
    size_t           length,
    size_t           offset,
    Sim_MemoryAccess mem_access_flags
) {
    int file_descriptor = fileno(file_ptr);
    int protection = _sim_unix_memmgmt_mem_access_flags_to_proti(mem_access_flags);

    void* mapped_address = mmap(
        (starting_address == (void*)-1) ? NULL : starting_address,
        length,
        protection,
        MAP_PRIVATE | (starting_address != (void*)-1) ? MAP_FIXED : 0,
        file_descriptor,
        (off_t)offset
    );

    // match Unix errno with SimSoft return code...
    if (mapped_address == MAP_FAILED) {
        switch (errno) {
        case EBADF:
        case ENODEV:
            THROW(SIM_ERR_BADFILE, "(sim_memmgmt_map_file_ptr) Bad file handle");

        case EINVAL:
        case ENXIO:
        case EOVERFLOW:
        case EPERM:
            THROW(SIM_ERR_INVALARG,
                "(sim_memmgmt_map_file_ptr) Address, length, or offset not aligned to "
                "page boundary"
            );

        case ENOTSUP:
            THROW(SIM_ERR_UNSUPRTD,
                "(sim_memmgmt_map_file_ptr) Not supported by this operating system"
            );

        default:
            return (void*)-1;
        }
    }

    return mapped_address;
}

void _sim_sys_memmgmt_unmap(void* mapped_address, size_t length) {
    if (munmap(mapped_address, length))
        THROW(SIM_ERR_INVALARG,
            "(sim_memmgmt_unmap) Invalid address/length combo: %p %" PRIu32,
            mapped_address,
            (uint32)length
        );
}

bool _sim_sys_memmgmt_protect(
    void*            starting_address,
    size_t           length,
    Sim_MemoryAccess mem_access_flags
) {
    int protection = _sim_unix_memmgmt_mem_access_flags_to_proti(mem_access_flags);

    if (!mprotect(starting_address, length, protection))
        return true;

    switch (errno) {
    case EACCES:
    case EINVAL:
    case ENOMEM:
        THROW(SIM_ERR_INVALARG,
            "(sim_memmgmt_protect) Invalid address/length combo: %p %" PRIu32,
            starting_address,
            (uint32)length
        );
    
    case ENOTSUP:
        THROW(SIM_ERR_UNSUPRTD,
            "(sim_memmgmt_protect) Not supported by this operating system"
        );

    default:
        return false;
    }
}

bool _sim_sys_memmgmt_sync(void* starting_address, size_t length) {
    if (!msync(starting_address, length, MS_ASYNC))
        return true;

    switch (errno) {
    case EINVAL:
    case ENOMEM:
        THROW(SIM_ERR_INVALARG,
            "(sim_memmgmt_sync) Invalid address/length combo: %p %" PRIu32,
            starting_address,
            (uint32)length
        );

    default:
        RETURN(SIM_RC_FAILURE, false);
    }
}

bool _sim_sys_memmgmt_lock(void* starting_address, size_t length) {
    if (!mlock(starting_address, length))
        return true;

    switch (errno) {
    case EINVAL:
    case ENOMEM:
        THROW(SIM_ERR_INVALARG,
            "(sim_memmgmt_lock) Invalid address/length combo: %p %" PRIu32,
            starting_address,
            (uint32)length
        );

    default:
        return false;
    }
}

bool _sim_sys_memmgmt_unlock(void* starting_address, size_t length) {
    if (!munlock(starting_address, length))
        return true;

    switch (errno) {
    case EINVAL:
    case ENOMEM:
        
        THROW(SIM_ERR_INVALARG,
            "(sim_memmgmt_unlock) Invalid address/length combo: %p %" PRIu32,
            starting_address,
            (uint32)length
        );

    default:
        return false;
    }
}

#endif // SIMSOFT_UNIX_MEMMGMT_C_