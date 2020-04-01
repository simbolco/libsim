/**
 * @file vector.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/vector.h
 * @version 0.1
 * @date 2020-01-09
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_VECTOR_C_
#define SIMSOFT_VECTOR_C_

#include "simsoft/vector.h"
#include "./_internal.h"

// sim_vector_construct(4): Constructs a new vector.
void sim_vector_construct(
    Sim_Vector*           vector_ptr,
    const size_t          item_size,
    const Sim_IAllocator* allocator_ptr,
    size_t                initial_size
) {
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR,);

    if (!initial_size)
        initial_size = SIM_DEFAULT_VECTOR_SIZE;
        
    // use default allocator on NULL
    if (!allocator_ptr)
        allocator_ptr = sim_allocator_get_default();

    // allocate internal array
    void* data_ptr = allocator_ptr->malloc(item_size * initial_size);
    RETURN_IF(!data_ptr, SIM_RC_ERR_OUTOFMEM,);
    
    // set up initial properties of Sim_Vector
    Sim_Vector vec = {
        ._item_size = item_size,
        ._allocator_ptr = allocator_ptr,
        ._allocated = initial_size,

        .count = 0,
        .data_ptr = data_ptr
    };

    // copy to vector pointer
    memcpy(vector_ptr, &vec, sizeof(Sim_Vector));
    RETURN(SIM_RC_SUCCESS,);
}

// sim_vector_destroy(1): Destroys a vector.
void sim_vector_destroy(Sim_Vector *const vector_ptr) {
    // check for nullptr
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR,);

    // free internal array
    vector_ptr->_allocator_ptr->free(vector_ptr->data_ptr);
    RETURN(SIM_RC_SUCCESS,);
}

// sim_vector_is_empty(1):  Checks if a vector is empty.
bool sim_vector_is_empty(Sim_Vector *const vector_ptr) {
    // check for nullptr
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR, false);

    RETURN(SIM_RC_SUCCESS, vector_ptr->count == 0);
}

// sim_vector_clear(1): Clears a vector of all its contents.
void sim_vector_clear(Sim_Vector *const vector_ptr) {
    // check for nullptr
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR,);

    // free array & set count to 0
    vector_ptr->_allocator_ptr->free(vector_ptr->data_ptr);
    vector_ptr->data_ptr = NULL;
    vector_ptr->_allocated = 0;
    vector_ptr->count = 0;

    RETURN(SIM_RC_SUCCESS,);
}

// sim_vector_resize(2): Resizes a vector to a given size.
void sim_vector_resize(
    Sim_Vector *const vector_ptr,
    const size_t size
) {
    // check for nullptr
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR,);

    // skip if allocated & size are identical
    RETURN_IF(size == vector_ptr->_allocated, SIM_RC_SUCCESS,);

    // error if size < count
    RETURN_IF(size <vector_ptr->count, SIM_RC_ERR_INVALARG,);

    // calculate how many bytes to allocate for internal array
    size_t bytes_to_alloc =
        (size > SIM_DEFAULT_VECTOR_SIZE ?
            size :
            SIM_DEFAULT_VECTOR_SIZE
        ) * vector_ptr->_item_size
    ;
    
    // realloc new internal array or malloc new one if NULL
    void* data_ptr;
    if (vector_ptr->data_ptr)
        data_ptr = vector_ptr->_allocator_ptr->realloc(vector_ptr->data_ptr, bytes_to_alloc);
    else
        data_ptr = vector_ptr->_allocator_ptr->malloc(bytes_to_alloc);

    RETURN_IF(!data_ptr, SIM_RC_ERR_OUTOFMEM,);
    
    // reassign data_ptr and _allocated
    vector_ptr->data_ptr = data_ptr;
    vector_ptr->_allocated = size;

    RETURN(SIM_RC_SUCCESS,);
}

// sim_vector_get_ptr(2):  Get pointer to data in a vector at a given index.
void* sim_vector_get_ptr(
    Sim_Vector *const vector_ptr,
    const size_t      index
) {
    // check for nullptr
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR, NULL);

    // check for out-of-bounds index
    RETURN_IF(index >= vector_ptr->count, SIM_RC_ERR_OUTOFBND, NULL);

    const size_t item_size = vector_ptr->_item_size;

    // set data_out_ptr to pointer to data if provided
    RETURN(SIM_RC_SUCCESS, (uint8*)vector_ptr->data_ptr + (item_size * index));
}

// sim_vector_get(3): Get an item from a vector at a given index.
void sim_vector_get(
    Sim_Vector *const vector_ptr,
    const size_t      index,
    void*             data_out_ptr
) {
    void* data_ptr = sim_vector_get_ptr(vector_ptr, index);
    if (data_ptr)
        memcpy(data_out_ptr, data_ptr, vector_ptr->_item_size);
}

// sim_vector_index_of(4): Get the index of the first item in a vector that tests equal to given
//                         data.
size_t sim_vector_index_of(
    Sim_Vector *const vector_ptr,
    const void *const item_ptr,
    Sim_PredicateProc predicate_proc,
    const size_t      starting_index
) {
    // check for nullptrs
    RETURN_IF(
        !vector_ptr || !item_ptr || !predicate_proc,
        SIM_RC_ERR_NULLPTR,
        (size_t)-1
    );

    // check for out-of-bounds starting index
    RETURN_IF(starting_index >= vector_ptr->count, SIM_RC_ERR_OUTOFBND, (size_t)-1);
    
    uint8*  data_ptr = (uint8*)vector_ptr->data_ptr;
    size_t count = vector_ptr->count;
    const size_t item_size = vector_ptr->_item_size;

    // iterate over indexes
    for (size_t i = starting_index; i < count; i++) {
        // check if item & comparison_data compare as equal
        if ((*predicate_proc)(data_ptr, item_ptr))
            RETURN(SIM_RC_SUCCESS, i);

        // iterate data_ptr forward one item
        data_ptr += item_size;
    }
    
    RETURN(SIM_RC_ERR_NOTFOUND, (size_t)-1);
}

// sim_vector_contains(3): Checks if an item is contained in the vector.
bool sim_vector_contains(
    Sim_Vector *const vector_ptr,
    const void *const item_ptr,
    Sim_PredicateProc predicate_proc
) {
    return sim_vector_index_of(
        vector_ptr,
        item_ptr,
        predicate_proc,
        0
    ) != (size_t)-1;
}

// sim_vector_push(2): Push a new item to the back of the vector.
void sim_vector_push(
    Sim_Vector *const vector_ptr,
    const void*       new_item_ptr
) {
    sim_vector_insert(vector_ptr, new_item_ptr, vector_ptr ? vector_ptr->count : 0);
}

// sim_vector_insert(3):  Insert a new item into the vector at a particular index.
void sim_vector_insert(
    Sim_Vector *const vector_ptr,
    const void*       new_item_ptr,
    const size_t      index
) {
    // check for nullptrs
    RETURN_IF(!vector_ptr || !new_item_ptr, SIM_RC_ERR_NULLPTR,);

    // check for out-of-bounds index
    RETURN_IF(index > vector_ptr->count, SIM_RC_ERR_OUTOFBND,);

    // reallocate data_ptr if cleared
    if (!vector_ptr->data_ptr) {
        sim_vector_resize(vector_ptr, SIM_DEFAULT_VECTOR_SIZE);
        if (_SIM_RETURN_CODE)
            return;
    }

    const size_t item_size = vector_ptr->_item_size;
    uint8*  data_ptr = vector_ptr->data_ptr;
    
    // resize vector if full
    if (vector_ptr->count == vector_ptr->_allocated) {
        // check for overflow
        RETURN_IF(
            item_size * vector_ptr->_allocated * 2 < item_size * vector_ptr->_allocated,
            SIM_RC_ERR_OUTOFMEM,
        );

        data_ptr = vector_ptr->_allocator_ptr->realloc(
            data_ptr,
            item_size * vector_ptr->_allocated * 2
        );

        // check if resize failed
        RETURN_IF(!data_ptr, SIM_RC_ERR_OUTOFMEM,);

        vector_ptr->_allocated *= 2;
        vector_ptr->data_ptr = data_ptr;
    }

    // pointer to insert new item into
    uint8* insert_ptr = data_ptr + (item_size * index);

    // move items ahead of index forward
    memmove(
        insert_ptr + item_size,
        insert_ptr,
        item_size * (vector_ptr->count - index)
    );

    // insert new item
    memcpy(insert_ptr, new_item_ptr, item_size);

    // increment data count
    vector_ptr->count++;
    RETURN(SIM_RC_SUCCESS,);
}

// sim_vector_pop(2): Pops an item off the back of a vector.
void sim_vector_pop(
    Sim_Vector *const vector_ptr,
    void*             data_out_ptr
) {
    sim_vector_remove(vector_ptr, data_out_ptr, vector_ptr ? vector_ptr->count - 1 : 0);
}

// sim_vector_remove(3): Removes an item from a vector at a given index.
void sim_vector_remove(
    Sim_Vector *const vector_ptr,
    void*             data_out_ptr,
    const size_t      index
) {
    // check for nullptr
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR,);

    // check for out-of-bounds index
    RETURN_IF(index >= vector_ptr->count, SIM_RC_ERR_OUTOFBND,);
    
    size_t item_size  = vector_ptr->_item_size;
    uint8*  remove_ptr = (uint8*)vector_ptr->data_ptr + (item_size * index);

    // fill data_out_ptr with removed item if provided
    if (data_out_ptr)
        memcpy(data_out_ptr, remove_ptr, item_size);

    // move items ahead of index backward
    memmove(
        remove_ptr,
        remove_ptr + item_size,
        item_size * (vector_ptr->count - index)
    );

    // decrement count
    vector_ptr->count--;

    // resize down if able to
    if (
        vector_ptr->count <= vector_ptr->_allocated / 4 &&
        vector_ptr->_allocated > SIM_DEFAULT_VECTOR_SIZE
    ) {
        size_t new_size = (vector_ptr->_allocated / 2) > SIM_DEFAULT_VECTOR_SIZE ?
            (vector_ptr->_allocated / 2) :
            SIM_DEFAULT_VECTOR_SIZE
        ;

        void* data_ptr = vector_ptr->_allocator_ptr->realloc(
            vector_ptr->data_ptr,
            new_size * item_size
        );
        vector_ptr->_allocated = new_size;

        // use new array if successfully reallocated; else use old array
        if (data_ptr) vector_ptr->data_ptr = data_ptr;
    }

    RETURN(SIM_RC_SUCCESS,);
}

// sim_vector_foreach(3): Applies a given function for each item in the vector.
bool sim_vector_foreach(
    Sim_Vector *const vector_ptr,
    Sim_ForEachProc   foreach_proc,
    Sim_Variant       userdata
) {
    // check for nullptrs
    RETURN_IF(!vector_ptr || !foreach_proc, SIM_RC_ERR_NULLPTR, false);

    size_t count = vector_ptr->count;
    uint8*  data_ptr   = vector_ptr->data_ptr;
    size_t item_size  = vector_ptr->_item_size;

    for (size_t i = 0; i < count; i++) {
        if (!(*foreach_proc)(data_ptr, i, userdata))
            RETURN(SIM_RC_SUCCESS, false);
        
        data_ptr += item_size;
    }

    RETURN(SIM_RC_SUCCESS, true);
}

void _sim_vector_filter(
    Sim_Vector *const vector_ptr,
    Sim_FilterProc    filter_proc,
    Sim_Variant       userdata,
    Sim_Vector *const out_vector_ptr,
    bool              remove
) {
    // check for nullptr
    RETURN_IF(!vector_ptr || !filter_proc, SIM_RC_ERR_NULLPTR,);

    size_t count   = vector_ptr->count;
    uint8*  data_ptr     = vector_ptr->data_ptr;
    uint8*  reinsert_ptr = vector_ptr->data_ptr;
    size_t item_size    = vector_ptr->_item_size;

    // iterate over items in vector
    for (size_t i = 0; i < count; i++) {
        bool result = !(*filter_proc)(data_ptr, userdata);

        if (result) {
            if (remove) {
                reinsert_ptr -= item_size;
                vector_ptr->count--;
            }

            // move removed item to out_vector if provided
            if (out_vector_ptr)
                sim_vector_push(out_vector_ptr, data_ptr);
        } else if (data_ptr != reinsert_ptr)
            memcpy(reinsert_ptr, data_ptr, item_size);
        
        reinsert_ptr += item_size;
        data_ptr += item_size;
    }

    RETURN(SIM_RC_SUCCESS,);
}

// sim_vector_extract(4): Extracts items out of the vector based on a given function.
void sim_vector_extract(
    Sim_Vector *const vector_ptr,
    Sim_FilterProc    filter_proc,
    Sim_Variant       userdata,
    Sim_Vector *const out_vector_ptr
) {
    _sim_vector_filter(vector_ptr, filter_proc, userdata, out_vector_ptr, true);
}

// sim_vector_select(4): Selects items from the vector based on a given function.
void sim_vector_select(
    Sim_Vector *const vector_ptr,
    Sim_FilterProc    select_proc,
    Sim_Variant       userdata,
    Sim_Vector *const out_vector_ptr
) { 
    RETURN_IF(!out_vector_ptr, SIM_RC_ERR_NULLPTR,);
    _sim_vector_filter(vector_ptr, select_proc, userdata, out_vector_ptr, false);
}

#endif /* SIMSOFT_VECTOR_C_ */
