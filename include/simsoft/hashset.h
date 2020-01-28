/**
 * @file hashset.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for hashsets
 * @version 0.1
 * @date 2020-01-10
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_HASHSET_H_
#define SIMSOFT_HASHSET_H_

#include "./common.h"
#include "./allocator.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

#       ifndef SIM_HASH_STARTING_SIZE
#           define SIM_HASH_STARTING_SIZE 53
#       endif

        /**
         * @struct Sim_HashSet
         * @brief Generic unordered set type.
         * 
         */
        typedef struct Sim_HashSet {
            const struct {
                Sim_TypeInfo type_info;
                Sim_HashFuncPtr hash_func_ptr;           // Pointer to hash function
                Sim_PredicateFuncPtr predicate_func_ptr; // Pointer to predicate function
            } _item_properties;  // properties of hashset items
            const Sim_Allocator *const _allocator_ptr; // bucket allocator
            const size_t _initial_size;
            const size_t _base_size; // base size used for initialization
            size_t _allocated;  // how many buckets have been allocated
            const size_t _deleted_item; // dedicated location for deleted key-value pairs

            size_t count;   // amount of items stored in the hashset
            void* data_ptr; // pointer to hash buckets
        } Sim_HashSet;

        /**
         * @fn Sim_ReturnCode sim_hashset_initialize(7)
         * @brief Initializes a new hashset.
         * 
         * @param[in,out] hashset_ptr        Pointer to a hashset to initialize.
         * @param[in]     item_size          Size of each item.
         * @param[in]     item_type          Type of the items stored in the hashset.
         * @param[in]     item_hash_func_ptr Item hash function.
         * @param[in]     item_hash_userdata User-provided data for item hash function.
         * @param[in]     allocator_ptr      Pointer to allocator to use when resizing hash 
         *                                   buckets.
         * @param[in]     initial_size       The initial allocated size of the newly created
         *                                   hashset.
         * 
         * @return @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr is @c NULL ;
         *         @b SIM_RC_ERR_OUTOFMEM if hash buckets couldn't be allocated;
         *         @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_hashset_destroy
         * @sa sim_hashset_create
         * @sa sim_hashset_free
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_initialize(
            Sim_HashSet*         hashset_ptr,
            const size_t         item_size,
            const Sim_DataType   item_type,
            Sim_HashFuncPtr      item_hash_func_ptr,
            Sim_PredicateFuncPtr item_predicate_func_ptr,
            const Sim_Allocator* allocator_ptr,
            const size_t         initial_size
        );

        extern SIM_API Sim_HashSet* C_CALL sim_hashset_create(
            const size_t         item_size,
            const Sim_DataType   item_type,
            Sim_HashFuncPtr      item_hash_func_ptr,
            Sim_PredicateFuncPtr item_predicate_func_ptr,
            const Sim_Allocator* allocator_ptr,
            const size_t         initial_size
        );

        /**
         * @fn Sim_ReturnCode sim_hashset_destroy(1)
         * @brief Destroys an initialized hashset.
         * 
         * @param[in,out] hashset_ptr Pointer to a hashset to destroy.
         * 
         * @return @b SIM_RC_ERR_NULLPTR if @e hashset_ptr is @c NULL ;
         *         @b SIM_RC_SUCCESS     otherwise.
         * 
         * @sa sim_hashset_initialize
         * @sa sim_hashset_create
         * @sa sim_hashset_free
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_destroy(
            Sim_HashSet* hashset_ptr
        );

        /**
         * @fn Sim_ReturnCode sim_hashset_free(1)
         * @brief Frees & destroys a heap-allocated hashset.
         * 
         * @param[in,out] hashset_ptr Pointer to a hashset to free/destroy.
         * 
         * @return @b SIM_RC_ERR_NULLPTR if @e hashset_ptr is @c NULL ;
         *         @b SIM_RC_SUCCESS     otherwise.
         * 
         * @sa sim_hashset_initialize
         * @sa sim_hashset_create
         * @sa sim_hashset_destroy
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_free(
            Sim_HashSet* hashset_ptr
        );

        /**
         * @def bool sim_hashset_is_empty(1)
         * @brief Checks if the hashset is empty.
         * 
         * @param[in] hashset_ptr Pointer to a hashset to check.
         * 
         * @return @c true if the hashset is empty; @c false otherwise or if the
         *         hashset is @c NULL .
         */
#       define sim_hashset_is_empty(hashset_ptr) \
            (hashset_ptr ? hashset_ptr->count == 0 : false)
        
        /**
         * @fn Sim_ReturnCode sim_hashset_clear(1)
         * @brief Clears a hashset of all its contents.
         * 
         * @param[in,out] hashset_ptr Pointer to hashset to empty.
         * 
         * @return @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr is @c NULL ;
         *         @b SIM_RC_SUCCESS      otherwise.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_clear(
            Sim_HashSet *const hashset_ptr
        );
        
        /**
         * @fn bool sim_hashset_contains(2)
         * @brief Checks if an item is contained in the hashset.
         * 
         * @param[in,out] hashset_ptr Pointer to hashset to search.
         * @param[in]     item_ptr    Pointer to item to compare against.
         * 
         * @return @c true if the item is contained within @e hashset_ptr;
         *         @c false otherwise or if @e hashset_ptr is @c NULL .
         */
        extern SIM_API bool C_CALL sim_hashset_contains(
            Sim_HashSet *const hashset_ptr,
            const void *const  item_ptr
        );

        /**
         * @fn Sim_ReturnCode sim_hashset_resize(2)
         * @brief Resizes the hashset to a new size.
         * 
         * @param[in,out] hashset_ptr Pointer to a hashset to resize.
         * @param[in]     new_size    The new size of the hashset.
         * 
         * @return @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr is @c NULL ;
         *         @b SIM_RC_ERR_OUTOFMEM if the hashset cannot be resized;
         *         @b SIM_RC_SUCCESS      otherwise.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_resize(
            Sim_HashSet *const hashset_ptr,
            const size_t       new_size
        );

        /**
         * @fn Sim_ReturnCode sim_hashset_insert(2)
         * @brief 
         * 
         * @param[in,out] hashset_ptr
         * @param[in]     new_item_ptr
         * 
         * @return 
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_insert(
            Sim_HashSet *const hashset_ptr,
            const void*        new_item_ptr
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_remove(
            Sim_HashSet *const hashset_ptr,
            const void *const  remove_item_ptr
        );

        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_foreach(
            Sim_HashSet *const      hashset_ptr,
            Sim_ConstForEachFuncPtr foreach_func_ptr,
            Sim_Variant             userdata
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
#       include "./exception.hh"
        
        template <
            class T,
            class _Hash,
            class _Alloc = Allocator
        >
        class HashSet {
        private:
            C_API::Sim_HashSet _c_hashset;
        };

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_HASHSET_H_ */
