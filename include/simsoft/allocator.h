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
         * @typedef Sim_MemAllocFuncPtr
         * @headerfile allocator.h "simsoft/allocator.h"
         * @relates Sim_IAllocator
         * @breif Function pointer for dynamic memory allocation.
         * 
         * @param[in] size How much memory to allocate.
         * 
         * @return @c NULL if out of memory; pointer to allocated space otherwise.
         * 
         * @sa Sim_MemFilledAllocFuncPtr
         * @sa Sim_MemReallocFuncPtr
         * @sa Sim_MemFreeFuncPtr
         */
        typedef void* (*Sim_MemAllocFuncPtr)(
            size_t size
        );

        /**
         * @typedef Sim_MemFilledAllocFuncPtr
         * @headerfile allocator.h "simsoft/allocator.h"
         * @relates Sim_IAllocator
         * @breif Function pointer for filled dynamic memory allocation.
         * 
         * @param[in] size How much memory to allocate.
         * @param[in] fill An unsigned byte to fill the allocated memory with.
         * 
         * @return @c NULL if out of memory; pointer to allocated space otherwise. 
         * 
         * @sa Sim_MemAllocFuncPtr
         * @sa Sim_MemReallocFuncPtr
         * @sa Sim_MemFreeFuncPtr
         */
        typedef void* (*Sim_MemFilledAllocFuncPtr)(
            size_t size,
            ubyte fill
        );

        /**
         * @typedef Sim_MemReallocFuncPtr
         * @headerfile allocator.h "simsoft/allocator.h"
         * @relates Sim_IAllocator
         * @brief Function pointer for dynamic memory reallocation.
         * 
         * @param[in] ptr  Pointer to malloc'd/falloc'd memory to reallocate.
         * @param[in] size Size request for reallocated memory.
         * 
         * @return @c NULL if out of memory; pointer to reallocated space otherwise.
         * 
         * @sa Sim_MemAllocFuncPtr
         * @sa Sim_MemFilledAllocFuncPtr
         * @sa Sim_MemFreeFuncPtr
         */
        typedef void* (*Sim_MemReallocFuncPtr)(
            void* ptr,
            size_t size
        );

        /**
         * @typedef Sim_MemFreeFuncPtr
         * @headerfile allocator.h "simsoft/allocator.h"
         * @relates Sim_IAllocator
         * @breif Function pointer for freeing dynamically allocated memory.
         * 
         * @param[in] ptr Pointer to malloc'd/falloc'd/realloc'd memory to free.
         * 
         * @sa Sim_MemAllocFuncPtr
         * @sa Sim_MemReallocFuncPtr
         * @sa Sim_MemFreeFuncPtr
         */
        typedef void  (*Sim_MemFreeFuncPtr)(
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
            Sim_MemAllocFuncPtr       malloc;
            Sim_MemFilledAllocFuncPtr falloc;
            Sim_MemReallocFuncPtr     realloc;
            Sim_MemFreeFuncPtr        free;
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
         * @fn const Sim_IAllocator *const sim_get_default_allocator(0)
         * @relates Sim_IAllocator
         * @brief Retrieve the default allocator.
         * 
         * @return Pointer to the default allocator.
         * 
         * @sa sim_set_default_allocator
         */
        extern EXPORT const Sim_IAllocator *const C_CALL sim_get_default_allocator(void);
        
        /**
         * @fn sim_set_default_allocator(1)
         * @relates Sim_IAllocator
         * @brief Set the default allocator.
         * 
         * @param[in] allocator Pointer to an allocator to use as the default or @c NULL to
         *                      use the original default allocator.
         * 
         * @sa sim_get_default_allocator
         */
        extern EXPORT void C_CALL sim_set_default_allocator(
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
