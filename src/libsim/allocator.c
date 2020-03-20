/**
 * @file allocator.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/allocator.h
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_ALLOCATOR_C_
#define SIMSOFT_ALLOCATOR_C_

#include "simsoft/allocator.h"

static const Sim_IAllocator SIM_DEFAULT_ALLOCATOR = {
    sim_allocator_default_malloc,
    sim_allocator_default_falloc,
    sim_allocator_default_realloc,
    sim_allocator_default_free
};
static const Sim_IAllocator* SIM_EXTERN_DEFAULT_ALLOCATOR = &SIM_DEFAULT_ALLOCATOR;

// sim_allocator_default_malloc(1): The default memory allocation function used by the default
//                                  allocator.
void* sim_allocator_default_malloc(size_t size) {
    return malloc(size);
}

// sim_allocator_default_falloc(2): The default filled memory allocation function used by the
//                                  default allocator.
void* sim_allocator_default_falloc(size_t size, uint8 fill) {
    void* ptr = malloc(size);

    if (ptr)
        memset(ptr, (int)fill, size);

    return ptr;
}

// sim_allocator_default_realloc(2): The default memory reallocation function used by the default
//                                   allocator.
void* sim_allocator_default_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

// sim_allocator_default_free(1): The default memory free function used by the default allocator.
void sim_allocator_default_free(void* ptr) {
    free(ptr);
}

// sim_allocator_get_default(0): Retrieve the default allocator.
const Sim_IAllocator *const sim_allocator_get_default(void) {
    return SIM_EXTERN_DEFAULT_ALLOCATOR;
}

// sim_allocator_set_default(1): Get the default allocator.
void sim_allocator_set_default(Sim_IAllocator *const allocator) {
    SIM_EXTERN_DEFAULT_ALLOCATOR = allocator ?
        allocator :
        &SIM_DEFAULT_ALLOCATOR
    ;
}

#endif /* SIMSOFT_ALLOCATOR_C_ */
