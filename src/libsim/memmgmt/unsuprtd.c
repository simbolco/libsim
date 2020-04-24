/**
 * @file unsuprtd/memmgmt.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/memmgmt.h on unsupported platforms
 * @version 0.1
 * @date 2020-03-17
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_UNSUPRTD_MEMMGMT_C_
#define SIMSOFT_UNSUPRTD_MEMMGMT_C_

#include "../_memmgmt.h"

void* _sim_sys_memmgmt_map_file_ptr(
    void*            starting_address,
    FILE*            file_ptr,
    size_t           length,
    size_t           offset,
    Sim_MemoryAccess mem_access_flags
) {
#   warning("sim_memmgmt_map_file_ptr(4) and sim_memmgmt_map_fixed_file_ptr(5) are unsupported")
    (void)starting_address; (void)file_ptr; (void)length; (void)offset; (void)mem_access_flags;
    THROW(SIM_RC_ERR_UNSUPRTD);
}

bool _sim_sys_memmgmt_unmap(void* mapped_address, size_t length) {
#   warning("sim_memmgmt_unmap(2) is unsupported")
    (void)mapped_address; (void)length;
    THROW(SIM_RC_ERR_UNSUPRTD);
}

bool _sim_sys_memmgmt_protect(
    void*            starting_address,
    size_t           length,
    Sim_MemoryAccess mem_access_flags
) {
#   warning("sim_memmgmt_protect(3) is unsupported")
    (void)starting_address; (void)length; (void)mem_access_flags;
    THROW(SIM_RC_ERR_UNSUPRTD);
}

bool _sim_sys_memmgmt_sync(void* starting_address, size_t length) {
#   warning("sim_memmgmt_sync(2) is unsupported")
    (void)starting_address; (void)length;
    THROW(SIM_RC_ERR_UNSUPRTD);
}

bool _sim_sys_memmgmt_lock(void* starting_address, size_t length) {
#   warning("sim_memmgmt_lock(2) is unsupported")
    (void)starting_address; (void)length;
    THROW(SIM_RC_ERR_UNSUPRTD);
}

bool _sim_sys_memmgmt_unlock(void* starting_address, size_t length) {
#   warning("sim_memmgmt_unlock(2) is unsupported")
    (void)starting_address; (void)length;
    THROW(SIM_RC_ERR_UNSUPRTD);
}

#endif /* SIMSOFT_UNSUPRTD_MEMMGMT_C_ */
