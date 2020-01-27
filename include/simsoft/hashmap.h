/**
 * @file hashmap.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for hashmaps
 * @version 0.1
 * @date 2020-01-10
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_HASHMAP_H_
#define SIMSOFT_HASHMAP_H_

#include "./common.h"
#include "./allocator.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

#       ifndef SIM_HASH_STARTING_SIZE
#           define SIM_HASH_STARTING_SIZE 53
#       endif

        /**
         * @struct Sim_HashMap
         * @brief Generic unordered key-value pair container / associative array type.
         * 
         * @tparam _key_properties Properties pertaining to the keys stored in this hashmap.
         * @tparam _value_size     Size in bytes of the values stored in this hashmap.
         * @tparam _allocator_ptr  Pointer to allocator used to allocate buckets/nodes.
         */
        typedef struct Sim_HashMap {
            const struct {
                Sim_TypeInfo type_info;
                Sim_HashFuncPtr hash_func_ptr;           // Pointer to hash function
                Sim_PredicateFuncPtr predicate_func_ptr; // Pointer to predicate function
            } _key_properties;  // properties of hashmap keys
            const Sim_Allocator *const _allocator_ptr; // bucket allocator
            const size_t _initial_size;
            const size_t _base_size; // base size used for initialization
            size_t _allocated; // how many buckets have been allocated
            const size_t _deleted_item; // dedicated location for deleted key-value pairs

            size_t count;   // amount of items stored in the hashmap
            void* data_ptr; // pointer to hash buckets

            size_t _value_size; // size of hashmap values
        } Sim_HashMap;

        typedef struct Sim_MapConstKeyValuePair {
            const void *const key;
            void *const value;
        } Sim_MapConstKeyValuePair;

        typedef bool (*Sim_MapForEachFuncPtr)(
            Sim_MapConstKeyValuePair *const key_value_pair_ptr,
            Sim_Variant userdata,
            const size_t index
        );

        /**
         * @fn Sim_ReturnCode sim_hashmap_initialize(8)
         * @brief Initializes a new hashmap.
         * 
         * @param[in,out] hashmap_ptr            Pointer to a hashmap to initialize.
         * @param[in]     key_size               Size of hashmap keys.
         * @param[in]     key_type               Type of the keys stored in the hashmap.
         * @param[in]     key_hash_func_ptr      Key hash function.
         * @param[in]     key_predicate_func_ptr Key predicate function.
         * @param[in]     value_size             Size of each item.
         * @param[in]     allocator_ptr          Pointer to allocator to use when resizing hash
         *                                       buckets.
         * @param[in]     initial_size           The initial allocated size of the newly created
         *                                       hashmap.
         * 
         * @return @b SIM_RC_ERR_NULLPTR  if @e hashmap_ptr is @c NULL ;
         *         @b SIM_RC_ERR_OUTOFMEM if hash buckets couldn't be allocated;
         *         @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_hashmap_create
         * @sa sim_hashmap_destroy
         * @sa sim_hashmap_free
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_initialize(
            Sim_HashMap*         hashmap_ptr,
            const size_t         key_size,
            const Sim_DataType   key_type,
            Sim_HashFuncPtr      key_hash_func_ptr,
            Sim_PredicateFuncPtr key_predicate_func_ptr,
            const size_t         value_size,
            const Sim_Allocator* allocator_ptr,
            const size_t         initial_size
        );

        /**
         * @fn Sim_HashMap* sim_hashmap_create(7)
         * @brief Creates a new hashmap on the heap.
         * 
         * @param[in]     key_size               Size of hashmap keys.
         * @param[in]     key_type               Type of the keys stored in the hashmap.
         * @param[in]     key_hash_func_ptr      Key hash function.
         * @param[in]     key_predicate_func_ptr Key predicate function.
         * @param[in]     value_size             Size of each item.
         * @param[in]     allocator_ptr          Pointer to allocator to use when resizing hash
         *                                       buckets.
         * @param[in]     initial_size           The initial allocated size of the newly created
         *                                       hashmap.
         * 
         * @return Pointer to the new hashmap, @c NULL on error.
         * 
         * @sa sim_hashmap_initialize
         * @sa sim_hashmap_destroy
         * @sa sim_hashmap_free
         */
        extern SIM_API Sim_HashMap* C_CALL sim_hashmap_create(
            const size_t         key_size,
            const Sim_DataType   key_type,
            Sim_HashFuncPtr      key_hash_func_ptr,
            Sim_PredicateFuncPtr key_predicate_func_ptr,
            const size_t         value_size,
            const Sim_Allocator* allocator_ptr,
            const size_t         initial_size
        );

        /**
         * @fn Sim_ReturnCode sim_hashmap_destroy(1)
         * @brief Destroys an initialized hashmap.
         * 
         * @param[in,out] hashmap_ptr Pointer to a hashmap to destroy.
         * 
         * @return @b SIM_RC_ERR_NULLPTR if @e hashmap_ptr is @c NULL ;
         *         @b SIM_RC_SUCCESS     otherwise.
         * 
         * @sa sim_hashmap_initialize
         * @sa sim_hashmap_create
         * @sa sim_hashmap_free
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_destroy(
            Sim_HashMap* hashmap_ptr
        );

        /**
         * @fn Sim_ReturnCode sim_hashmap_free(1)
         * @brief Frees & destroys a heap-allocated hashmap.
         * 
         * @param[in,out] hashmap_ptr Pointer to a hashmap to free/destroy.
         * 
         * @return @b SIM_RC_ERR_NULLPTR if @e hashmap_ptr is @c NULL ;
         *         @b SIM_RC_SUCCESS     otherwise.
         * 
         * @sa sim_hashmap_initialize
         * @sa sim_hashmap_create
         * @sa sim_hashmap_destroy
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_free(
            Sim_HashMap* hashmap_ptr
        );
        
        /**
         * @def bool sim_hashmap_is_empty(1)
         * @brief Checks if the hashmap is empty.
         * 
         * @param[in] hashmap_ptr Pointer to a hashmap to check.
         * 
         * @return @c true if the hashmap is empty; @c false otherwise or if the
         *         hashmap is @c NULL .
         */
#       define sim_hashmap_is_empty(hashmap_ptr) \
            (hashmap_ptr ? hashmap_ptr->count == 0 : false)

        /**
         * @fn Sim_ReturnCode sim_hashmap_clear(1)
         * @brief Clears a hashmap of all its contents.
         * 
         * @param[in,out] hashmap_ptr Pointer to hashmap to empty.
         * 
         * @return @b SIM_RC_ERR_NULLPTR  if @e hashmap_ptr is @c NULL ;
         *         @b SIM_RC_SUCCESS      otherwise.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_clear(
            Sim_HashMap *const hashmap_ptr
        );

        /**
         * @fn bool sim_hashmap_contains_key(2)
         * @brief Checks if a key is contained in the hashmap.
         * 
         * @param[in,out] hashmap_ptr         Pointer to hashmap to search.
         * @param[in]     key_ptr             Pointer to key to compare against.
         * 
         * @return @c true if the key is contained within @e hashmap_ptr;
         *         @c false otherwise or if @e hashmap_ptr is @c NULL .
         */
        extern SIM_API bool C_CALL sim_hashmap_contains(
            Sim_HashMap *const hashmap_ptr,
            const void *const key_ptr
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_resize(
            Sim_HashMap *const hashmap_ptr,
            size_t new_size
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_get(
            Sim_HashMap *const hashmap_ptr,
            const void*        key_ptr,
            void*              out_value_ptr
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_get_ptr(
            Sim_HashMap *const hashmap_ptr,
            const void*        key_ptr,
            void* *const       out_value_ptr
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_insert(
            Sim_HashMap *const hashmap_ptr,
            const void* new_key_ptr,
            const void* value_ptr
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_remove(
            Sim_HashMap *const hashmap_ptr,
            const void *const remove_key_ptr
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_hashmap_foreach(
            Sim_HashMap *const    hashmap_ptr,
            Sim_MapForEachFuncPtr foreach_func_ptr,
            Sim_Variant           userdata
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
#       include "./exception.hpp"
        
        template <
            class K,
            class V,
            class _Hash,
            class _Pred,
            class _Alloc = Allocator
        >
        class HashMap {
        private:
            C_API::Sim_HashMap _c_hashmap;
        };

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_HASHMAP_H_ */
