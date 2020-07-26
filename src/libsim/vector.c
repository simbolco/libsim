/**
 * @file vector.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/vector.h
 * @version 0.2
 * @date 2020-07-25
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_VECTOR_C_
#define SIMSOFT_VECTOR_C_

#include "simsoft/vector.h"

#include "simsoft/except.h"

#include "./_internal.h"

// sim_vector_construct(4): Constructs a new vector.
void sim_vector_construct(
    Sim_Vector *const   vector_ptr,
    const size_t        item_size,
    Sim_IAllocator*     allocator_ptr,
    size_t              initial_size
) {
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    if (!initial_size)
        initial_size = SIM_DEFAULT_VECTOR_SIZE;
        
    // use default allocator on NULL
    if (!allocator_ptr)
        allocator_ptr = sim_allocator_get_default();

    // allocate internal array
    void* data_ptr = VPROC_CALL(allocator_ptr, alloc, item_size * initial_size);
    if (!data_ptr)
        THROW(SIM_ERR_OUTOFMEM, "(%s) Failed to allocate vector array", FUNCTION_NAME);

    // assign unchanging properties
    memcpy(
        (uint8*)vector_ptr + offsetof(Sim_Vector, _item_size),
        &item_size,
        sizeof item_size
    );
    memcpy(
        (uint8*)vector_ptr + offsetof(Sim_Vector, _allocator_ptr),
        &allocator_ptr,
        sizeof allocator_ptr
    );

    // assign properties
    vector_ptr->_allocated = initial_size;
    vector_ptr->count = 0;
    vector_ptr->data_ptr = data_ptr;
}

// sim_vector_destroy(1): Destroys a vector.
void sim_vector_destroy(Sim_Vector *const vector_ptr) {
    // check for nullptr
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    // free internal array
    VPROC_CALL(vector_ptr->_allocator_ptr, free, vector_ptr->data_ptr);
}

// sim_vector_is_empty(1):  Checks if a vector is empty.
bool sim_vector_is_empty(Sim_Vector *const vector_ptr) {
    // check for nullptr
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    return vector_ptr->count == 0;
}

// sim_vector_clear(1): Clears a vector of all its contents.
void sim_vector_clear(Sim_Vector *const vector_ptr) {
    // check for nullptr
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    // free array & set count to 0
    VPROC_CALL(vector_ptr->_allocator_ptr, free, vector_ptr->data_ptr);
    vector_ptr->data_ptr = NULL;
    vector_ptr->_allocated = 0;
    vector_ptr->count = 0;
}

// sim_vector_resize(2): Resizes a vector to a given size.
void sim_vector_resize(
    Sim_Vector *const vector_ptr,
    const size_t size
) {
    // check for nullptr
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    // skip if allocated & size are identical
    if (size == vector_ptr->_allocated)
        return;

    // error if size < count
    if (size < vector_ptr->count)
        THROW(SIM_ERR_INVALARG, "(%s) Cannot resize vector to less than count", FUNCTION_NAME);

    // calculate how many bytes to allocate for internal array
    size_t bytes_to_alloc =
        (size > SIM_DEFAULT_VECTOR_SIZE ?
            size :
            SIM_DEFAULT_VECTOR_SIZE
        ) * vector_ptr->_item_size
    ;
    
    // realloc new internal array or malloc new one if NULL
    void* data_ptr = VPROC_CALL(vector_ptr->_allocator_ptr, resize,
        vector_ptr->data_ptr,
        bytes_to_alloc
    );
    if (!data_ptr)
        THROW(SIM_ERR_OUTOFMEM, "(%s) Failed to resize Vector@%p", FUNCTION_NAME, vector_ptr);
    
    // reassign data_ptr and _allocated
    vector_ptr->data_ptr = data_ptr;
    vector_ptr->_allocated = size;
}

// sim_vector_get_ptr(2):  Get pointer to data in a vector at a given index.
void* sim_vector_get_ptr(
    Sim_Vector *const vector_ptr,
    const size_t      index
) {
    // check for nullptr
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    // check for out-of-bounds index
    if (index >= vector_ptr->count)
        THROW(SIM_ERR_OUTOFBND, "(%s) Vector index out of bounds", FUNCTION_NAME);

    const size_t item_size = vector_ptr->_item_size;

    // set data_out_ptr to pointer to data if provided
    return (uint8*)vector_ptr->data_ptr + (item_size * index);
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

// sim_vector_find(4): Find the index of the first item in a vector that tests equal to given data.
size_t sim_vector_find(
    Sim_Vector *const vector_ptr,
    const void *const item_ptr,
    Sim_PredicateProc predicate_proc,
    const size_t      starting_index
) {
    // check for nullptrs
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    if (!item_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);
    if (!predicate_proc)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 2 is NULL", FUNCTION_NAME);

    // check for out-of-bounds starting index
    if (starting_index >= vector_ptr->count)
        THROW(SIM_ERR_OUTOFBND, "(%s) Vector index out of bounds", FUNCTION_NAME);
    
    uint8*  data_ptr = (uint8*)vector_ptr->data_ptr;
    size_t count = vector_ptr->count;
    const size_t item_size = vector_ptr->_item_size;

    // iterate over indexes
    for (size_t i = starting_index; i < count; i++) {
        // check if item & comparison_data compare as equal
        if ((*predicate_proc)(data_ptr, item_ptr))
            return i;

        // iterate data_ptr forward one item
        data_ptr += item_size;
    }
    
    return (size_t)-1;
}

// sim_vector_contains(3): Checks if an item is contained in the vector.
bool sim_vector_contains(
    Sim_Vector *const vector_ptr,
    const void *const item_ptr,
    Sim_PredicateProc predicate_proc
) {
    return sim_vector_find(
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

// sim_vector_insert(3): Insert a new item into the vector at a particular index.
void sim_vector_insert(
    Sim_Vector *const vector_ptr,
    const void*       new_item_ptr,
    const size_t      index
) {
    // check for nullptrs
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    if (!new_item_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);

    // check for out-of-bounds index
    if (index > vector_ptr->count)
        THROW(SIM_ERR_OUTOFBND, "(%s) Vector index out of range", FUNCTION_NAME);

    // reallocate data_ptr if cleared
    if (!vector_ptr->data_ptr)
        sim_vector_resize(vector_ptr, SIM_DEFAULT_VECTOR_SIZE);

    const size_t item_size = vector_ptr->_item_size;
    uint8* data_ptr = vector_ptr->data_ptr;
    
    // resize vector if full
    if (vector_ptr->count == vector_ptr->_allocated) {
        // check for overflow
        if (item_size * vector_ptr->_allocated * 2 < item_size * vector_ptr->_allocated)
            THROW(SIM_ERR_OUTOFMEM, "(%s) Failed to resize Vector@%p", FUNCTION_NAME, vector_ptr);

        data_ptr = VPROC_CALL(vector_ptr->_allocator_ptr, resize,
            data_ptr,
            item_size * vector_ptr->_allocated * 2
        );

        // check if resize failed
        if (!data_ptr)
            THROW(SIM_ERR_OUTOFMEM, "(%s) Failed to resize Vector@%p", FUNCTION_NAME, vector_ptr);

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
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    // check for out-of-bounds index
    if (index >= vector_ptr->count)
        THROW(SIM_ERR_OUTOFBND, "(%s) Vector index out of range", FUNCTION_NAME);
    
    size_t item_size  = vector_ptr->_item_size;
    uint8* remove_ptr = (uint8*)vector_ptr->data_ptr + (item_size * index);

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
    if (vector_ptr->count <= vector_ptr->_allocated / 4 &&
        vector_ptr->_allocated > SIM_DEFAULT_VECTOR_SIZE
    ) {
        size_t new_size = (vector_ptr->_allocated / 2) > SIM_DEFAULT_VECTOR_SIZE ?
            (vector_ptr->_allocated / 2) :
            SIM_DEFAULT_VECTOR_SIZE
        ;

        void* data_ptr = VPROC_CALL(vector_ptr->_allocator_ptr, resize,
            vector_ptr->data_ptr,
            new_size * item_size
        );
        vector_ptr->_allocated = new_size;

        // use new array if successfully reallocated; else use old array
        if (data_ptr)
            vector_ptr->data_ptr = data_ptr;
    }
}

// sim_vector_foreach(3): Applies a given function for each item in the vector.
bool sim_vector_foreach(
    Sim_Vector *const vector_ptr,
    Sim_ForEachProc   foreach_proc,
    Sim_Variant       userdata
) {
    // check for nullptrs
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    if (!foreach_proc)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);

    size_t count = vector_ptr->count;
    uint8*  data_ptr   = vector_ptr->data_ptr;
    size_t item_size  = vector_ptr->_item_size;

    for (size_t i = 0; i < count; i++) {
        if (!(*foreach_proc)(data_ptr, i, userdata))
            return false;
        
        data_ptr += item_size;
    }

    return true;
}

// filter-extract combination helper function
static void _sim_vector_filter(
    Sim_Vector *const vector_ptr,
    Sim_FilterProc    filter_proc,
    Sim_Variant       userdata,
    Sim_Vector *const out_vector_ptr,
    bool              remove
) {
    // check for nullptr
    if (!vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    if (!filter_proc)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);

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
    if (!out_vector_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 3 is NULL", FUNCTION_NAME);
    _sim_vector_filter(vector_ptr, select_proc, userdata, out_vector_ptr, false);
}

#endif // SIMSOFT_VECTOR_C_
