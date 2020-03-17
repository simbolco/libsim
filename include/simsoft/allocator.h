/**
 * @file allocator.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for allocator inferface
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_ALLOCATOR_H_
#define SIMSOFT_ALLOCATOR_H_

#include "./common.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

        /**
         * @typedef Sim_MemAllocProc
         * @headerfile allocator.h "simsoft/allocator.h"
         * @relates Sim_IAllocator
         * @breif Function pointer for dynamic memory allocation.
         * 
         * @param[in] size How much memory to allocate.
         * 
         * @return @c NULL if out of memory; pointer to allocated space otherwise.
         * 
         * @sa Sim_MemFilledAllocProc
         * @sa Sim_MemReallocProc
         * @sa Sim_MemFreeProc
         */
        typedef void* (*Sim_MemAllocProc)(
            size_t size
        );

        /**
         * @typedef Sim_MemFilledAllocProc
         * @headerfile allocator.h "simsoft/allocator.h"
         * @relates Sim_IAllocator
         * @breif Function pointer for filled dynamic memory allocation.
         * 
         * @param[in] size How much memory to allocate.
         * @param[in] fill An unsigned byte to fill the allocated memory with.
         * 
         * @return @c NULL if out of memory; pointer to allocated space otherwise. 
         * 
         * @sa Sim_MemAllocProc
         * @sa Sim_MemReallocProc
         * @sa Sim_MemFreeProc
         */
        typedef void* (*Sim_MemFilledAllocProc)(
            size_t size,
            ubyte fill
        );

        /**
         * @typedef Sim_MemReallocProc
         * @headerfile allocator.h "simsoft/allocator.h"
         * @relates Sim_IAllocator
         * @brief Function pointer for dynamic memory reallocation.
         * 
         * @param[in] ptr  Pointer to malloc'd/falloc'd memory to reallocate.
         * @param[in] size Size request for reallocated memory.
         * 
         * @return @c NULL if out of memory; pointer to reallocated space otherwise.
         * 
         * @sa Sim_MemAllocProc
         * @sa Sim_MemFilledAllocProc
         * @sa Sim_MemFreeProc
         */
        typedef void* (*Sim_MemReallocProc)(
            void* ptr,
            size_t size
        );

        /**
         * @typedef Sim_MemFreeProc
         * @headerfile allocator.h "simsoft/allocator.h"
         * @relates Sim_IAllocator
         * @breif Function pointer for freeing dynamically allocated memory.
         * 
         * @param[in] ptr Pointer to malloc'd/falloc'd/realloc'd memory to free.
         * 
         * @sa Sim_MemAllocProc
         * @sa Sim_MemReallocProc
         * @sa Sim_MemFreeProc
         */
        typedef void  (*Sim_MemFreeProc)(
            void* ptr
        );


        /**
         * @struct Sim_IAllocator
         * @headerfile allocator.h "simsoft/allocator.h"
         * @brief Interface for a memory allocator.
         * 
         * @var malloc  Memory allocation function.
         * @var falloc  Filled memory allocation function.
         * @var realloc Memory reallocation function.
         * @var free    Allocated memory free function.
         */
        typedef struct Sim_IAllocator {
            Sim_MemAllocProc       malloc;
            Sim_MemFilledAllocProc falloc;
            Sim_MemReallocProc     realloc;
            Sim_MemFreeProc        free;
        } Sim_IAllocator;

        /**
         * @fn void* sim_allocator_default_malloc(1)
         * @relates Sim_IAllocator
         * @brief The default memory allocation function used by the default allocator.
         * 
         * @param[in] size How much memory to allocate.
         * 
         * @return @c NULL if out of memory; pointer to allocated space otherwise.
         * 
         * @sa sim_allocator_default_falloc
         * @sa sim_allocator_default_realloc
         * @sa sim_allocator_default_free
         */
        extern EXPORT void* C_CALL sim_allocator_default_malloc(
            size_t size
        );

        /**
         * @fn void* sim_allocator_default_falloc(2)
         * @relates Sim_IAllocator
         * @brief The default filled memory allocation function used by the default allocator.
         * 
         * @param[in] size How much memory to allocate.
         * @param[in] fill An unsigned byte to fill the allocated memory with.
         * 
         * @return @c NULL if out of memory; pointer to allocated space otherwise.
         * 
         * @sa sim_allocator_default_malloc
         * @sa sim_allocator_default_realloc
         * @sa sim_allocator_default_free
         */
        extern EXPORT void* C_CALL sim_allocator_default_falloc(
            size_t size,
            ubyte fill
        );

        /**
         * @fn void* sim_allocator_default_realloc(2)
         * @relates Sim_IAllocator
         * @brief The default memory reallocation function used by the default allocator.
         * 
         * @param[in] ptr  Pointer to malloc'd/falloc'd memory to reallocate.
         * @param[in] size Size request for reallocated memory.
         * 
         * @return @c NULL if out of memory; pointer to reallocated space otherwise.
         * 
         * @sa sim_allocator_default_malloc
         * @sa sim_allocator_default_falloc
         * @sa sim_allocator_default_free
         */
        extern EXPORT void* C_CALL sim_allocator_default_realloc(
            void* ptr,
            size_t size
        );

        /**
         * @fn void sim_allocator_default_free(1)
         * @relates Sim_IAllocator
         * @brief The default memory free function used by the default allocator.
         * 
         * @param[in] ptr Pointer to malloc'd/falloc'd/realloc'd memory to free.
         * 
         * @sa sim_allocator_default_malloc
         * @sa sim_allocator_default_falloc
         * @sa sim_allocator_default_realloc
         */
        extern EXPORT void  C_CALL sim_allocator_default_free(
            void* ptr
        );

        /**
         * @fn const Sim_IAllocator *const sim_allocator_get_default(0)
         * @relates Sim_IAllocator
         * @brief Retrieve the default allocator.
         * 
         * @return Pointer to the default allocator.
         * 
         * @sa sim_allocator_set_default
         */
        extern EXPORT const Sim_IAllocator *const C_CALL sim_allocator_get_default(void);
        
        /**
         * @fn sim_allocator_set_default(1)
         * @relates Sim_IAllocator
         * @brief Set the default allocator.
         * 
         * @param[in] allocator Pointer to an allocator to use as the default or @c NULL to
         *                      use the original default allocator.
         * 
         * @sa sim_allocator_get_default
         */
        extern EXPORT void C_CALL sim_allocator_set_default(
            Sim_IAllocator *const allocator
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */

        class EXPORT Allocator {
        private:
            C_API::Sim_IAllocator _c_allocator;
        
        public:
            virtual void* malloc(size_t size) = 0;
            virtual void* falloc(size_t size, ubyte fill = 0x00) = 0;
            virtual void* realloc(void* ptr, size_t size) = 0;
            virtual void  free(void* ptr) = 0;

            Allocator();
            virtual ~Allocator() = 0;

            C_API::Sim_IAllocator *const get_c_allocator() const {
                return &_c_allocator;
            }
        }
        
#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_ALLOCATOR_H_ */
