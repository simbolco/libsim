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
         * @headerfile hashset.h "simsoft/hashset.h"
         * @brief Generic unordered set type.
         * 
         * @tparam _item_properties Properties pertaining to the items stored in the hashset.
         * @tparam _allocator_ptr   Pointer to allocator used to allocate buckets/nodes.
         * 
         * @var Sim_HashSet::count
         *     The number of items contained in the hashset.
         * @var Sim_HashSet::data_ptr
         *     Pointer to the internal hash table used by the hashset.
         * @var Sim_HashSet::_initial_size @private
         *     The starting size of the hash table used by the hashset.
         * @var Sim_HashSet::_base_size @private
         *     The base size passed into the hashset's constructor.
         * @var Sim_HashSet::_allocated @private
         *     The amount of allocated buckets in the hash table used by the hashset.
         */
        typedef struct Sim_HashSet {
            const struct {
                size_t size;                      // Item size

                Sim_HashProc hash_proc;           // Pointer to hash function
                Sim_PredicateProc predicate_proc; // Pointer to predicate function
            } _item_properties; // properties of hashset items
            const Sim_IAllocator *const _allocator_ptr; // bucket allocator
            const size_t _initial_size; // the size of the hashset on initialization
            const size_t _base_size; // used for dynamically resizing
            size_t _allocated;  // how many buckets have been allocated

            size_t count;   // amount of items stored in the hashset
            void* data_ptr; // pointer to hash buckets
        } Sim_HashSet;

        /**
         * @fn void sim_hashset_construct(
         *         Sim_HashSet *const,
         *         const size_t,
         *         Sim_HashProc,
         *         Sim_PredicateProc,
         *         const Sim_IAllocator*,
         *         const size_t
         *     )
         * @relates @capi{Sim_HashSet}
         * @brief Constructs a new hashset.
         * 
         * @param[in,out] hashset_ptr         Pointer to a hashset to construct.
         * @param[in]     item_size           Size of each item.
         * @param[in]     item_hash_proc      Item hash function.
         * @param[in]     item_predicate_proc Item equality predicate function.
         * @param[in]     allocator_ptr       Pointer to allocator to use when resizing hash
         *                                    buckets.
         * @param[in]     initial_size        The initial allocated size of the newly created
         *                                    hashset.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr or @e item_predicate_proc @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if hash buckets couldn't be allocated;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_hashset_destroy
         */
        extern EXPORT void C_CALL sim_hashset_construct(
            Sim_HashSet *const    hashset_ptr,
            const size_t          item_size,
            Sim_HashProc          item_hash_proc,
            Sim_PredicateProc     item_predicate_proc,
            const Sim_IAllocator* allocator_ptr,
            const size_t          initial_size
        );

        /**
         * @fn void sim_hashset_destroy(Sim_HashSet *const)
         * @relates @capi{Sim_HashSet}
         * @brief Destroys a hashset.
         * 
         * @param[in,out] hashset_ptr Pointer to a hashset to destroy.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e hashset_ptr is @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @sa sim_hashset_construct
         */
        extern EXPORT void C_CALL sim_hashset_destroy(
            Sim_HashSet *const hashset_ptr
        );

        /**
         * @fn bool sim_hashset_is_empty(Sim_HashSet *const)
         * @relates @capi{Sim_HashSet}
         * @brief Checks if a hashset is empty.
         * 
         * @param[in] hashset_ptr Pointer to a hashset to check.
         * 
         * @return @c true if the hashset is empty; @c false otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e hashset_ptr is @c NULL;
         *     @b SIM_RC_SUCCESS otherwise.
         */
        extern EXPORT bool C_CALL sim_hashset_is_empty(
            Sim_HashSet *const hashset_ptr
        );
        
        /**
         * @fn void sim_hashset_clear(Sim_HashSet *const)
         * @relates @capi{Sim_HashSet}
         * @brief Clears a hashset of all its contents.
         * 
         * @param[in,out] hashset_ptr Pointer to hashset to empty.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr is @c NULL ;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void C_CALL sim_hashset_clear(
            Sim_HashSet *const hashset_ptr
        );
        
        /**
         * @fn bool sim_hashset_contains(Sim_HashSet *const, const void *const)
         * @relates @capi{Sim_HashSet}
         * @brief Checks if an item is contained in a hashset.
         * 
         * @param[in,out] hashset_ptr Pointer to hashset to search.
         * @param[in]     item_ptr    Pointer to item to compare against.
         * 
         * @return @c false on error (see remarks) or if item isn't contained in the hashset;
         *         @c true otherwise.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e hashset_ptr or @e item_ptr are @c NULL ;
         *     @b SIM_RC_NOT_FOUND   if @e item_ptr isn't contained in the hashset;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT bool C_CALL sim_hashset_contains(
            Sim_HashSet *const hashset_ptr,
            const void *const  item_ptr
        );

        /**
         * @fn void sim_hashset_resize(Sim_HashSet *const, const size_t)
         * @relates @capi{Sim_HashSet}
         * @brief Resizes a hashset to a given size.
         * 
         * @param[in,out] hashset_ptr Pointer to a hashset to resize.
         * @param[in]     new_size    The new size of the hashset.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr is @c NULL ;
         *     @b SIM_RC_ERR_INVALARG if @e new_size < @e hashset_ptr->count ;
         *     @b SIM_RC_ERR_OUTOFMEM if the hashset couldn't be resized;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void C_CALL sim_hashset_resize(
            Sim_HashSet *const hashset_ptr,
            const size_t       new_size
        );

        /**
         * @fn void sim_hashset_insert(Sim_HashSet *const, const void*)
         * @relates @capi{Sim_HashSet}
         * @brief Adds an item into a hashset.
         * 
         * @param[in,out] hashset_ptr  Pointer to a hashset to insert into.
         * @param[in]     new_item_ptr Pointer to a new item to add to the hashset.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr or @e new_item_ptr are @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if the hashset had to resize to accomodate the newly
         *                            inserted item and was unable to;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void C_CALL sim_hashset_insert(
            Sim_HashSet *const hashset_ptr,
            const void*        new_item_ptr
        );

        /**
         * @fn void sim_hashset_remove(Sim_HashSet *const, const void *const)
         * @relates @capi{Sim_HashSet}
         * @brief Removes an item from a hashset.
         * 
         * @param[in,out] hashset_ptr     Pointer to a hashset to remove from.
         * @param[in]     remove_item_ptr Pointer to an item to remove from the hashset.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e hashset_ptr or @e remove_item_ptr are @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if the hashset had to resize to save space and was
         *                            unable to;
         *     @b SIM_RC_FAILURE     if @c *remove_item_ptr was not contained in the hashset;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void C_CALL sim_hashset_remove(
            Sim_HashSet *const hashset_ptr,
            const void *const  remove_item_ptr
        );

        /**
         * @fn bool sim_hashset_foreach(Sim_HashSet *const, Sim_ConstForEachProc, Sim_Variant)
         * @relates @capi{Sim_HashSet}
         * @brief Applies a given function to each item in the hashset.
         * 
         * @param[in,out] hashset_ptr  Pointer to a hashset whose items will be iterated over.
         * @param[in]     foreach_proc Pointer to a function that will be applied to each item in
         *                             the hashset.
         * @param[in]     userdata     User-provided data for @e foreach_proc.
         * 
         * @return @c false on error (see remarks) or if the loop wasn't fully completed;
         *         @c true  otherwise.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e hashset_ptr or @e foreach_proc are @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT bool C_CALL sim_hashset_foreach(
            Sim_HashSet *const   hashset_ptr,
            Sim_ConstForEachProc foreach_proc,
            Sim_Variant          userdata
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
#       include "./exception.hpp"
        
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
