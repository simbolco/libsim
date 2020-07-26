/**
 * @file allocator.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for allocator inferface
 * @version 0.2
 * @date 2020-07-04
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_ALLOCATOR_H_
#define SIMSOFT_ALLOCATOR_H_

#include "macro.h"
#include "typedefs.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API

        typedef struct Sim_IAllocator Sim_IAllocator;

        /**
         * @brief Virtual function pointer for dynamic memory allocation.
         * 
         * @param[in,out] alloc Pointer to the allocator associated with this function.
         * @param[in]     size  How much memory to allocate.
         * 
         * @returns @c NULL if out of memory; pointer to allocated space otherwise.
         * 
         * @sa Sim_AllocatorCallocProc
         * @sa Sim_AllocatorResizeProc
         * @sa Sim_AllocatorFreeProc
         */
        typedef void* (*Sim_AllocatorAllocProc)(
            Sim_IAllocator *const alloc,
            size_t                size
        );

        /**
         * @brief Virtual function pointer for zero-filled array dynamic memory allocation.
         * 
         * @param[in,out] alloc Pointer to the allocator associated with this function.
         * @param[in]     count How many elements to allocate.
         * @param[in]     size  How much memory each element is sized.
         * 
         * @returns @c NULL if out of memory; pointer to allocated space otherwise. 
         * 
         * @sa Sim_AllocatorAllocProc
         * @sa Sim_AllocatorResizeProc
         * @sa Sim_AllocatorFreeProc
         */
        typedef void* (*Sim_AllocatorCallocProc)(
            Sim_IAllocator *const alloc,
            size_t                count,
            size_t                size
        );

        /**
         * @brief Virtual function pointer for dynamic memory reallocation.
         * 
         * @param[in,out] alloc Pointer to the allocator associated with this function.
         * @param[in]     ptr   Pointer to memory allocated by @e alloc to reallocate.
         * @param[in]     size  Size request for reallocated memory.
         * 
         * @returns @c NULL if out of memory; pointer to reallocated space otherwise.
         * 
         * @sa Sim_AllocatorAllocProc
         * @sa Sim_AllocatorCallocProc
         * @sa Sim_AllocatorFreeProc
         */
        typedef void* (*Sim_AllocatorResizeProc)(
            Sim_IAllocator *const alloc,
            void*                 ptr,
            size_t                size
        );

        /**
         * @brief Virtual function pointer for freeing dynamically allocated memory.
         * 
         * @param[in,out] alloc Pointer to the allocator associated with this function.
         * @param[in]     ptr   Pointer to memory allocated by @e alloc to free.
         * 
         * @sa Sim_AllocatorAllocProc
         * @sa Sim_AllocatorCallocProc
         * @sa Sim_AllocatorResizeProc
         */
        typedef void  (*Sim_AllocatorFreeProc)(
            Sim_IAllocator *const alloc,
            void*                 ptr
        );

        /**
         * @brief Interface for a memory allocator.
         */
        struct Sim_IAllocator {
            Sim_AllocatorAllocProc  alloc;  ///< Memory allocation function.
            Sim_AllocatorCallocProc calloc; ///< Filled memory allocation function.
            Sim_AllocatorResizeProc resize; ///< Memory reallocation function.
            Sim_AllocatorFreeProc   free;   ///< Memory deallocation function.
        };

        /**
         * @brief The default memory allocation function used by the default allocator.
         * 
         * @param[in,out] alloc Pointer to the allocator associated with this function.
         * @param[in]     size  How much memory to allocate.
         * 
         * @returns @c NULL if out of memory; pointer to allocated space otherwise.
         * 
         * @sa sim_allocator_default_calloc
         * @sa sim_allocator_default_resize
         * @sa sim_allocator_default_free
         */
        DYNAPI_PROC(void*, sim_allocator_default_alloc,,
            Sim_IAllocator *const alloc,
            size_t size
        );

        /**
         * @brief The default filled memory allocation function used by the default allocator.
         * 
         * @param[in,out] alloc Pointer to the allocator associated with this function.
         * @param[in]     count How many elements to allocate.
         * @param[in]     size  How much memory each element is sized.
         * 
         * @returns @c NULL if out of memory; pointer to allocated space otherwise.
         * 
         * @sa sim_allocator_default_alloc
         * @sa sim_allocator_default_resize
         * @sa sim_allocator_default_free
         */
        DYNAPI_PROC(void*, sim_allocator_default_calloc,,
            Sim_IAllocator *const alloc,
            size_t count,
            size_t size
        );

        /**
         * @brief The default memory reallocation function used by the default allocator.
         * 
         * @param[in,out] alloc Pointer to the allocator associated with this function.
         * @param[in]     ptr   Pointer to malloc'd/falloc'd memory to reallocate.
         * @param[in]     size  Size request for reallocated memory.
         * 
         * @returns @c NULL if out of memory; pointer to reallocated space otherwise.
         * 
         * @sa sim_allocator_default_alloc
         * @sa sim_allocator_default_calloc
         * @sa sim_allocator_default_free
         */
        DYNAPI_PROC(void*, sim_allocator_default_resize,,
            Sim_IAllocator *const alloc,
            void*  ptr,
            size_t size
        );

        /**
         * @brief The default memory free function used by the default allocator.
         * 
         * @param[in,out] alloc Pointer to the allocator associated with this function.
         * @param[in]     ptr   Pointer to malloc'd/falloc'd/realloc'd memory to free.
         * 
         * @sa sim_allocator_default_alloc
         * @sa sim_allocator_default_calloc
         * @sa sim_allocator_default_resize
         */
        DYNAPI_PROC(void, sim_allocator_default_free,,
            Sim_IAllocator *const alloc,
            void* ptr
        );

        /**
         * @brief Retrieve the default allocator.
         * 
         * @returnsPointer to the default allocator.
         * 
         * @sa sim_allocator_set_default
         */
        DYNAPI_PROC(Sim_IAllocator*, sim_allocator_get_default,, void);
        
        /**
         * @brief Set the default allocator.
         * 
         * @param[in] allocator Pointer to an allocator to use as the default or @c NULL to
         *                      use the original default allocator.
         * 
         * @sa sim_allocator_get_default
         */
        DYNAPI_PROC(void, sim_allocator_set_default,,
            Sim_IAllocator *const allocator
        );
    
    CPP_NAMESPACE_C_API_END // end C API
#   ifdef __cplusplus       // start C++ API

        
        
#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft)

#endif // SIMSOFT_ALLOCATOR_H_
