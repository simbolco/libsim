/**
 * @file allocator.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for allocators
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
         * @relates Sim_Allocator
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
         * @relates Sim_Allocator
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
         * @relates Sim_Allocator
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
         * @relates Sim_Allocator
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
         * @struct Sim_Allocator
         * @headerfile allocator.h "simsoft/allocator.h"
         * @brief Interface for a memory allocator.
         * 
         * @var malloc  Memory allocation function.
         * @var falloc  Filled memory allocation function.
         * @var realloc Memory reallocation function.
         * @var free    Allocated memory free function.
         */
        typedef struct Sim_Allocator {
            Sim_MemAllocFuncPtr       malloc;
            Sim_MemFilledAllocFuncPtr falloc;
            Sim_MemReallocFuncPtr     realloc;
            Sim_MemFreeFuncPtr        free;
        } Sim_Allocator;

        /**
         * @fn void* sim_allocator_default_malloc(1)
         * @relates Sim_Allocator
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
        extern SIM_API void* C_CALL sim_allocator_default_malloc(
            size_t size
        );

        /**
         * @fn void* sim_allocator_default_falloc(2)
         * @relates Sim_Allocator
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
        extern SIM_API void* C_CALL sim_allocator_default_falloc(
            size_t size,
            ubyte fill
        );

        /**
         * @fn void* sim_allocator_default_realloc(2)
         * @relates Sim_Allocator
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
        extern SIM_API void* C_CALL sim_allocator_default_realloc(
            void* ptr,
            size_t size
        );

        /**
         * @fn void sim_allocator_default_free(1)
         * @relates Sim_Allocator
         * @brief The default memory free function used by the default allocator.
         * 
         * @param[in] ptr Pointer to malloc'd/falloc'd/realloc'd memory to free.
         * 
         * @sa sim_allocator_default_malloc
         * @sa sim_allocator_default_falloc
         * @sa sim_allocator_default_realloc
         */
        extern SIM_API void  C_CALL sim_allocator_default_free(
            void* ptr
        );

        /**
         * @fn const Sim_Allocator *const sim_get_default_allocator(0)
         * @relates Sim_Allocator
         * @brief Retrieve the default allocator.
         * 
         * @return Pointer to the default allocator.
         * 
         * @sa sim_set_default_allocator
         */
        extern SIM_API const Sim_Allocator *const C_CALL sim_get_default_allocator(void);
        
        /**
         * @fn sim_set_default_allocator(1)
         * @relates Sim_Allocator
         * @brief Set the default allocator.
         * 
         * @param[in] allocator Pointer to an allocator to use as the default or @c NULL to
         *                      use the original default allocator.
         * 
         * @sa sim_get_default_allocator
         */
        extern SIM_API void C_CALL sim_set_default_allocator(
            Sim_Allocator *const allocator
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */

        class Allocator {
        protected:
            C_API::Sim_Allocator _c_allocator;
        
        public:
            Allocator(const C_API::Sim_Allocator *const c_allocator_ptr) {
                memcpy(&c_allocator, c_allocator_ptr, sizeof(C_API::Sim_Allocator));
            }
            Allocator() : Allocator(sim_get_default_allocator()) {}

            ~Allocator() {}

            C_API::Sim_Allocator *const get_c_allocator() const {
                return &_c_allocator;
            }

            template <class T>
            T* malloc() final {
                return _c_allocator.malloc(sizeof(T));
            }
            template <>
            void* malloc(size_t size) final {
                return _c_allocator.malloc(size);
            }

            template <class T>
            T* falloc(ubyte fill = 0) final {
                return _c_allocator.falloc(sizeof(T), fill);
            }
            template <>
            void* falloc(size_t size, ubyte fill = 0) final {
                return _c_allocator.falloc(size, fill);
            }

            void* realloc(void* ptr, size_t size) final {
                return _c_allocator.realloc(ptr, size);
            }

            void free(void* ptr) final {
                return _c_allocator.free(ptr);
            }
        }
        
#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_ALLOCATOR_H_ */
