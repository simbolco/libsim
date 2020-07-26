/**
 * @file hashmap.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for hashmaps
 * @version 0.2
 * @date 2020-07-25
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_HASHMAP_H_
#define SIMSOFT_HASHMAP_H_

#include "allocator.h"
#include "macro.h"
#include "typedefs.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API

#       ifndef SIM_HASH_DEFAULT_SIZE
#           define SIM_HASH_DEFAULT_SIZE 53
#       endif

        /**
         * @brief Generic unordered key-value pair container / associative array type.
         * 
         * @tparam _key_properties Properties pertaining to the keys stored in the hashmap.
         * @tparam _value_size     Size in bytes of the values stored in the hashmap.
         * @tparam _allocator_ptr  Pointer to allocator used to allocate buckets/nodes.
         * 
         * @var Sim_HashMap::_initial_size @private
         *     The starting size of the hash table used by the hashmap.
         * @var Sim_HashMap::_base_size @private
         *     The base size passed into the hashmap's constructor.
         * @var Sim_HashMap::_allocated @private
         *     The amount of allocated buckets in the hash table used by the hashmap.
         * @var Sim_HashMap::_value_size @private
         *     The size of values contained in the hashmap in bytes.
         */
        typedef struct Sim_HashMap {
            const struct {
                size_t size;                      // Key size

                Sim_HashProc hash_proc;           // Pointer to hash function
                Sim_PredicateProc predicate_proc; // Pointer to predicate function
            } _key_properties;  // properties of hashmap keys
            Sim_IAllocator *const _allocator_ptr; // bucket allocator
            const size_t _initial_size;
            const size_t _base_size; // base size used for initialization
            size_t _allocated; // how many buckets have been allocated

            size_t count;   ///< The number of key-value pairs contained in the hashmap.
            void* data_ptr; ///< Pointer to the internal hash table used by the hashmap.

            size_t _value_size; // size of hashmap values
        } Sim_HashMap;

#       ifndef SIM_DEFINED_MAP_FOREACH_STRUCTS
#           define SIM_DEFINED_MAP_FOREACH_STRUCTS
            /**
             * @brief Function pointer used when iterating over a map.
             * 
             * @param[in] key_value_pair_ptr Pointer to a key-value pair in a map.
             * @param[in] index              The pair's index in the map it's contained in.
             * @param[in] userdata           User-provided callback data.
             * 
             * @returns @c false to break out of the calling foreach loop;
             *          @c true  to continue iterating.
             */
            typedef bool (*Sim_MapForEachProc)(
                const void *const const_key_ptr,
                void *const       value_ptr,
                const size_t      index,
                Sim_Variant       userdata
            );
#       endif // SIM_DEFINED_MAP_FOREACH_STRUCTS

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Constructs a new hashmap.
         * 
         * @param[in,out] hashmap_ptr        Pointer to a hashmap to construct.
         * @param[in]     key_size           Size of hashmap keys.
         * @param[in]     key_hash_proc      Key hash function.
         * @param[in]     key_predicate_proc Key equality predicate function.
         * @param[in]     value_size         Size of each item.
         * @param[in]     allocator_ptr      Pointer to allocator to use when resizing hash
         *                                   buckets.
         * @param[in]     initial_size       The initial allocated size of the newly created
         *                                   hashmap.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr or @e key_predicate_proc are @c NULL;
         * @throw SIM_ERR_OUTOFMEM if hash buckets couldn't be allocated.
         * 
         * @sa sim_hashmap_destroy
         */
        DYNAPI_PROC(void, sim_hashmap_construct,,
            Sim_HashMap *const  hashmap_ptr,
            const size_t        key_size,
            Sim_HashProc        key_hash_proc,
            Sim_PredicateProc   key_predicate_proc,
            const size_t        value_size,
            Sim_IAllocator*     allocator_ptr,
            const size_t        initial_size
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Destroys a hashmap.
         * 
         * @param[in,out] hashmap_ptr Pointer to a hashmap to destroy.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr is @c NULL.
         * 
         * @sa sim_hashmap_construct
         */
        DYNAPI_PROC(void, sim_hashmap_destroy,,
            Sim_HashMap *const hashmap_ptr
        );
        
        /**
         * @relates @capi{Sim_HashMap}
         * @brief Checks if the hashmap is empty.
         * 
         * @param[in] hashmap_ptr Pointer to a hashmap to check.
         * 
         * @returns @c true if the hashmap is empty.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr is @c NULL.
         */
        DYNAPI_PROC(bool, sim_hashmap_is_empty,,
            Sim_HashMap *const hashmap_ptr
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Clears a hashmap of all its contents.
         * 
         * @param[in,out] hashmap_ptr Pointer to hashmap to empty.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr is @c NULL.
         */
        DYNAPI_PROC(void, sim_hashmap_clear,,
            Sim_HashMap *const hashmap_ptr
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Checks if a key is contained in the hashmap.
         * 
         * @param[in,out] hashmap_ptr         Pointer to hashmap to search.
         * @param[in]     key_ptr             Pointer to key to compare against.
         * 
         * @returns @c false if the key isn't contained in the hashmap; @c true otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr or @e key_ptr are @c NULL.
         */
        DYNAPI_PROC(bool, sim_hashmap_contains_key,,
            Sim_HashMap *const hashmap_ptr,
            const void *const  key_ptr
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Resizes the hashmap to a new size.
         * 
         * @param[in,out] hashmap_ptr Pointer to a hashset to resize.
         * @param[in]     new_size    The new size of the hashset.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr is @c NULL;
         * @throw SIM_ERR_INVALARG if @e new_size < @c hashmap_ptr->count;
         * @throw SIM_ERR_OUTOFMEM if the hashmap couldn't be resized.
         */
        DYNAPI_PROC(void, sim_hashmap_resize,,
            Sim_HashMap *const hashmap_ptr,
            size_t             new_size
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Get a value from the hashmap via a particular key.
         * 
         * @param[in,out] hashmap_ptr   Pointer to a hashmap to retrieve a value from.
         * @param[in]     key_ptr       Pointer to lookup key.
         * @param[out]    out_value_ptr Pointer to be filled with the associated value.
         * 
         * @returns @c false if the key isn't contained in the hashmap; @c true otherwise.
         * 
         * @throw SIM_ERR_NULLPTR if @e hashmap_ptr, @e key_ptr, or @e out_value_ptr are @c NULL.
         */
        DYNAPI_PROC(bool, sim_hashmap_get,,
            Sim_HashMap *const hashmap_ptr,
            const void*        key_ptr,
            void*              out_value_ptr
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Get pointer to value in the hashmap via a particular key.
         * 
         * @param[in,out] hashmap_ptr   Pointer to a hashmap to retrieve a value from.
         * @param[in]     key_ptr       Pointer to lookup key.
         * 
         * @returns @c NULL if the key isn't contained in the hashmap; pointer to a value in the
         *          hashmap otherwise.
         * 
         * @throw SIM_ERR_NULLPTR if @e hashmap_ptr or @e key_ptr are @c NULL.
         */
        DYNAPI_PROC(void*, sim_hashmap_get_ptr,,
            Sim_HashMap *const hashmap_ptr,
            const void*        key_ptr
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Inserts a key-value pair into the hashmap or overwrites a pre-existing pair if
         *        the value is already in the hashmap.
         * 
         * @param[in,out] hashmap_ptr Pointer to a hashmap to insert into.
         * @param[in]     new_key_ptr Pointer to a new key to add to the hashmap.
         * @param[in]     value_ptr   Pointer to a value to associate with the key.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr, @e new_key_ptr, or @e value_ptr are @c NULL;
         * @throw SIM_ERR_OUTOFMEM if the hashmap had to resize to accomodate the newly inserted
         *                         and was unable to.
         */
        DYNAPI_PROC(void, sim_hashmap_insert,,
            Sim_HashMap *const hashmap_ptr,
            const void*        new_key_ptr,
            const void*        value_ptr
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Removes a key-value pair from the hashmap via a key.
         * 
         * @param[in,out] hashmap_ptr    Pointer to a hashmap to remove from.
         * @param[in]     remove_key_ptr Pointer to a key to remove from the hashset.
         * 
         * @returns @c false if the key isn't contained in the hashmap; @c true otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr or @e remove_key_ptr are @c NULL;
         * @throw SIM_ERR_OUTOFMEM if the hashmap had to resize to save space and was unable to.
         */
        DYNAPI_PROC(bool, sim_hashmap_remove,,
            Sim_HashMap *const hashmap_ptr,
            const void *const  remove_key_ptr
        );

        /**
         * @relates @capi{Sim_HashMap}
         * @brief Applies a given function to each key-value pair in the hashmap.
         * 
         * @param[in,out] hashmap_ptr  Pointer to a hashmap whose key-value pairs will be iterated
         *                             over.
         * @param[in]     foreach_proc Pointer to a function that will be applied to each pair in
         *                             the hashmap.
         * @param[in]     foreach_data User-provided data for @e foreach_proc.
         * 
         * @returns @c false if the loop wasn't fully completed; @c true otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e hashmap_ptr or @e foreach_proc are @c NULL.
         */
        DYNAPI_PROC(bool, sim_hashmap_foreach,,
            Sim_HashMap *const hashmap_ptr,
            Sim_MapForEachProc foreach_proc,
            Sim_Variant        foreach_data
        );
    
    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus // start C++ API
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

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft) // end SimSoft namespace

#endif // SIMSOFT_HASHMAP_H_
