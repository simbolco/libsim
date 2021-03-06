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

        /**
         * @def SIM_DEFAULT_VECTOR_SIZE
         * 
         */
#       ifndef SIM_DEFAULT_VECTOR_SIZE
#           define SIM_DEFAULT_VECTOR_SIZE 32
#       endif
        
        /**
         * @struct Sim_Vector
         * @headerfile vector.h "simsoft/vector.h"
         * @brief Generic dynamic array container type.
         * 
         * @tparam _item_size     How large the items contained in the vector are.
         * @tparam _allocator_ptr Pointer to allocator used when resizing internal array.
         * 
         * @var Sim_Vector::count
         *     The number of items contained in the vector.
         * @var Sim_Vector::data_ptr
         *     Pointer to the internal array used by the vector.
         * @var Sim_Vector::_allocated @private
         *     The number of items allocated for the internal array.
         */
        typedef struct Sim_Vector {
            const size_t _item_size;
            const Sim_IAllocator *const _allocator_ptr;
            size_t _allocated; // how much has been allocated

            size_t count;
            void*  data_ptr;
        } Sim_Vector;

        /**
         * @fn void sim_vector_construct(
         *         Sim_Vector *const,
         *         const size_t,
         *         const Sim_IAllocator*,
         *         size_t
         *     )
         * @relates @capi{Sim_Vector}
         * @brief Constructs a new vector.
         * 
         * @param[in,out] vector_ptr    Pointer to a vector to construct.
         * @param[in]     item_size     Size of each item.
         * @param[in]     allocator_ptr Pointer to allocator to use when resizing internal array.
         * @param[in]     initial_size  The initial size of the newly created vector.
         *
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if the vector size requested couldn't be allocated;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_destroy
         */
        extern EXPORT void C_CALL sim_vector_construct(
            Sim_Vector *const     vector_ptr,
            const size_t          item_size,
            const Sim_IAllocator* allocator_ptr,
            size_t                initial_size
        );

        /**
         * @fn void sim_vector_destroy(Sim_Vector *const)
         * @relates @capi{Sim_Vector}
         * @brief Destroys a vector.
         * 
         * @param[in] vector_ptr Pointer to vector to destroy.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr is @c NULL;
         *     @b SIM_RC_SUCCESS otherwise.
         * 
         * @sa sim_vector_construct
         */
        extern EXPORT void C_CALL sim_vector_destroy(
            Sim_Vector *const vector_ptr
        );
        
        /**
         * @fn bool sim_vector_is_empty(Sim_Vector *const)
         * @relates @capi{Sim_Vector}
         * @brief Checks if a vector is empty.
         * 
         * @param[in] vector_ptr Pointer to a vector to check.
         * 
         * @return @c true if the vector is empty; @c false otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr is @c NULL;
         *     @b SIM_RC_SUCCESS otherwise.
         */
        extern EXPORT bool C_CALL sim_vector_is_empty(
            Sim_Vector *const vector_ptr
        );

        /**
         * @fn void sim_vector_clear(Sim_Vector *const)
         * @relates @capi{Sim_Vector}
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
         * @fn void sim_vector_resize(Sim_Vector *const, const size_t)
         * @relates @capi{Sim_Vector}
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
            const size_t      size
        );

        /**
         * @fn void sim_vector_get(Sim_Vector *const, const size_t, void*)
         * @relates @capi{Sim_Vector}
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
         * @fn void* sim_vector_get_ptr(Sim_Vector *const, const size_t)
         * @relates @capi{Sim_Vector}
         * @brief Get pointer to data in a vector at a given index.
         * 
         * @param[in,out] vector_ptr Pointer to vector to index into.
         * @param[in]     index      Index into the vector.
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
         * @fn size_t sim_vector_find(
         *         Sim_Vector *const,
         *         const void *const,
         *         Sim_PredicateProc,
         *         const size_t
         *     )
         * @relates @capi{Sim_Vector}
         * @brief Find the index of the first item in a vector that tests equal to given data.
         * 
         * @param[in,out] vector_ptr     Pointer to vector to search.
         * @param[in]     item_ptr       Pointer to item to compare against.
         * @param[in]     predicate_proc Pointer to equality predicate function.
         * @param[in]     starting_index Index into the vector in which to begin the search.
         * 
         * @return (size_t)-1 on error (see remarks); vector index otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e vector_ptr, @e item_ptr, or @e predicate_proc
         *                            are @c NULL ;
         *     @b SIM_RC_ERR_OUTOFBND if @e starting_index >= @c vector_ptr->count ;
         *     @b SIM_RC_NOT_FOUND    if no item in the vector is equivalent to @e item_ptr;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_vector_contains
         */
        extern EXPORT size_t C_CALL sim_vector_find(
            Sim_Vector *const vector_ptr,
            const void *const item_ptr,
            Sim_PredicateProc predicate_proc,
            const size_t      starting_index
        );

        /**
         * @fn bool sim_vector_contains(
         *         Sim_Vector *const,
         *         const void *const,
         *         Sim_PredicateProc
         *     )
         * @relates @capi{Sim_Vector}
         * @brief Checks if an item is contained in the vector.
         * 
         * @param[in,out] vector_ptr     Pointer to vector to search.
         * @param[in]     item_ptr       Pointer to item to compare against.
         * @param[in]     predicate_proc Pointer to equality predicate function.
         * 
         * @return @c false on error (see remarks) or if @e item_ptr isn't contained in
         *            @e vector_ptr;
         *         @c true otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr, @e item_ptr, or @e predicate_proc
         *                           are @c NULL ;
         *     @b SIM_RC_NOT_FOUND   if no item in the vector is equivalent to @e item_ptr;
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @sa sim_vector_find
         */
        extern EXPORT bool C_CALL sim_vector_contains(
            Sim_Vector *const vector_ptr,
            const void *const item_ptr,
            Sim_PredicateProc predicate_proc
        );

        /**
         * @fn void sim_vector_push(Sim_Vector *const, const void*)
         * @relates @capi{Sim_Vector}
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
         * @fn void sim_vector_insert(Sim_Vector *const, const void*, const size_t)
         * @relates @capi{Sim_Vector}
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
         * @fn void sim_vector_pop(Sim_Vector *const, void*)
         * @relates @capi{Sim_Vector}
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
         * @fn void sim_vector_remove(Sim_Vector *const, void*, const size_t)
         * @relates @capi{Sim_Vector}
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
         * @fn bool sim_vector_foreach(Sim_Vector *const, Sim_ForEachProc, Sim_Variant)
         * @relates @capi{Sim_Vector}
         * @brief Applies a given function to each item in the vector.
         * 
         * @param[in,out] vector_ptr   Pointer to vector whose items will be passed into the
         *                             given function.
         * @param[in]     foreach_proc Pointer to iteration function.
         * @param[in]     userdata     User-provided data to @e foreach_proc.
         * 
         * @return @c false on error (see remarks) or if the loop wasn't fully completed;
         *         @c true  otherwise.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr or @e foreach_proc are @c NULL;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT bool C_CALL sim_vector_foreach(
            Sim_Vector *const vector_ptr,
            Sim_ForEachProc   foreach_proc,
            Sim_Variant       userdata
        );

        /**
         * @fn void sim_vector_extract(
         *         Sim_Vector *const,
         *         Sim_FilterProc,
         *         Sim_Variant,
         *         Sim_Vector *const
         *     )
         * @relates @capi{Sim_Vector}
         * @brief Extracts items out of the vector based on a given function.
         * 
         * @param[in,out] vector_ptr      Pointer to vector whose items will be filtered through
         *                                the given function.
         * @param[in]     filter_proc     Pointer to filter function.
         * @param[in]     userdata        User-provided data to @e filter_proc.
         * @param[in,out] out_vector_ptr  Pointer to vector where extracted items will be
         *                                inserted into.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr or @e filter_proc are @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @remarks If @e filter_proc returns @c true for a given item, it will remain
         *          in the vector, otherwise the item is removed and inserted into
         *          @e out_vector_ptr if it isn't @c NULL .
         * 
         * @sa sim_vector_select
         */
        extern EXPORT void C_CALL sim_vector_extract(
            Sim_Vector *const vector_ptr,
            Sim_FilterProc    filter_proc,
            Sim_Variant       userdata,
            Sim_Vector *const out_vector_ptr
        );

        /**
         * @fn void sim_vector_select(
         *         Sim_Vector *const,
         *         Sim_FilterProc,
         *         Sim_Variant,
         *         Sim_Vector *const
         *     )
         * @relates @capi{Sim_Vector}
         * @brief Selects items from the vector based on a given function.
         * 
         * @param[in,out] vector_ptr      Pointer to vector whose items will be selected via
         *                                the given function.
         * @param[in]     select_proc     Pointer to selection function.
         * @param[in]     userdata        User-provided data to @e select_proc.
         * @param[in,out] out_vector_ptr  Pointer to vector where filtered items will be
         *                                inserted into.
         * 
         * @remarks sim_return_code() is set to one of the folliwng:
         *     @b SIM_RC_ERR_NULLPTR if @e vector_ptr, @e select_proc, or
         *                           @e out_vector_ptr are @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @remarks If @e select_proc returns @c true for a given item, it will be copied and
         *          inserted into @e out_vector_ptr.
         * 
         * @sa sim_vector_extract
         */
        extern EXPORT void C_CALL sim_vector_select(
            Sim_Vector *const vector_ptr,
            Sim_FilterProc    select_proc,
            Sim_Variant       userdata,
            Sim_Vector *const out_vector_ptr
        );

    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
#       include "./exception.hpp"

        /**
         * @class SimSoft::Vector
         * @brief Generic dynamic array container class.
         * 
         * @tparam T      The type being stored in this vector.
         * @tparam _Pred  Predicate used to check for equality.
         * @tparam _Alloc Allocator for internal array.
         */
        template <
            class T,
            class _Pred = Predicate_Equal<T>,
            class _Alloc = Allocator
        >
        class Vector : private C_API::Sim_Vector {
        private:
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

            static C_API::Sim_Vector* _get_c_vector() {
                return (C_API::Sim_Vector*)((uint8*)this) + offsetof(Vector<T>, _item_size);
            }

        public:
            /**
             * @typedef SimSoft::Vector::value_type
             * @brief The type being stored in this Vector.
             */
            typedef T value_type;
            /**
             * @typedef SimSoft::Vector::reference
             * @brief Reference to @e value_type.
             */
            typedef T& reference;
            /**
             * @typedef SimSoft::Vector::const_reference
             * @brief Const reference to @e value_type.
             */
            typedef const T& const_reference;
            /**
             * @typedef SimSoft::Vector::pointer
             * @brief Pointer to @e value_type.
             */
            typedef T* pointer;
            /**
             * @typedef SimSoft::Vector::const_pointer
             * @brief Const pointer to @e value_type.
             */
            typedef const T* const_pointer;
            /**
             * @typedef SimSoft::Vector::pointer_const
             * @brief Pointer const to @e value_type.
             */
            typedef T *const pointer_const;
            /**
             * @typedef SimSoft::Vector::const_pointer_const
             * @brief Const pointer const to @e value_type.
             */
            typedef const T *const const_pointer_const;

            /**
             * @typedef SimSoft::Vector::allocator
             * @brief The allocator type used by this vector.
             */
            typedef _Alloc allocator;

            /**
             * @typedef SimSoft::Vector::iterator
             * @brief Vector forward iterator type.
             */
            typedef Iterator<false> iterator;
            /**
             * @typedef SimSoft::Vector::reverse_iterator
             * @brief Vector reverse iterator type.
             */
            typedef Iterator<true>  reverse_iterator;
            /**
             * @typedef SimSoft::Vector::const_iterator
             * @brief Vector const forward iterator type.
             */
            typedef ConstIterator<false> const_iterator;
            /**
             * @typedef SimSoft::Vector::const_reverse_iterator
             * @brief Vector const reverse iterator type.
             */
            typedef ConstIterator<true>  const_reverse_iterator;

            /**
             * @var SimSoft::Vector::DEFAULT_SIZE
             * @brief The default size used when intializing a new vector.
             */
            constexpr static size_t DEFAULT_SIZE = SIM_DEFAULT_VECTOR_SIZE;

            /**
             * @fn SimSoft::Vector::Vector(size_t)
             * @brief Constructs a new Vector object.
             * 
             * @param[in] initial_size @b Optional: The initial size of the newly created Vector.
             *                         Defaults to @e Vector::DEFAULT_SIZE.
             * 
             * @throws Exception If the @e initial_size requested couldn't be allocated.
             */
            Vector(size_t initial_size = DEFAULT_SIZE) throws Exception {
                C_API::sim_vector_construct(
                    _get_c_vector(),
                    sizeof(value_type),
                    allocator::get_c_allocator(),
                    initial_size
                );
                ReturnCode rc == (ReturnCode)C_API::sim_return_code();
                if (rc > ReturnCode::FAILURE)
                    throw Exception();
            }

            ~Vector() {
                sim_vector_destroy(_get_c_vector());
            }

            void clear() {
                sim_vector_clear(_get_c_vector());
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

            size_t find(const T& item, size_t starting_index = 0) throws Exception {
                ReturnCode rc;
                size_t index;

                if ((rc = (ReturnCode)sim_vector_find(
                    &_c_vector,
                    &item,
                    ::_predicate_helper,
                    starting_index,
                    &index
                )))
                    throw Exception(rc);
                
                return index;
            }
            size_t find(T&& item, size_t starting_index = 0) throws Exception {
                ReturnCode rc;
                size_t index;

                if ((rc = (ReturnCode)sim_vector_find(
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
