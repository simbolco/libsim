/**
 * @file hash.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/hashset.h & simsoft/hashmap.h
 * @version 0.1
 * @date 2020-01-10
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_HASH_C_
#define SIMSOFT_HASH_C_

#include "simsoft/hashmap.h"
#include "simsoft/hashset.h"
#include "simsoft/util.h"
#include "./_internal.h"

// Hashmap/hashset aliasing to allow for identical internal implementation
typedef union _Sim_HashPtr {
    Sim_HashMap *const hashmap_ptr;
    Sim_HashSet *const hashset_ptr;
} _Sim_HashPtr;

typedef union _Sim_HashForEachProc {
    Sim_ConstForEachProc set_foreach_proc;
    Sim_MapForEachProc   map_foreach_proc;
} _Sim_HashForEachProc;

// == INTERNAL IMPLEMENTATION FUNCTIONS ===========================================================

// Creates a new hash table node.
static void* _sim_hash_create_node(
    const void*                 key_ptr,
    const size_t                key_size,
    const void*                 val_ptr,
    const size_t                val_size,
    const Sim_IAllocator *const allocator_ptr
) {
    uint8* node_ptr = allocator_ptr->malloc(key_size + val_size);
    if (!node_ptr)
        return NULL;

    // copy key to front of node
    memcpy(node_ptr, key_ptr, key_size);
    // copy value to back of node
    memcpy(node_ptr + key_size, val_ptr, val_size);

    return node_ptr;
}

// Destroys a hash table node.
inline void _sim_hash_destroy_node(
    void*                       node_ptr,
    const Sim_IAllocator *const allocator_ptr
) {
    allocator_ptr->free(node_ptr);
}

// Initializes a hash table (map or set).
static void _sim_hash_construct(
    _Sim_HashPtr          hash_ptr,
    const size_t          key_size,
    Sim_HashProc          key_hash_proc,
    Sim_PredicateProc     key_predicate_proc,
    const size_t          value_size,
    const Sim_IAllocator* allocator_ptr,
    const size_t          initial_size
) {
    // check for nullptr
    if (!hash_ptr.hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!key_predicate_proc)
        THROW(SIM_RC_ERR_NULLPTR);
        
    allocator_ptr = allocator_ptr ? allocator_ptr : sim_allocator_get_default();

    // allocate buckets
    size_t starting_size = _sim_next_prime(initial_size);
    if (starting_size < SIM_HASH_DEFAULT_SIZE)
        starting_size = SIM_HASH_DEFAULT_SIZE;
    void** data_ptr = allocator_ptr->falloc(
        (sizeof *data_ptr) * starting_size,
        0
    );
    if (!data_ptr)
        THROW(SIM_RC_ERR_OUTOFMEM);
        
    Sim_HashMap hashmap = {
        ._key_properties = {
            .size = key_size,
            .hash_proc = key_hash_proc,
            .predicate_proc = key_predicate_proc
        },
        ._allocator_ptr = allocator_ptr,
        ._initial_size = initial_size,
        ._base_size = initial_size,
        ._allocated = starting_size,

        .count = 0,
        .data_ptr = (void*)data_ptr,

        ._value_size = value_size
    };

    // copy to hash table pointer
    memcpy(
        hash_ptr.hashmap_ptr,
        &hashmap,
        (value_size > 0) ?
            sizeof(Sim_HashMap) :
            sizeof(Sim_HashSet)
    );

    RETURN(SIM_RC_SUCCESS,);
}

// Clears a hash table.
static void _sim_hash_clear(
    _Sim_HashPtr hash_ptr
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    if (!hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
        
    // destroy hash table nodes
    void** item_ptr = hashmap_ptr->data_ptr;
    for (size_t i = 0; i < hashmap_ptr->_allocated; i++)
        if (item_ptr[i] && item_ptr[i] != (void*)1)
            _sim_hash_destroy_node(item_ptr[i], hashmap_ptr->_allocator_ptr);
    
    // zero out hash table data
    memset(hashmap_ptr->data_ptr, 0, (sizeof *item_ptr) * hashmap_ptr->_allocated);

    // reset count
    hashmap_ptr->count = 0;

    RETURN(SIM_RC_SUCCESS,);
}

// Destroys a hash table.
static void _sim_hash_destroy(
    _Sim_HashPtr hash_ptr
) {
    _sim_hash_clear(hash_ptr);
    
    THROW(sim_get_return_code());
    if (sim_get_return_code() > 0)
        RETURN(sim_get_return_code(),);
    
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    // free bucket array when destroying
    hashmap_ptr->_allocator_ptr->free(hashmap_ptr->data_ptr);

    RETURN(SIM_RC_SUCCESS,);
}

#define HASH_TABLE_IF_CONTAIN(                                                         \
    key_ptr,                                                                           \
    key_size,                                                                          \
    hash_data_ptr,                                                                     \
    allocated,                                                                         \
    hash_proc,                                                                         \
    pred_proc,                                                                         \
    ...                                                                                \
)                                                                                      \
                                                                                       \
    uint8** data_ptr = (uint8**)hash_data_ptr;                                         \
    size_t attempt = 0;                                                                \
    uint8* current_node_ptr;                                                           \
    size_t index;                                                                      \
                                                                                       \
    size_t hash = (hash_proc ?                                                         \
        (*hash_proc)(key_ptr, 0) :                                                     \
        sim_siphash(key_ptr, key_size, (Sim_HashKey){SIPHASH_KEY1, SIPHASH_KEY2})      \
    ) % allocated;                                                                     \
    size_t hash2 = 0;                                                                  \
                                                                                       \
    index = hash % allocated;                                                          \
    current_node_ptr = data_ptr[index];                                                \
                                                                                       \
    while (current_node_ptr) {                                                         \
        if (current_node_ptr != (void*)1 && (*pred_proc)(key_ptr, current_node_ptr)) { \
            UNPACK(__VA_ARGS__);                                                       \
        }                                                                              \
                                                                                       \
        attempt++;                                                                     \
        if (hash_proc)                                                                 \
            hash = (*hash_proc)(key_ptr, attempt);                                     \
        else {                                                                         \
            if (attempt == 1)                                                          \
                hash2 = sim_siphash(                                                   \
                    key_ptr,                                                           \
                    key_size,                                                          \
                    (Sim_HashKey){SIPHASH_KEY3, SIPHASH_KEY4}                          \
                ) % allocated;                                                         \
            hash += hash2 + attempt;                                                   \
        }                                                                              \
        index = hash % allocated;                                                      \
        current_node_ptr = data_ptr[index];                                            \
    }

#define HASH_IF_CONTAIN(hashmap_ptr, key_ptr, ...)   \
    HASH_TABLE_IF_CONTAIN(                           \
        key_ptr,                                     \
        hashmap_ptr->_key_properties.size,           \
        hashmap_ptr->data_ptr,                       \
        hashmap_ptr->_allocated,                     \
        hashmap_ptr->_key_properties.hash_proc,      \
        hashmap_ptr->_key_properties.predicate_proc, \
        __VA_ARGS__                                  \
    )

// resize hash table to new size
static void _sim_hash_resize(
    _Sim_HashPtr hash_ptr,
    const size_t new_size
) {
    // resize only if larger than or equal to the initial size
    if (new_size > hash_ptr.hashmap_ptr->_initial_size) {
        Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

        // initialize new hash table
        uint8** new_data_ptr = hashmap_ptr->_allocator_ptr->falloc(
            (sizeof *new_data_ptr) * new_size,
            0
        );
        if (!new_data_ptr)
            THROW(SIM_RC_ERR_OUTOFMEM);
        
        // re-hash old data and move them into new hash table
        uint8** hash_data_ptr = hashmap_ptr->data_ptr;
        for (size_t i = 0; i < hashmap_ptr->_allocated; i++) {
            uint8* hash_node = hash_data_ptr[i];
            if (hash_node && hash_node != (void*)1) {
                bool test = false;
                HASH_TABLE_IF_CONTAIN(
                    hash_node,
                    hashmap_ptr->_key_properties.size,
                    new_data_ptr,
                    new_size,
                    hashmap_ptr->_key_properties.hash_proc,
                    hashmap_ptr->_key_properties.predicate_proc,

                    test = true;
                    break;
                );
                if (!test)
                    data_ptr[index] = hash_node;
            }
        }

        // free old array & reassign data_ptr to new array
        hashmap_ptr->data_ptr = new_data_ptr;
        hashmap_ptr->_allocated = new_size;
        hashmap_ptr->_allocator_ptr->free(hash_data_ptr);
    }

    RETURN(SIM_RC_SUCCESS,);
}

void _sim_hash_insert(
    _Sim_HashPtr hash_ptr,
    const void*  key_ptr,
    const void*  value_ptr
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    // check how much of the hash table is used & resize up if necessary
    const size_t load = hashmap_ptr->count * 100 / hashmap_ptr->_allocated;
    if (load > 70) {
        _sim_hash_resize(hash_ptr, hashmap_ptr->_base_size * 2);
        THROW(sim_get_return_code());
        if (sim_get_return_code() > 0)
            RETURN(sim_get_return_code(),);
    }

    HASH_IF_CONTAIN(hashmap_ptr, key_ptr,
        // copy contents of value_ptr to node if hashmap
        if (value_ptr)
            memcpy(
                current_node_ptr + hashmap_ptr->_value_size,
                value_ptr,
                hashmap_ptr->_value_size
            );
        RETURN(SIM_RC_SUCCESS,);
    );

    // insert new item
    void* node = _sim_hash_create_node(
        key_ptr,
        hashmap_ptr->_key_properties.size,
        value_ptr,
        value_ptr ?
            hashmap_ptr->_value_size :
            0
        ,
        hashmap_ptr->_allocator_ptr
    );
    if (!node)
        THROW(SIM_RC_ERR_OUTOFMEM);
    
    data_ptr[index] = node;
    hashmap_ptr->count++;
    RETURN(SIM_RC_SUCCESS,);
}

// Removes an item from a hash table.
static void _sim_hash_remove(
    _Sim_HashPtr hash_ptr,
    const void*  key_ptr
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    // check how much of the hash table is used & resize down if necessary
    const size_t load = hashmap_ptr->count * 100 / hashmap_ptr->_allocated;
    if (load < 10) {
        _sim_hash_resize(hash_ptr, hashmap_ptr->_base_size * 2);
        
        THROW(sim_get_return_code());
        if (sim_get_return_code() > 0)
            RETURN(sim_get_return_code(),);
    }

    HASH_IF_CONTAIN(hashmap_ptr, key_ptr, 
        // destroy node
        _sim_hash_destroy_node(current_node_ptr, hashmap_ptr->_allocator_ptr);
        
        // set data_ptr @ index to deleted node
        data_ptr[index] = (void*)1;

        // decrement count
        hashmap_ptr->count--;

        RETURN(SIM_RC_SUCCESS,);
    );
    
    // failed to remove item; item not in hash table
    RETURN(SIM_RC_FAILURE,);
}

// Checks if an item/key is contained in a hash table.
static bool _sim_hash_contains(
    _Sim_HashPtr hash_ptr,
    const void* key_ptr
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    // check for nullptrs
    if (!hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!key_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    HASH_IF_CONTAIN(hashmap_ptr, key_ptr,
        RETURN(SIM_RC_SUCCESS, true);
    );

    RETURN(SIM_RC_NOT_FOUND, false);
}

// Apply a function for each item in hash table.
static bool _sim_hash_foreach(
    _Sim_HashPtr         hash_ptr,
    const bool           is_hashmap,
    _Sim_HashForEachProc foreach_proc,
    Sim_Variant          userdata
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    // check for nullptrs
    if (!hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!foreach_proc.set_foreach_proc)
        THROW(SIM_RC_ERR_NULLPTR);
    
    uint8** data_ptr = hashmap_ptr->data_ptr;
    size_t item_num = 0;

    // iterate through allocated
    if (is_hashmap) {
        for (size_t i = 0; i < hashmap_ptr->_allocated; i++) {
            // if the item exists...
            if (data_ptr[i] && data_ptr[i] != (void*)1) {
                if (!foreach_proc.map_foreach_proc(
                    data_ptr[i],
                    (uint8*)data_ptr[i] + hashmap_ptr->_value_size,
                    item_num,
                    userdata
                ))
                    RETURN(SIM_RC_SUCCESS, false);
                
                item_num++;
            }
        }
    } else {
        for (size_t i = 0; i < hashmap_ptr->_allocated; i++) {
            // if the item exists...
            if (data_ptr[i] && data_ptr[i] != (void*)1) {
                if (!foreach_proc.set_foreach_proc(
                    data_ptr[i],
                    item_num,
                    userdata
                ))
                    RETURN(SIM_RC_SUCCESS, false);
                
                item_num++;
            }
        }
    }

    RETURN(SIM_RC_SUCCESS, true);
}

// == HASHSET PUBLIC API ==========================================================================

// sim_hashset_construct(6): Constructs a new hashset.
void sim_hashset_construct(
    Sim_HashSet*          hashset_ptr,
    const size_t          item_size,
    Sim_HashProc          item_hash_proc,
    Sim_PredicateProc     item_predicate_proc,
    const Sim_IAllocator* allocator_ptr,
    const size_t          initial_size
) {
    _sim_hash_construct(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        item_size,
        item_hash_proc,
        item_predicate_proc,
        0,
        allocator_ptr,
        initial_size
    );
}

// sim_hashset_destroy(1): Destroys a hashset.
void sim_hashset_destroy(
    Sim_HashSet *const hashset_ptr
) {
    _sim_hash_destroy(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr })
    );
}

// sim_hashset_is_empty(1): Checks if the hashmap is empty.
bool sim_hashset_is_empty(Sim_HashSet *const hashset_ptr) {
    // check for nullptr
    if (!hashset_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    RETURN(SIM_RC_SUCCESS, hashset_ptr->count == 0);
}

// sim_hashset_clear(1): Clears a hashset of all its contents.
void sim_hashset_clear(
    Sim_HashSet *const hashset_ptr
) {
    _sim_hash_clear(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr })
    );
}

// sim_hashset_contains(2): Checks if an item is contained in a hashset.
bool sim_hashset_contains(
    Sim_HashSet *const hashset_ptr,
    const void *const  item_ptr
) {
    return _sim_hash_contains(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        item_ptr
    );
}

// sim_hashset_resize(2): Resizes a hashset to a given size.
void sim_hashset_resize(
    Sim_HashSet *const hashset_ptr,
    const size_t       new_size
) {
    if (!hashset_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (new_size < hashset_ptr->count)
        THROW(SIM_RC_ERR_INVALARG);
    
    _sim_hash_resize(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        new_size
    );
}

// sim_hashset_add(2): Adds an item into a hashset.
void sim_hashset_insert(
    Sim_HashSet *const hashset_ptr,
    const void*        new_item_ptr
) {
    // check for nullptrs
    if (!hashset_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!new_item_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    _sim_hash_insert(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        new_item_ptr,
        NULL
    );
}

// sim_hashset_remove(2): Removes an item from a hashset.
void sim_hashset_remove(
    Sim_HashSet *const hashset_ptr,
    const void *const  remove_item_ptr
) {
    // check for nullptr
    if (!hashset_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!remove_item_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    _sim_hash_remove(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        remove_item_ptr
    );
}

// sim_hashset_foreach(3): Applies a given function to each item in a hashset.
bool sim_hashset_foreach(
    Sim_HashSet *const   hashset_ptr,
    Sim_ConstForEachProc foreach_proc,
    Sim_Variant          userdata
) {
    return _sim_hash_foreach(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        false,
        (_Sim_HashForEachProc){ .set_foreach_proc = foreach_proc },
        userdata
    );
}

// == HASHMAP PUBLIC API ==========================================================================

// sim_hashmap_construct(7): Initializes a new hashmap.
void sim_hashmap_construct(
    Sim_HashMap *const    hashmap_ptr,
    const size_t          key_size,
    Sim_HashProc          key_hash_proc,
    Sim_PredicateProc     key_predicate_proc,
    const size_t          value_size,
    const Sim_IAllocator* allocator_ptr,
    const size_t          initial_size
) {
    _sim_hash_construct(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        key_size,
        key_hash_proc,
        key_predicate_proc,
        value_size,
        allocator_ptr,
        initial_size
    );
}

// sim_hashmap_destroy(1): Destroys an initilaized hashmap.
void sim_hashmap_destroy(
    Sim_HashMap *const hashmap_ptr
) {
    _sim_hash_destroy(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr })
    );
}

// sim_hashmap_is_empty(1): Checks if the hashmap is empty.
bool sim_hashmap_is_empty(Sim_HashMap *const hashmap_ptr) {
    // check for nullptr
    if (!hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

    RETURN(SIM_RC_SUCCESS, hashmap_ptr->count == 0);
}

// sim_hashmap_contains_key(2): Checks if a key is contained in the hashmap.
bool sim_hashmap_contains_key(
    Sim_HashMap *const hashmap_ptr,
    const void *const key_ptr
) {
    return _sim_hash_contains(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        key_ptr
    );
}

// sim_hashmap_resize(2): Resize the hashmap to a new size.
void sim_hashmap_resize(
    Sim_HashMap *const hashmap_ptr,
    size_t new_size
) {
    if (!hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (new_size < hashmap_ptr->count)
        THROW(SIM_RC_ERR_INVALARG);
    
    _sim_hash_resize(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        new_size
    );
}

// sim_hashmap_get_ptr(2): Get pointer to value in a hashmap via a given key.
void* sim_hashmap_get_ptr(
    Sim_HashMap *const hashmap_ptr,
    const void*        key_ptr
) {
    if (!hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    else if (!key_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    HASH_IF_CONTAIN(hashmap_ptr, key_ptr,
        RETURN(SIM_RC_SUCCESS, current_node_ptr + hashmap_ptr->_key_properties.size);
    );

    RETURN(SIM_RC_NOT_FOUND, NULL);
}

// sim_hashmap_get(3): Get a value from a hashmap via a given key.
void sim_hashmap_get(
    Sim_HashMap *const hashmap_ptr,
    const void*        key_ptr,
    void*              out_value_ptr
) {
    void* value_ptr;

    if (!out_value_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

    // use get_ptr to save on macro-expanded code >:^)
    value_ptr = sim_hashmap_get_ptr(hashmap_ptr, key_ptr);
    THROW(sim_get_return_code());
    if (sim_get_return_code() > 0)
        RETURN(sim_get_return_code(),);
    
    memcpy(
        out_value_ptr,
        value_ptr,
        hashmap_ptr->_value_size
    );

    RETURN(SIM_RC_SUCCESS,);
}

// sim_hashmap_insert(3): Inserts a key-value pair into the hashmap or overwrites a pre-existing
//                        pair if the value is already in the hashmap.
void sim_hashmap_insert(
    Sim_HashMap *const hashmap_ptr,
    const void*        new_key_ptr,
    const void*        value_ptr
) {
    // check for nullptrs
    if (!hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!new_key_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!value_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    _sim_hash_insert(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        new_key_ptr,
        value_ptr
    );
}

// sim_hashmap_remove(2): Removes a key-value pair from the hashmap via a key.
void sim_hashmap_remove(
    Sim_HashMap *const hashmap_ptr,
    const void *const remove_key_ptr
) {
    // check for nullptr
    if (!hashmap_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!remove_key_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    _sim_hash_remove(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        remove_key_ptr
    );
}

// sim_hashmap_foreach(3): Applies a given function to each key-value pair in the hashmap.
bool sim_hashmap_foreach(
    Sim_HashMap *const hashmap_ptr,
    Sim_MapForEachProc foreach_proc,
    Sim_Variant        userdata
) {
    return _sim_hash_foreach(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        true,
        (_Sim_HashForEachProc){ .map_foreach_proc = foreach_proc },
        userdata
    );
}

#endif /* SIMSOFT_HASH_C_ */
