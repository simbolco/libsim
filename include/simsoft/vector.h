/**
 * @file vector.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for vectors/array list containers
 * @version 0.1
 * @date 2020-01-09
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_VECTOR_H_
#define SIMSOFT_VECTOR_H_

#include "./common.h"
#include "./allocator.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

#       ifndef SIM_DEFAULT_VECTOR_SIZE
#           define SIM_DEFAULT_VECTOR_SIZE 32
#       endif
        
        /**
         * @struct Sim_Vector
         * @headerfile vector.h "simsoft/vector.h"
         * @brief Generic vector container / dynamically-sized array type
         * 
         * @tparam _item_properties Properties pertaining to the items stored in this vector.
         * @tparam _allocator_ptr   Pointer to allocator used when resizing internal array.
         */
        typedef struct Sim_Vector {
            const Sim_TypeInfo _item_properties; // properties of items in this vector
            const Sim_IAllocator *const _allocator_ptr; // array allocator
            size_t _allocated; // how much has been allocated

            size_t count;    // amount of items stored in the vector
            void*  data_ptr; // pointer to internal array
        } Sim_Vector;

        /**
         * @fn void sim_vector_construct(5)
         * @relates Sim_Vector
         * @brief Constructs a new vector.
         * 
         * @param[in,out] vector_ptr    Pointer to a vector to construct.
         * @param[in]     item_size     Size of each item.
         * @param[in]     item_type     Type of the items stored in the vector.
         * @param[in]     allocator_ptr Pointer to allocator to use when resizing internal array.
         * @param[in]     initial_size  The initial allocated size of the newly created vector.
         *
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if the vector size requested couldn't be allocated;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_destroy
         */
        extern EXPORT void C_CALL sim_vector_construct(
            Sim_Vector*           vector_ptr,
            const size_t          item_size,
            const Sim_DataType    item_type,
            const Sim_IAllocator* allocator_ptr,
            size_t                initial_size
        );

/*/ == TODO: We need to find a better way of doing this...
#       define sim_vector_create_sint8(allocator_ptr, initial_size) \
            sim_vector_create(                                      \
                sizeof(sint8),                                      \
                SIM_DATATYPE_INTEGRAL,                              \
                allocator_ptr,                                      \
                initial_size                                        \
            )
#       define sim_vector_create_sint16(allocator_ptr, initial_size) \
            sim_vector_create(                                       \
                sizeof(sint16),                                      \
                SIM_DATATYPE_INTEGRAL,                               \
                allocator_ptr,                                       \
                initial_size                                         \
            )
#       define sim_vector_create_sint32(allocator_ptr, initial_size) \
            sim_vector_create(                                       \
                sizeof(sint32),                                      \
                SIM_DATATYPE_INTEGRAL,                               \
                allocator_ptr,                                       \
                initial_size                                         \
            )
#       define sim_vector_create_sint64(allocator_ptr, initial_size) \
            sim_vector_create(                                       \
                sizeof(sint64),                                      \
                SIM_DATATYPE_INTEGRAL,                               \
                allocator_ptr,                                       \
                initial_size                                         \
            )

#       define sim_vector_create_char(allocator_ptr, initial_size) \
            sim_vector_create(                                     \
                sizeof(char),                                      \
                SIM_DATATYPE_INTEGRAL,                             \
                allocator_ptr,                                     \
                initial_size                                       \
            )
#       define sim_vector_create_short(allocator_ptr, initial_size) \
            sim_vector_create(                                      \
                sizeof(short),                                      \
                SIM_DATATYPE_INTEGRAL,                              \
                allocator_ptr,                                      \
                initial_size                                        \
            )
#       define sim_vector_create_int(allocator_ptr, initial_size) \
            sim_vector_create(                                    \
                sizeof(int),                                      \
                SIM_DATATYPE_INTEGRAL,                            \
                allocator_ptr,                                    \
                initial_size                                      \
            )
#       define sim_vector_create_long(allocator_ptr, initial_size) \
            sim_vector_create(                                     \
                sizeof(long),                                      \
                SIM_DATATYPE_INTEGRAL,                             \
                allocator_ptr,                                     \
                initial_size                                       \
            )
#       define sim_vector_create_longlong(allocator_ptr, initial_size) \
            sim_vector_create(                                         \
                sizeof(long long),                                     \
                SIM_DATATYPE_INTEGRAL,                                 \
                allocator_ptr,                                         \
                initial_size                                           \
            )

#       define sim_vector_create_uint8(allocator_ptr, initial_size) \
            sim_vector_create(                                      \
                sizeof(uint8),                                      \
                SIM_DATATYPE_UNSIGNED,                              \
                allocator_ptr,                                      \
                initial_size                                        \
            )
#       define sim_vector_create_uint16(allocator_ptr, initial_size) \
            sim_vector_create(                                       \
                sizeof(uint16),                                      \
                SIM_DATATYPE_UNSIGNED,                               \
                allocator_ptr,                                       \
                initial_size                                         \
            )
#       define sim_vector_create_uint32(allocator_ptr, initial_size) \
            sim_vector_create(                                       \
                sizeof(uint32),                                      \
                SIM_DATATYPE_UNSIGNED,                               \
                allocator_ptr,                                       \
                initial_size                                         \
            )
#       define sim_vector_create_uint64(allocator_ptr, initial_size) \
            sim_vector_create(                                       \
                sizeof(uint64),                                      \
                SIM_DATATYPE_UNSIGNED,                               \
                allocator_ptr,                                       \
                initial_size                                         \
            )

#       define sim_vector_create_uchar(allocator_ptr, initial_size) \
            sim_vector_create(                                      \
                sizeof(unsigned char),                              \
                SIM_DATATYPE_UNSIGNED,                              \
                allocator_ptr,                                      \
                initial_size                                        \
            )
#       define sim_vector_create_ushort(allocator_ptr, initial_size) \
            sim_vector_create(                                       \
                sizeof(unsigned short),                              \
                SIM_DATATYPE_UNSIGNED,                               \
                allocator_ptr,                                       \
                initial_size                                         \
            )
#       define sim_vector_create_uint(allocator_ptr, initial_size) \
            sim_vector_create(                                     \
                sizeof(unsigned int),                              \
                SIM_DATATYPE_UNSIGNED,                             \
                allocator_ptr,                                     \
                initial_size                                       \
            )
#       define sim_vector_create_ulong(allocator_ptr, initial_size) \
            sim_vector_create(                                      \
                sizeof(unsigned long),                              \
                SIM_DATATYPE_UNSIGNED,                              \
                allocator_ptr,                                      \
                initial_size                                        \
            )
#       define sim_vector_create_ulonglong(allocator_ptr, initial_size) \
            sim_vector_create(                                          \
                sizeof(unsigned long long),                             \
                SIM_DATATYPE_UNSIGNED,                                  \
                allocator_ptr,                                          \
                initial_size                                            \
            )

#       define sim_vector_create_float(allocator_ptr, initial_size) \
            sim_vector_create(                                      \
                sizeof(float),                                      \
                SIM_DATATYPE_FLOAT,                                 \
                allocator_ptr,                                      \
                initial_size                                        \
            )
#       define sim_vector_create_double(allocator_ptr, initial_size) \
            sim_vector_create(                                       \
                sizeof(double),                                      \
                SIM_DATATYPE_FLOAT,                                  \
                allocator_ptr,                                       \
                initial_size                                         \
            )

        *//**
         * @def Sim_Vector* sim_vector_create_usertype(T, 2)
         * @relates Sim_Vector
         * @brief Creates a new vector storing a user-defined type on the heap.
         * 
         * @tparam    TYPE          The user-defined type the vector will be storing.
         * @param[in] allocator_ptr Pointer to allocator to use when resizing internal array.
         * @param[in] initial_size  The initial size of the newly created vector.
         * 
         * @return @c NULL if the vector couldn't be allocated;
         *         pointer to a Sim_Vector otherwise.
         * 
         * @sa sim_vector_create
         *//*
#       define sim_vector_create_usertype(TYPE, allocator_ptr, initial_size) \
            sim_vector_create(                                               \
                sizeof(TYPE),                                                \
                SIM_DATATYPE_OTHER,                                          \
                allocator_ptr,                                               \
                initial_size                                                 \
            ) */

        /**
         * @fn void sim_vector_destroy(1)
         * @relates Sim_Vector
         * @brief Destroys a vector.
         * 
         * @param[in] vector_ptr Pointer to vector to destroy.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr is @c NULL;
         *     @b SIM_RC_SUCCESS otherwise
         * 
         * @sa sim_vector_construct
         */
        extern EXPORT void C_CALL sim_vector_destroy(
            Sim_Vector *const vector_ptr
        );
        
        /**
         * @def bool sim_vector_is_empty(1)
         * @relates Sim_Vector
         * @brief Checks if the vector is empty.
         * 
         * @param[in] vector_ptr Pointer to a vector to check.
         * 
         * @return @c true if the vector is empty; @c false otherwise.
         */
#       define sim_vector_is_empty(vector_ptr) \
            ((vector_ptr)->count == 0)

        /**
         * @fn void sim_vector_clear(1)
         * @relates Sim_Vector
         * @brief Clears a vector of all its contents.
         * 
         * @param[in,out] vector_ptr Pointer to vector to empty.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr is @c NULL ;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void C_CALL sim_vector_clear(
            Sim_Vector *const vector_ptr
        );

        /**
         * @fn void sim_vector_resize(2)
         * @relates Sim_Vector
         * @brief Resizes a vector to a given size.
         * 
         * @param[in,out] vector_ptr Pointer to a vector to resize.
         * @param[in]     size       The new size of the vector.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if the vector couldn't be resized;
         *     @b SIM_RC_ERR_INVALARG if @e size < @c vector_ptr->count ;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void C_CALL sim_vector_resize(
            Sim_Vector *const vector_ptr,
            const size_t size
        );

        /**
         * @fn void sim_vector_get(3)
         * @relates Sim_Vector
         * @brief Get an item from a vector at a given index.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to index into.
         * @param[in]     index        Index into the vector.
         * @param[out]    data_out_ptr Pointer to memory to fill with indexed data.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFBND if @e index >= @c vector_ptr->count ;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void C_CALL sim_vector_get(
            Sim_Vector *const vector_ptr,
            const size_t      index,
            void*             data_out_ptr
        );

        /**
         * @fn void* sim_vector_get_ptr(2)
         * @relates Sim_Vector
         * @brief Get pointer to data in a vector at a given index.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to index into.
         * @param[in]     index        Index into the vector.
         *
         * @return @c NULL on error (see remarks); pointer to data otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFBND if @e index >= @c vector_ptr->count ;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void* C_CALL sim_vector_get_ptr(
            Sim_Vector *const vector_ptr,
            const size_t      index
        );

        /**
         * @fn size_t sim_vector_index_of(4)
         * @relates Sim_Vector
         * @brief Gets the index of the first item in a vector that tests equal to given data.
         * 
         * @param[in,out] vector_ptr          Pointer to vector to search.
         * @param[in]     item_ptr            Pointer to item to compare against.
         * @param[in]     predicate_func_ptr  Pointer to equality predicate function.
         * @param[in]     starting_index      Index into the vector in which to begin the search.
         * 
         * @return (size_t)-1 on error (see remarks); vector index otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr, @e item_ptr, or @e predicate_func_ptr
         *                            are @c NULL ;
         *     @b SIM_RC_ERR_OUTOFBND if @e starting_index >= @c vector_ptr->count ;
         *     @b SIM_RC_ERR_NOTFOUND if no item in the vector is equivalent to @e item_ptr;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_contains
         */
        extern EXPORT size_t C_CALL sim_vector_index_of(
            Sim_Vector *const    vector_ptr,
            const void *const    item_ptr,
            Sim_PredicateFuncPtr predicate_func_ptr,
            const size_t         starting_index
        );

        /**
         * @fn bool sim_vector_contains(3)
         * @relates Sim_Vector
         * @brief Checks if an item is contained in the vector.
         * 
         * @param[in,out] vector_ptr         Pointer to vector to search.
         * @param[in]     item_ptr           Pointer to item to compare against.
         * @param[in]     predicate_func_ptr Pointer to equality predicate function.
         * 
         * @return @c false on error (see remarks) or if @e item_ptr isn't contained in
         *            @e vector_ptr;
         *         @c true otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr, @e item_ptr, or @e predicate_func_ptr
         *                            are @c NULL ;
         *     @b SIM_RC_ERR_NOTFOUND if no item in the vector is equivalent to @e item_ptr;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_index_of
         */
        extern EXPORT bool C_CALL sim_vector_contains(
            Sim_Vector *const    vector_ptr,
            const void *const    item_ptr,
            Sim_PredicateFuncPtr predicate_func_ptr
        );

        /**
         * @fn void sim_vector_push(2)
         * @relates Sim_Vector
         * @brief Push a new item to the back of a vector.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to push item into.
         * @param[in]     new_item_ptr Pointer to new item to push into vector.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr or @e new_item_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if @e vector_ptr couldn't be resized to fit the new item;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_insert
         */
        extern void C_CALL sim_vector_push(
            Sim_Vector *const vector_ptr,
            const void*       new_item_ptr
        );

        /**
         * @fn void sim_vector_insert(3)
         * @relates Sim_Vector
         * @brief Insert a new item into a vector at a given index.
         * 
         * @param[in,out] vector_ptr   Pointer to the vector to insert item into.
         * @param[in]     new_item_ptr Pointer to new item to insert into vector.
         * @param[in]     index        Index into vector in which to insert new item.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr or @e new_item_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFBND if @e index > vector_ptr->count ;
         *     @b SIM_RC_ERR_OUTOFMEM if @e vector_ptr couldn't be resized to fit the new item;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_push
         */
        extern EXPORT void C_CALL sim_vector_insert(
            Sim_Vector *const vector_ptr,
            const void*       new_item_ptr,
            const size_t      index
        );

        /**
         * @fn void sim_vector_pop(2)
         * @relates Sim_Vector
         * @brief Pops an item off the back of a vector.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to pop item from.
         * @param[out]    item_out_ptr Pointer to memory to fill with popped item.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFBND if @c vector_ptr->count == 0;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_remove
         */
        extern EXPORT void C_CALL sim_vector_pop(
            Sim_Vector *const vector_ptr,
            void*             item_out_ptr
        );
        
        /**
         * @fn void sim_vector_remove(3)
         * @relates Sim_Vector
         * @brief Removes an item from the vector at a given index.
         * 
         * @param[in,out] vector_ptr   Pointer to vector to remove item from.
         * @param[out]    item_out_ptr Pointer to memory to fill with removed item.
         * @param[in]     index        Index into vector in which to remove an item.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFBND if @e index >= vector_ptr->count;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_pop
         */
        extern EXPORT void C_CALL sim_vector_remove(
            Sim_Vector *const vector_ptr,
            void*             item_out_ptr,
            const size_t      index
        );

        /**
         * @fn bool sim_vector_foreach(3)
         * @relates Sim_Vector
         * @brief Applies a given function to each item in the vector.
         * 
         * @param[in,out] vector_ptr       Pointer to vector whose items will be passed into the
         *                                 given function.
         * @param[in]     foreach_func_ptr Pointer to iteration function.
         * @param[in]     userdata         User-provided data to @e foreach_func_ptr.
         * 
         * @return @c false on error (see remarks) or if the loop wasn't fully completed;
         *         @c true  otherwise.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr or @e foreach_func_ptr are @c NULL;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT bool C_CALL sim_vector_foreach(
            Sim_Vector *const vector_ptr,
            Sim_ForEachFuncPtr foreach_func_ptr,
            Sim_Variant userdata
        );

        /**
         * @fn void sim_vector_extract(4)
         * @relates Sim_Vector
         * @brief Extracts items out of the vector based on a given function.
         * 
         * @param[in,out] vector_ptr      Pointer to vector whose items will be filtered through
         *                                the given function.
         * @param[in]     filter_func_ptr Pointer to filter function.
         * @param[in]     userdata        User-provided data to @e filter_func_ptr.
         * @param[in,out] out_vector_ptr  Pointer to vector where extracted items will be
         *                                inserted into.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr or @e filter_func_ptr are @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @remarks If @e filter_func_ptr returns @c true for a given item, it will remain
         *          in the vector, otherwise the item is removed and inserted into
         *          @e out_vector_ptr if it isn't @c NULL .
         * 
         * @sa sim_vector_select
         */
        extern EXPORT void C_CALL sim_vector_extract(
            Sim_Vector *const vector_ptr,
            Sim_FilterFuncPtr filter_func_ptr,
            Sim_Variant       userdata,
            Sim_Vector *const out_vector_ptr
        );

        /**
         * @fn void sim_vector_select(4)
         * @relates Sim_Vector
         * @brief Selects items from the vector based on a given function.
         * 
         * @param[in,out] vector_ptr      Pointer to vector whose items will be selected via
         *                                the given function.
         * @param[in]     select_func_ptr Pointer to selection function.
         * @param[in]     userdata        User-provided data to @e select_func_ptr.
         * @param[in,out] out_vector_ptr  Pointer to vector where filtered items will be
         *                                inserted into.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr, @e select_func_ptr, or
         *                           @e out_vector_ptr are @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @remarks If @e select_func_ptr returns @c true for a given item, it will be
         *          copied and inserted into @e out_vector_ptr.
         * 
         * @sa sim_vector_extract
         */
        extern EXPORT void C_CALL sim_vector_select(
            Sim_Vector *const vector_ptr,
            Sim_FilterFuncPtr select_func_ptr,
            Sim_Variant       userdata,
            Sim_Vector *const out_vector_ptr
        );

        /**
         * @fn void sim_vector_sort(2)
         * @relates Sim_Vector
         * @brief Sorts items in the vector based on initialization settings or a user-provided
         *        comparison function.
         * 
         * @param[in,out] vector_ptr          Pointer to vector whose items will be sorted.
         * @param[in]     comparison_func_ptr Pointer to comparison function.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr is @c NULL or if @e comparison_func_ptr is
         *                           @c NULL under certain conditions (see remarks);
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @remarks Faster sorting procedures are used when built-in C numeric datatypes are
         *          stored in the vector. If the vector was intialized with any of the following
         *          initialization settings via @c sim_vector_create , @e comparison_func_ptr can
         *          be passed as @c NULL to use the faster sorting functions:
         *          <ul>
         *              <li> {@c char; unsigned char; sint8; uint8}
         *                  <ul><li> @e item_size: 1 </li>
         *                  <li> @e item_type: @b INTEGRAL, @b UNSIGNED </li></ul>
         *              </li>
         *              <li> {@c short; unsigned short; sint16; uint16}
         *                  <ul><li> @e item_size: 2 </li>
         *                  <li> @e item_type: @b INTEGRAL, @b UNSIGNED </li></ul>
         *              </li>
         *              <li> {@c int; unsigned int; float; sint32; uint32}
         *                  <ul><li> @e item_size: 4 </li>
         *                  <li> @e item_type: @b INTEGRAL, @b UNSIGNED, @b FLOAT </li></ul>
         *              </li>
         *              <li> {@c long; long long; unsigned long; unsigned long long; double;
         *              sint64; uint64}
         *                  <ul><li> @e item_size: 8 </li>
         *                  <li> @e item_type: @b INTEGRAL, @b UNSIGNED, @b FLOAT </li></ul>
         *              </li>
         *          </ul>
         *          Any other combination of @e item_type and @e item_size used when the vector
         *          was initialized is required to pass in a non-NULL @e comparison_func_ptr,
         *          otherwise @b SIM_RC_ERR_NULLPTR will be returned.
         */
        extern EXPORT void C_CALL sim_vector_sort(
            Sim_Vector *const     vector_ptr,
            Sim_ComparisonFuncPtr comparison_func_ptr
        );

    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
#       include "./exception.hpp"

        template <
            class T,
            class _Pred = Predicate_Equal<T>,
            class _Alloc = Allocator
        >
        class Vector {
        private:
            C_API::Sim_Vector _c_vector;

            template <bool Reversed>
            class Iterator {
            private:
                T* _item_ptr;
            
            public:
                Iterator(T* item_ptr) : _item_ptr(item_ptr);
            };

            template <bool Reversed>
            class ConstIterator {
                const T* _item_ptr;

            public:
                ConstIterator(const T* item_ptr) : _item_ptr(item_ptr);
            };

            static bool _predicate_helper(const T *const item1, const T *const item2) const {
                return _Pred(*item1, *item2);
            }

        public:
            typedef T value_type;
            typedef T& reference;
            typedef const T& const_reference;
            typedef T* pointer;
            typedef const T* const_pointer;
            typedef T *const pointer_const;
            typedef const T *const const_pointer_const;

            typedef _Alloc allocator;

            typedef Iterator<false> iterator;
            typedef Iterator<true>  reverse_iterator;
            typedef ConstIterator<false> const_iterator;
            typedef ConstIterator<true>  const_reverse_iterator;

            constexpr size_t DEFAULT_SIZE = SIM_DEFAULT_VECTOR_SIZE;

            enum DataType {
                OTHER    = C_API::Sim_DataType::SIM_DATATYPE_OTHER,
                INTEGRAL = C_API::Sim_DataType::SIM_DATATYPE_INTEGRAL,
                UNSIGNED = C_API::Sim_DataType::SIM_DATATYPE_UNSIGNED,
                FLOAT    = C_API::Sim_DataType::SIM_DATATYPE_FLOAT
            };

            Vector(size_t initial_size = DEFAULT_SIZE) throws Exception {
                ReturnCode rc;
                
                if ((rc = (ReturnCode)sim_vector_initialize(
                    &_c_vector,
                    sizeof(T),
                    DataType::OTHER,
                    initial_size
                )))
                    throw Exception(rc);
            }

            ~Vector() {
                sim_vector_destroy(&_c_vector);
            }

            void clear() {
                sim_vector_clear(&_c_vector);
            }

            void resize(size_t new_size) throws Exception {
                ReturnCode rc;
                
                if ((rc = (ReturnCode)sim_vector_resize(
                    &_c_vector,
                    new_size
                )))
                    throw Exception(rc);
            }

            T& get(size_t index) throws Exception {
                ReturnCode rc;
                T* ptr;
                
                if ((rc = (ReturnCode)sim_vector_get_ptr(
                    &_c_vector,
                    index,
                    &((void*)ptr)
                )))
                    throw Exception(rc);
                
                return *ptr;
            }
            const T& get(size_t index) const throws Exception {
                ReturnCode rc;
                const T* ptr;
                
                if ((rc = (ReturnCode)sim_vector_get_ptr(
                    &_c_vector,
                    index,
                    &((void*)const_cast<T*>(ptr))
                )))
                    throw Exception(rc);
                
                return *ptr;
            }

            T& operator[](size_t index) throws Exception {
                return get(index);
            }
            const T& operator[](size_t index) const throws Exception {
                return get(index);
            }

            void push(const T& item) throws Exception {
                ReturnCode rc;
                
                if ((rc = (ReturnCode)sim_vector_push(
                    &_c_vector,
                    &item
                )))
                    throw Exception(rc);
            }
            void push(T&& item) throws Exception {
                ReturnCode rc;
                
                if ((rc = (ReturnCode)sim_vector_push(
                    &_c_vector,
                    &item
                )))
                    throw Exception(rc);
            }

            void insert(const T& item, size_t index) throws Exception {
                ReturnCode rc;
                
                if ((rc = (ReturnCode)sim_vector_insert(
                    &_c_vector,
                    &item,
                    index
                )))
                    throw Exception(rc);
            }
            void insert(T&& item, size_t index) throws Exception {
                ReturnCode rc;
                
                if ((rc = (ReturnCode)sim_vector_insert(
                    &_c_vector,
                    &item,
                    index
                )))
                    throw Exception(rc);
            }

            T pop() throws Exception {
                ReturnCode rc;
                T temp;

                if ((rc = (ReturnCode)sim_vector_pop(
                    &_c_vector,
                    &temp
                )))
                    throw Exception(rc);

                return temp;
            }

            T remove(size_t index) throws Exception {
                ReturnCode rc;
                T temp;

                if ((rc = (ReturnCode)sim_vector_remove(
                    &_c_vector,
                    &temp,
                    index
                )))
                    throw Exception(rc);

                return temp;
            }

            size_t index_of(const T& item, size_t starting_index = 0) throws Exception {
                ReturnCode rc;
                size_t index;

                if ((rc = (ReturnCode)sim_vector_index_of(
                    &_c_vector,
                    &item,
                    ::_predicate_helper,
                    starting_index,
                    &index
                )))
                    throw Exception(rc);
                
                return index;
            }
            size_t index_of(T&& item, size_t starting_index = 0) throws Exception {
                ReturnCode rc;
                size_t index;

                if ((rc = (ReturnCode)sim_vector_index_of(
                    &_c_vector,
                    &item,
                    ::_predicate_helper,
                    starting_index,
                    &index
                )))
                    throw Exception(rc);
                
                return index;
            }

            bool contains(const T& item) noexcept const {
                return sim_vector_contains(
                    &_c_vector,
                    &item,
                    ::_predicate_helper
                );
            }
            bool contains(T&& item) noexcept const {
                return sim_vector_contains(
                    &_c_vector,
                    &item,
                    ::_predicate_helper
                );
            }
        };

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_VECTOR_H_ */
