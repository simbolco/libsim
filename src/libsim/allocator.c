/**
 * @file allocator.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/allocator.h
 * @version 0.2
 * @date 2020-07-04
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_ALLOCATOR_C_
#define SIMSOFT_ALLOCATOR_C_

#include <stdlib.h>
#include <string.h>

#include "simsoft/allocator.h"

static Sim_IAllocator SIM_DEFAULT_ALLOCATOR = {
    sim_allocator_default_alloc,
    sim_allocator_default_calloc,
    sim_allocator_default_resize,
    sim_allocator_default_free
};
static Sim_IAllocator* SIM_EXTERN_DEFAULT_ALLOCATOR = &SIM_DEFAULT_ALLOCATOR;

// sim_allocator_default_alloc(2): The default memory allocation function used by the default
//                                 allocator.
void* sim_allocator_default_alloc(Sim_IAllocator *const alloc, size_t size) {
    UNUSED(alloc)

    return malloc(size);
}

// sim_allocator_default_calloc(3): The default filled memory allocation function used by the
//                                  default allocator.
void* sim_allocator_default_calloc(Sim_IAllocator *const alloc, size_t count, size_t size) {
    UNUSED(alloc)
    
    void* ptr = calloc(count, size);

    return ptr;
}

// sim_allocator_default_resize(3): The default memory reallocation function used by the default
//                                  allocator.
void* sim_allocator_default_resize(Sim_IAllocator *const alloc, void* ptr, size_t size) {
    UNUSED(alloc)

    return realloc(ptr, size);
}

// sim_allocator_default_free(2): The default memory free function used by the default allocator.
void sim_allocator_default_free(Sim_IAllocator *const alloc, void* ptr) {
    UNUSED(alloc)

    free(ptr);
}

// sim_allocator_get_default(0): Retrieve the default allocator.
Sim_IAllocator* sim_allocator_get_default(void) {
    return SIM_EXTERN_DEFAULT_ALLOCATOR;
}

// sim_allocator_set_default(1): Get the default allocator.
void sim_allocator_set_default(Sim_IAllocator *const allocator) {
    SIM_EXTERN_DEFAULT_ALLOCATOR = allocator ?
        allocator :
        &SIM_DEFAULT_ALLOCATOR
    ;
}

#endif // SIMSOFT_ALLOCATOR_C_
