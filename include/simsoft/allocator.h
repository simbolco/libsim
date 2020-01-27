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

        typedef void *Sim_MemAllocFunction(size_t size);
        typedef void *Sim_FilledAllocFunction(size_t size, ubyte fill);
        typedef void *Sim_ReallocFunction(void* ptr, size_t size);
        typedef void  Sim_FreeFunction(void* ptr);

        /**
         * @struct Sim_Allocator
         * @brief Interface for a memory allocator.
         */
        typedef struct Sim_Allocator {
            Sim_MemAllocFunction    *const malloc;
            Sim_FilledAllocFunction *const falloc;
            Sim_ReallocFunction     *const realloc;
            Sim_FreeFunction        *const free;
        } Sim_Allocator;

        /**
         * @brief The default memory allocation function used by the default allocator.
         * 
         * @param[in] size How much memory to allocate.
         * 
         * @return @c NULL if out of memory; pointer to allocated space otherwise.
         * 
         * 
         * @sa sim_allocator_default_falloc
         * @sa sim_allocator_default_realloc
         * @sa sim_allocator_default_free
         */
        extern SIM_API void* C_CALL sim_allocator_default_malloc(
            size_t size
        );

        /**
         * @brief The default filled memory allocation function used by the default allocator.
         * 
         * @param[in] size How much memory to allocate.
         * @param[in] fill An unsigned byte to fill the allocated memory with.
         * 
         * @return @c NULL if out of memory; pointer to allocated space otherwise.
         * 
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
         * @brief Retrieve the default allocator.
         * 
         * @return Pointer to the default allocator.
         * 
         * @sa sim_set_default_allocator
         */
        extern SIM_API const Sim_Allocator *const C_CALL sim_get_default_allocator(void);
        
        /**
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
