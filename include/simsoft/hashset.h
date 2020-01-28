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

#       ifndef SIM_HASH_DEFAULT_SIZE
#           define SIM_HASH_DEFAULT_SIZE 53
#       endif

        /**
         * @struct Sim_HashSet
         * @brief Generic unordered set type.
         * 
         * @tparam _item_properties Properties pertaining to the items stored in this hashset.
         * @tparam _allocator_ptr   Pointer to allocator used when resizing internal array.
         */
        typedef struct Sim_HashSet {
            const struct {
                Sim_TypeInfo type_info;                  // Type information
                Sim_HashFuncPtr hash_func_ptr;           // Pointer to hash function
                Sim_PredicateFuncPtr predicate_func_ptr; // Pointer to predicate function
            } _item_properties; // properties of hashset items
            const Sim_Allocator *const _allocator_ptr; // bucket allocator
            const size_t _initial_size; // the size of the hashset on initialization
            const size_t _base_size; // used for dynamically resizing
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
         * @brief Adds an item into a hashset.
         * 
         * @param[in,out] hashset_ptr  Pointer to a hashset to insert into.
         * @param[in]     new_item_ptr Pointer to a new item to add to the hashset.
         * 
         * @return @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr or @e new_item_ptr are @c NULL ;
         *         @b SIM_RC_ERR_OUTOFMEM if the hashset had to resize to accomodate the newly
         *                                inserted item and was unable to;
         *         @b SIM_RC_FAILURE      if *new_item_ptr is already contained in the hashset;
         *         @b SIM_RC_SUCCESS      otherwise.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_insert(
            Sim_HashSet *const hashset_ptr,
            const void*        new_item_ptr
        );

        /**
         * @fn Sim_ReturnCode sim_hashset_remove(2)
         * @brief Removes an item from a hashset.
         * 
         * @param[in,out] hashset_ptr     Pointer to a hashset to remove from.
         * @param[in]     remove_item_ptr Pointer to an item to remove from the hashset.
         * 
         * @return @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr or @e remove_item_ptr are @c NULL ;
         *         @b SIM_RC_ERR_OUTOFMEM if the hashset had to resize to save space and was
         *                                unable to;
         *         @b SIM_RC_FAILURE      if *remove_item_ptr was not contained in the hashset;
         *         @b SIM_RC_SUCCESS      otherwise.
         */
        extern SIM_API Sim_ReturnCode C_CALL sim_hashset_remove(
            Sim_HashSet *const hashset_ptr,
            const void *const  remove_item_ptr
        );

        /**
         * @fn Sim_ReturnCode sim_hashset_foreach(3)
         * @brief Applies a given function to each item in the hashset.
         * 
         * @param[in,out] hashset_ptr      Pointer to a hashset whose items will be iterated over.
         * @param[in]     foreach_func_ptr Pointer to a function that will be applied to each item
         *                                 in the hashset.
         * @param[in]     userdata         User-provided data for @e foreach_func_ptr
         * 
         * @return @b SIM_RC_ERR_NULLPTR if @e hashset_ptr or @e foreach_func_ptr are @c NULL ;
         *         @b SIM_RC_FAILURE     if @e foreach_func_ptr returns @c false during the loop;
         *         @b SIM_RC_SUCCESS     otherwise.
         */
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
