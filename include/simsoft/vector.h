/**
 * @file vector.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for vectors/array list containers
 * @version 0.2
 * @date 2020-07-25
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_VECTOR_H_
#define SIMSOFT_VECTOR_H_

#include "allocator.h"
#include "macro.h"
#include "typedefs.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API

        /**
         * @def SIM_DEFAULT_VECTOR_SIZE
         * @brief The default starting size of vector types.
         */
#       ifndef SIM_DEFAULT_VECTOR_SIZE
#           define SIM_DEFAULT_VECTOR_SIZE 32
#       endif
        
        /**
         * @ingroup containers
         * @brief Generic dynamic array container type.
         * 
         * @tparam _item_size     How large the items contained in the vector are.
         * @tparam _allocator_ptr Pointer to allocator used when resizing internal array.
         */
        typedef struct Sim_Vector {
            const size_t _item_size;
            Sim_IAllocator *const _allocator_ptr;
            size_t _allocated; // how much has been allocated

            size_t count;    ///< The number of items contained in the vector.
            void*  data_ptr; ///< Pointer to the internal array used by the vector.
        } Sim_Vector;

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Constructs a new vector.
         * 
         * @param[in,out] vector_ptr    Pointer to a vector to construct.
         * @param[in]     item_size     Size of each item.
         * @param[in]     allocator_ptr Pointer to allocator to use when resizing internal array.
         * @param[in]     initial_size  The initial size of the newly created vector.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFMEM if the vector size requested couldn't be allocated.
         * 
         * @sa sim_vector_destroy
         */
        DYNAPI_PROC(void, sim_vector_construct,,
            Sim_Vector *const vector_ptr,
            const size_t      item_size,
            Sim_IAllocator*   allocator_ptr,
            size_t            initial_size
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Destroys a vector.
         * 
         * @param[in] vector_ptr Pointer to vector to destroy.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL.
         * 
         * @sa sim_vector_construct
         */
        DYNAPI_PROC(void, sim_vector_destroy,,
            Sim_Vector *const vector_ptr
        );
        
        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Checks if a vector is empty.
         * 
         * @param[in] vector_ptr Pointer to a vector to check.
         * 
         * @returns @c true if the vector is empty.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL.
         */
        DYNAPI_PROC(bool, sim_vector_is_empty,,
            Sim_Vector *const vector_ptr
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Clears a vector of all its contents.
         * 
         * @param[in,out] vector_ptr Pointer to vector to empty.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL.
         */
        DYNAPI_PROC(void, sim_vector_clear,,
            Sim_Vector *const vector_ptr
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Resizes a vector to a given size.
         * 
         * @param[in,out] vector_ptr Pointer to a vector to resize.
         * @param[in]     size       The new size of the vector.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFMEM if the vector couldn't be resized;
         * @throw SIM_ERR_INVALARG if @e size < @c vector_ptr->count.
         */
        DYNAPI_PROC(void, sim_vector_resize,,
            Sim_Vector *const vector_ptr,
            const size_t      size
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Get an item from a vector at a given index.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to index into.
         * @param[in]     index        Index into the vector.
         * @param[out]    data_out_ptr Pointer to memory to fill with indexed data.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUFOFBND if @e index >= @c vector_ptr->count.
         */
        DYNAPI_PROC(void,  sim_vector_get,,
            Sim_Vector *const vector_ptr,
            const size_t      index,
            void*             data_out_ptr
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Get pointer to data in a vector at a given index.
         * 
         * @param[in,out] vector_ptr Pointer to vector to index into.
         * @param[in]     index      Index into the vector.
         *
         * @returns@c NULL on error; pointer to data otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUFOFBND if @e index >= @c vector_ptr->count.
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        DYNAPI_PROC(void*, sim_vector_get_ptr,,
            Sim_Vector *const vector_ptr,
            const size_t      index
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Find the index of the first item in a vector that tests equal to given data.
         * 
         * @param[in,out] vector_ptr     Pointer to vector to search.
         * @param[in]     item_ptr       Pointer to item to compare against.
         * @param[in]     predicate_proc Pointer to equality predicate function.
         * @param[in]     starting_index Index into the vector in which to begin the search.
         * 
         * @returns (size_t)-1 if the item is not found; vector index otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFBND if @e starting_index >= @c vector_ptr->count.
         * 
         * @sa sim_vector_contains
         */
        DYNAPI_PROC(size_t, sim_vector_find,,
            Sim_Vector *const vector_ptr,
            const void *const item_ptr,
            Sim_PredicateProc predicate_proc,
            const size_t      starting_index
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Checks if an item is contained in the vector.
         * 
         * @param[in,out] vector_ptr     Pointer to vector to search.
         * @param[in]     item_ptr       Pointer to item to compare against.
         * @param[in]     predicate_proc Pointer to equality predicate function.
         * 
         * @returns @c false if @e item_ptr isn't contained in @e vector_ptr; @c true otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFBND if @e starting_index >= @c vector_ptr->count.
         * 
         * @sa sim_vector_find
         */
        DYNAPI_PROC(bool, sim_vector_contains,,
            Sim_Vector *const vector_ptr,
            const void *const item_ptr,
            Sim_PredicateProc predicate_proc
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Push a new item to the back of a vector.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to push item into.
         * @param[in]     new_item_ptr Pointer to new item to push into vector.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFMEM if @e vector_ptr couldn't be resized to fit the new item.
         * 
         * @sa sim_vector_insert
         */
        DYNAPI_PROC(void, sim_vector_push,,
            Sim_Vector *const vector_ptr,
            const void*       new_item_ptr
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Insert a new item into a vector at a given index.
         * 
         * @param[in,out] vector_ptr   Pointer to the vector to insert item into.
         * @param[in]     new_item_ptr Pointer to new item to insert into vector.
         * @param[in]     index        Index into vector in which to insert new item.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFBND if @e index > @c vector_ptr->count;
         * @throw SIM_ERR_OUTOFMEM if @e vector_ptr couldn't be resized to fit the new item.
         * 
         * @sa sim_vector_push
         */
        DYNAPI_PROC(void, sim_vector_insert,,
            Sim_Vector *const vector_ptr,
            const void*       new_item_ptr,
            const size_t      index
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Pops an item off the back of a vector.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to pop item from.
         * @param[out]    item_out_ptr Pointer to memory to fill with popped item.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFBND if @c vector_ptr->count == 0.
         * 
         * @sa sim_vector_remove
         */
        DYNAPI_PROC(void, sim_vector_pop,,
            Sim_Vector *const vector_ptr,
            void*             item_out_ptr
        );
        
        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Removes an item from the vector at a given index.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to remove item from.
         * @param[out]    item_out_ptr Pointer to memory to fill with removed item.
         * @param[in]     index        Index into vector in which to remove an item.
         * 
         * @throw SIM_ERR_NULLPTR  if @e vector_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFBND if @e index >= vector_ptr->count.
         * 
         * @sa sim_vector_pop
         */
        DYNAPI_PROC(void, sim_vector_remove,,
            Sim_Vector *const vector_ptr,
            void*             item_out_ptr,
            const size_t      index
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Applies a given function to each item in the vector.
         * 
         * @param[in,out] vector_ptr   Pointer to vector whose items will be passed into the
         *                             given function.
         * @param[in]     foreach_proc Pointer to iteration function.
         * @param[in]     foreach_data Callback data to @e foreach_proc.
         * 
         * @returns @c false if the loop wasn't fully completed; @c true otherwise.
         * 
         * @throw SIM_ERR_NULLPTR if @e vector_ptr or @e foreach_proc are @c NULL.
         */
        DYNAPI_PROC(bool, sim_vector_foreach,,
            Sim_Vector *const vector_ptr,
            Sim_ForEachProc   foreach_proc,
            Sim_Variant       foreach_data
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Extracts items out of the vector based on a given function.
         * 
         * @param[in,out] vector_ptr      Pointer to vector whose items will be filtered through
         *                                the given function.
         * @param[in]     filter_proc     Pointer to filter function.
         * @param[in]     filter_data     Callback data to @e filter_proc.
         * @param[in,out] out_vector_ptr  Pointer to vector where extracted items will be
         *                                inserted into. Can be @c NULL.
         * 
         * @throw SIM_ERR_NULLPTR if @e vector_ptr or @e filter_proc are @c NULL.
         * 
         * @remarks If @e filter_proc returns @c true for a given item, it will remain
         *          in the vector, otherwise the item is removed and inserted into
         *          @e out_vector_ptr if it isn't @c NULL.
         * 
         * @sa sim_vector_select
         */
        DYNAPI_PROC(void, sim_vector_extract,,
            Sim_Vector *const vector_ptr,
            Sim_FilterProc    filter_proc,
            Sim_Variant       filter_data,
            Sim_Vector *const out_vector_ptr
        );

        /**
         * @relates @capi{Sim_Vector}
         * @ingroup containers
         * @brief Selects items from the vector based on a given function.
         * 
         * @param[in,out] vector_ptr      Pointer to vector whose items will be selected via the
         *                                given function.
         * @param[in]     select_proc     Pointer to selection function.
         * @param[in]     select_data     Callback data to @e select_proc.
         * @param[in,out] out_vector_ptr  Pointer to vector where filtered items will be
         *                                inserted into.
         * 
         * @throw SIM_ERR_NULLPTR if @e vector_ptr, @e select_proc, or @e out_vector_ptr are
         *                        @c NULL.
         * 
         * @remarks If @e select_proc returns @c true for a given item, it will be copied and
         *          inserted into @e out_vector_ptr.
         * 
         * @sa sim_vector_extract
         */
        DYNAPI_PROC(void, sim_vector_select,,
            Sim_Vector *const vector_ptr,
            Sim_FilterProc    select_proc,
            Sim_Variant       select_data,
            Sim_Vector *const out_vector_ptr
        );

    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus // start C++ API

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft) // end SimSoft namespace

#endif // SIMSOFT_VECTOR_H_
