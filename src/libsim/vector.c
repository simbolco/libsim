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

// sim_vector_construct(5): Constructs a new vector.
void sim_vector_construct(
    Sim_Vector*           vector_ptr,
    const size_t          item_size,
    const Sim_DataType    item_type,
    const Sim_IAllocator* allocator_ptr,
    size_t                initial_size
) {
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR,);

    if (!initial_size)
        initial_size = SIM_DEFAULT_VECTOR_SIZE;
        
    // use default allocator on NULL
    if (!allocator_ptr)
        allocator_ptr = sim_get_default_allocator();

    // allocate internal array
    void* data_ptr = allocator_ptr->malloc(item_size * initial_size);
    RETURN_IF(!data_ptr, SIM_RC_ERR_OUTOFMEM,);
    
    // set up initial properties of Sim_Vector
    Sim_Vector vec = {
        ._item_properties = {
            .type = item_type,
            .size = item_size
        },
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
void sim_vector_destroy(
    Sim_Vector *const vector_ptr
) {
    // check for nullptr
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR,);

    // free internal array
    vector_ptr->_allocator_ptr->free(vector_ptr->data_ptr);
    RETURN(SIM_RC_SUCCESS,);
}

// sim_vector_clear(1): Clears a vector of all its contents.
void sim_vector_clear(
    Sim_Vector *const vector_ptr
) {
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
        ) * vector_ptr->_item_properties.size
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

    const size_t item_size = vector_ptr->_item_properties.size;

    // set data_out_ptr to pointer to data if provided
    RETURN(SIM_RC_SUCCESS, (byte*)vector_ptr->data_ptr + (item_size * index));
}

// sim_vector_get(3): Get an item from a vector at a given index.
void sim_vector_get(
    Sim_Vector *const vector_ptr,
    const size_t      index,
    void*             data_out_ptr
) {
    void* data_ptr = sim_vector_get_ptr(vector_ptr, index);
    if (data_ptr)
        memcpy(data_out_ptr, data_ptr, vector_ptr->_item_properties.size);
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
    
    byte*  data_ptr = (byte*)vector_ptr->data_ptr;
    size_t count = vector_ptr->count;
    const size_t item_size = vector_ptr->_item_properties.size;

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

    const size_t item_size = vector_ptr->_item_properties.size;
    byte*  data_ptr = vector_ptr->data_ptr;
    
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
    byte* insert_ptr = data_ptr + (item_size * index);

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
    
    size_t item_size  = vector_ptr->_item_properties.size;
    byte*  remove_ptr = (byte*)vector_ptr->data_ptr + (item_size * index);

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
    byte*  data_ptr   = vector_ptr->data_ptr;
    size_t item_size  = vector_ptr->_item_properties.size;

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
    byte*  data_ptr     = vector_ptr->data_ptr;
    byte*  reinsert_ptr = vector_ptr->data_ptr;
    size_t item_size    = vector_ptr->_item_properties.size;

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


#define RADIX_GET_MAX(TYPE)                   \
    inline TYPE _sim_vector_get_max_ ##TYPE ( \
        TYPE arr[],                           \
        size_t n                              \
    ) {                                       \
        TYPE mx = arr[0];                     \
        for (size_t i = 1; i < n; i++)        \
            if (arr[i] > mx)                  \
                mx = arr[i];                  \
        return mx;                            \
}

#define RADIX_COUNT_SORT(TYPE)                            \
    inline void _sim_vector_count_sort_ ##TYPE (          \
        TYPE arr[],                                       \
        size_t n,                                         \
        TYPE exp                                          \
    ) {                                                   \
        TYPE out[n]; /* output array */                   \
        size_t i;                                         \
        size_t count[10] = {0};                           \
                                                          \
        /* store count of occurences in count[] */        \
        for (i = 0; i < n; i++)                           \
            count[ (arr[i]/exp) % 10 ]++;                 \
                                                          \
        for (i = 1; i < 10; i++)                          \
            count[i] += count[i - 1];                     \
                                                          \
        /* Build output array */                          \
        for (i = n - 1; i != (size_t)-1; i--) {           \
            out[count[ (arr[i]/exp) % 10 ] - 1] = arr[i]; \
            count[ (arr[i]/exp) % 10 ]--;                 \
        }                                                 \
                                                          \
        /* Copy out[] to arr[] */                         \
        memcpy(arr, out, sizeof(TYPE) * n);               \
}

#define RADIX_FN_SORT(TYPE)                               \
    inline void _sim_vector_radix_sort_ ##TYPE (          \
        TYPE arr[],                                       \
        size_t n                                          \
    ) {                                                   \
        /* Find max number to know of digits */           \
        TYPE m = _sim_vector_get_max_ ##TYPE (arr, n);    \
                                                          \
        /* Do counting sort for every digit */            \
        for (TYPE exp = 1; m / exp > 0; exp *= 10)        \
            _sim_vector_count_sort_ ##TYPE (arr, n, exp); \
}

#define RADIX_SORT(TYPE)   \
    RADIX_GET_MAX(TYPE)    \
    RADIX_COUNT_SORT(TYPE) \
    RADIX_FN_SORT(TYPE)

// unsigned integral radix sort functions
RADIX_SORT(uint8)
RADIX_SORT(uint16)
RADIX_SORT(uint32)
RADIX_SORT(uint64)

// 32-bit floating point radix sort functions
RADIX_GET_MAX(float)
inline void _sim_vector_count_sort_float (
    float arr[],
    size_t n,
    float exp
) {
    float out[n]; /* output array */
    size_t i;
    size_t count[10] = {0.0f};

    /* store count of occurences in count[] */
    for (i = 0; i < n; i++)
        count[ (size_t)floorf(fabsf(fmodf((arr[0]/exp), 10.0f))) ]++;

    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    /* Build output array */
    for (i = n - 1; i != (size_t)-1; i--) {
        out[count[ (size_t)floorf(fabsf(fmodf((arr[0]/exp), 10.0f))) ] - 1] = arr[i];
        count[ (size_t)floorf(fabsf(fmodf((arr[0]/exp), 10.0f))) ]--;
    }

    /* Copy out[] to arr[] */
    memcpy(arr, out, sizeof(float) * n);
}
RADIX_FN_SORT(float)

// 64-bit floating point radix sort functions
RADIX_GET_MAX(double)
inline void _sim_vector_count_sort_double (
    double arr[],
    size_t n,
    double exp
) {
    double out[n]; /* output array */
    size_t i;
    size_t count[10] = {0.0};

    /* store count of occurences in count[] */
    for (i = 0; i < n; i++)
        count[ (size_t)floor(fabs(fmod((arr[i]/exp), 10.0))) ]++;

    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    /* Build output array */
    for (i = n - 1; i != (size_t)-1; i--) {
        out[count[ (size_t)floor(fabs(fmod((arr[i]/exp), 10.0))) ] - 1] = arr[i];
        count[ (size_t)floor(fabs(fmod((arr[i]/exp), 10.0))) ]--;
    }

    /* Copy out[] to arr[] */
    memcpy(arr, out, sizeof(double) * n);
}
RADIX_FN_SORT(double)

#define RADIX_SORT_VECTOR(TYPE, vector_ptr) \
    _sim_vector_radix_sort_ ##TYPE (vector_ptr->data_ptr, vector_ptr->count)
#define QUICK_SORT_VECTOR(vector_ptr, cmp_func) \
    qsort( \
        vector_ptr->data_ptr, \
        vector_ptr->count, \
        vector_ptr->_item_properties.size, \
        (int (*)(const void*, const void*)) cmp_func \
    )

#define SIGNED_RADIX_XOR(bitsize, vector_ptr, n) \
    for (size_t i = 0; i < count; i++)        \
        ((sint ##bitsize *)(vector_ptr->data_ptr))[i] ^= n

// sim_vector_sort(2): Sorts items in the vector based on initialization settings or a
//                     user-provided comparison function.
void sim_vector_sort(
    Sim_Vector *const  vector_ptr,
    Sim_ComparisonProc comparison_proc
) {
    // check for nullptr
    RETURN_IF(!vector_ptr, SIM_RC_ERR_NULLPTR,);

    // skip 0-sized vectors
    RETURN_IF(vector_ptr->count == 0, SIM_RC_SUCCESS,);

    if (comparison_proc) {
        QUICK_SORT_VECTOR(vector_ptr, comparison_proc);
        RETURN(SIM_RC_SUCCESS,);
    }

    size_t count = vector_ptr->count;

    switch (vector_ptr->_item_properties.type) {
    case SIM_DATATYPE_INTEGRAL:
        switch (vector_ptr->_item_properties.size) {
        // 8-bit signed
        case 1:
            SIGNED_RADIX_XOR(8, vector_ptr, -128);
            RADIX_SORT_VECTOR(uint8, vector_ptr);
            SIGNED_RADIX_XOR(8, vector_ptr, -128);
            break;

        // 16-bit signed
        case 2:
            SIGNED_RADIX_XOR(16, vector_ptr, -32768);
            RADIX_SORT_VECTOR(uint16, vector_ptr);
            SIGNED_RADIX_XOR(16, vector_ptr, -32768);
            break;

        // 32-bit signed
        case 4:
            SIGNED_RADIX_XOR(32, vector_ptr, -2147483648);
            RADIX_SORT_VECTOR(uint32, vector_ptr);
            SIGNED_RADIX_XOR(32, vector_ptr, -2147483648);
            break;

        // 64-bit signed
        case 8:
            SIGNED_RADIX_XOR(64, vector_ptr, -2147483648LL);
            RADIX_SORT_VECTOR(uint64, vector_ptr);
            SIGNED_RADIX_XOR(64, vector_ptr, -2147483648LL);
            break;

        default:
            break;
        }
        break;

    case SIM_DATATYPE_UNSIGNED:
        switch (vector_ptr->_item_properties.size) {
        // 8-bit unsigned
        case 1:
            RADIX_SORT_VECTOR(uint8, vector_ptr);
            break;

        // 16-bit unsigned
        case 2:
            RADIX_SORT_VECTOR(uint16, vector_ptr);
            break;

        // 32-bit unsigned
        case 4:
            RADIX_SORT_VECTOR(uint32, vector_ptr);
            break;

        // 64-bit unsigned
        case 8:
            RADIX_SORT_VECTOR(uint64, vector_ptr);
            break;

        default:
            break;
        }
        break;

    case SIM_DATATYPE_FLOAT:
        switch (vector_ptr->_item_properties.size) {
        // 32-bit floating point
        case 4:
            RADIX_SORT_VECTOR(float, vector_ptr);
            break;

        // 64-bit floating point
        case 8:
            RADIX_SORT_VECTOR(double, vector_ptr);
            break;

        default:
            break;
        }
        break;

    case SIM_DATATYPE_OTHER:
        break;
    }
    
    RETURN(SIM_RC_ERR_NULLPTR,);
}

#undef RADIX_FN_SORT
#undef RADIX_COUNT_SORT
#undef RADIX_GET_MAX
#undef RADIX_SORT_VECTOR
#undef QUICK_SORT_VECTOR
#undef SIGNED_RADIX_XOR

#endif /* SIMSOFT_VECTOR_C_ */
