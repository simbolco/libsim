/**
 * @file win32/memmgmt.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/memmgmt.h on Win32
 * @version 0.1
 * @date 2020-03-17
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_WIN32_MEMMGMT_C_
#define SIMSOFT_WIN32_MEMMGMT_C_

#include "../_memmgmt.h"
#include <io.h>

static DWORD _sim_win32_mem_access_flags_to_dwpage(Sim_MemoryAccess mem_access_flags) {
    DWORD protect = 0;

    if (mem_access_flags == SIM_MEMACCESS_NONE)
        return protect;
    
    if (mem_access_flags & SIM_MEMACCESS_EXECUTABLE)
        protect = (mem_access_flags & SIM_MEMACCESS_WRITABLE) ?
            PAGE_EXECUTE_READWRITE :
            PAGE_EXECUTE_READ
        ;
    else
        protect = (mem_access_flags & SIM_MEMACCESS_WRITABLE) ?
            PAGE_READWRITE :
            PAGE_READONLY
        ;
    
    return protect;
}

static DWORD _sim_win32_mem_access_flags_to_dwfile(Sim_MemoryAccess mem_access_flags) {
    DWORD desired_access = 0;

    if (mem_access_flags == SIM_MEMACCESS_NONE)
        return desired_access;
    
    if (mem_access_flags & SIM_MEMACCESS_READABLE)
        desired_access |= FILE_MAP_READ;
    if (mem_access_flags & SIM_MEMACCESS_WRITABLE)
        desired_access |= FILE_MAP_WRITE;
    if (mem_access_flags & SIM_MEMACCESS_EXECUTABLE)
        desired_access |= FILE_MAP_EXECUTE;
    
    return desired_access;
}

void* _sim_sys_memmgmt_map_file_ptr(
    void*            starting_address,
    FILE*            file_ptr,
    size_t           length,
    size_t           offset,
    Sim_MemoryAccess mem_access_flags
) {
    // set up all the ugly Win32 args (thank you mman-win32)...
    const DWORD file_offset_low = (sizeof(size_t) <= sizeof(DWORD)) ?
        (DWORD)offset :
        (DWORD)(offset & 0xFFFFFFFFL)
    ;
    const DWORD file_offset_high = (sizeof(size_t) <= sizeof(DWORD)) ?
        (DWORD)0 :
        (DWORD)((offset >> 32) & 0xFFFFFFFFL)
    ;
    const DWORD protect = _sim_win32_mem_access_flags_to_dwpage(mem_access_flags);
    const DWORD desired_access = _sim_win32_mem_access_flags_to_dwfile(mem_access_flags);

    const size_t max_size = offset + length;
    const DWORD max_size_low = (sizeof(size_t) <= sizeof(DWORD)) ?
        (DWORD)max_size :
        (DWORD)(max_size & 0xFFFFFFFFL)
    ;
    const DWORD max_size_high = (sizeof(size_t) <= sizeof(DWORD)) ?
        (DWORD)0 :
        (DWORD)((max_size >> 32) & 0xFFFFFFFFL)
    ;

    // grab Win32 file handle & validate
    HANDLE file_handle = (HANDLE)_get_osfhandle(_fileno(file_ptr));
    RETURN_IF(file_handle == INVALID_HANDLE_VALUE, SIM_RC_ERR_BADFILE, (void*)-1);

    // create the file mapping
    HANDLE file_mapping_handle = CreateFileMapping(
        file_handle,
        NULL,
        protect,
        max_size_high, max_size_low,
        NULL
    );
    if (!file_mapping_handle) {
        _sim_win32_print_last_error(
            "CreateFileMapping(%p, %p, %ld, %ld, %ld, %p)",
            file_handle,
            NULL,
            protect,
            max_size_high, max_size_low,
            NULL
        );
        RETURN(SIM_RC_FAILURE, (void*)-1);
    }

    void* mapped_address;
    if (starting_address != (void*)-1) /* non-fixed mappings */ {
        mapped_address = MapViewOfFile(
            file_mapping_handle,
            desired_access,
            file_offset_high, file_offset_low,
            (SIZE_T)length
        );
        CloseHandle(file_mapping_handle);

        // validate that mapping succeded...
        if (!mapped_address) {
            _sim_win32_print_last_error(
                "MapViewOfFile(%p, %ld, %ld, %ld, %llu)",
                file_mapping_handle,
                desired_access,
                file_offset_high, file_offset_low,
                (unsigned long long)length
            );
            RETURN(SIM_RC_FAILURE, (void*)-1);
        }
    } else /* fixed mappings */ {
        mapped_address = MapViewOfFileEx(
            file_mapping_handle,
            desired_access,
            file_offset_high, file_offset_low,
            (SIZE_T)length,
            starting_address
        );
        CloseHandle(file_mapping_handle);

        // validate that mapping succeded...
        if (!mapped_address) {
            _sim_win32_print_last_error(
                "MapViewOfFileEx(%p, %ld, %ld, %ld, %llu, %p)",
                file_mapping_handle,
                desired_access,
                file_offset_high, file_offset_low,
                (unsigned long long)length,
                starting_address
            );
            RETURN(SIM_RC_FAILURE, (void*)-1);
        }
    }

    return mapped_address;
}

bool _sim_sys_memmgmt_unmap(void* mapped_address, size_t length) {
    (void)length;

    RETURN_IF(UnmapViewOfFile(mapped_address), SIM_RC_SUCCESS, true);
    
    _sim_win32_print_last_error("UnmapViewOfFile(%p)", mapped_address);
    RETURN(SIM_RC_ERR_INVALARG, false);
}

bool _sim_sys_memmgmt_protect(
    void*            starting_address,
    size_t           length,
    Sim_MemoryAccess mem_access_flags
) {
    const DWORD protect = _sim_win32_mem_access_flags_to_dwpage(mem_access_flags);
    DWORD old_protect = 0;
    RETURN_IF(
        VirtualProtect(starting_address, (SIZE_T)length, protect, &old_protect),
        SIM_RC_SUCCESS, true
    );

    _sim_win32_print_last_error(
        "VirtualProtect(%p, %llu, %ld, %p",
        starting_address,
        (unsigned long long)length,
        (long)protect,
        &old_protect
    );
    RETURN(SIM_RC_ERR_INVALARG, false);
}

bool _sim_sys_memmgmt_sync(void* starting_address, size_t length) {
    RETURN_IF(FlushViewOfFile(starting_address, length), SIM_RC_SUCCESS, true);

    _sim_win32_print_last_error(
        "FlushViewOfFile(%p, %llu)",
        starting_address,
        (unsigned long long)length
    );
    RETURN(SIM_RC_ERR_INVALARG, false);
}

bool _sim_sys_memmgmt_lock(void* starting_address, size_t length) {
    RETURN_IF(VirtualLock(starting_address, length), SIM_RC_SUCCESS, true);

    _sim_win32_print_last_error(
        "VirtualLock(%p, %llu)",
        starting_address,
        (unsigned long long)length
    );
    RETURN(SIM_RC_ERR_INVALARG, false);
}

bool _sim_sys_memmgmt_unlock(void* starting_address, size_t length) {
    RETURN_IF(VirtualUnlock(starting_address, length), SIM_RC_SUCCESS, true);

    _sim_win32_print_last_error(
        "VirtualUnlock(%p, %llu)",
        starting_address,
        (unsigned long long)length
    );
    RETURN(SIM_RC_ERR_INVALARG, false);
}

#endif /* SIMSOFT_WIN32_MEMMGMT_C_ */
