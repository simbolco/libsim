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
#include "./_internal.h"

// Hashmap/hashset aliasing to allow for identical internal implementation
typedef union _Sim_HashPtr {
    Sim_HashMap *const hashmap_ptr;
    Sim_HashSet *const hashset_ptr;
} _Sim_HashPtr;

// == SECONDARY HASH FUNCTION (SipHash) ===========================================================

// == INTERNAL IMPLEMENTATION FUNCTIONS ===========================================================

// Creates a new hash table node.
static void* _sim_hash_create_node(
    const void*                key_ptr,
    const size_t               key_size,
    const void*                val_ptr,
    const size_t               val_size,
    const Sim_Allocator *const allocator_ptr
) {
    byte* node_ptr = allocator_ptr->malloc(key_size + val_size);
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
    void*                      node_ptr,
    const Sim_Allocator *const allocator_ptr
) {
    allocator_ptr->free(node_ptr);
}

// Initializes a hash table (map or set).
static Sim_ReturnCode _sim_hash_initialize(
    _Sim_HashPtr         hash_ptr,
    const size_t         key_size,
    const Sim_DataType   key_type,
    Sim_HashFuncPtr      key_hash_func_ptr,
    Sim_PredicateFuncPtr key_predicate_func_ptr,
    const size_t         value_size,
    const Sim_Allocator* allocator_ptr,
    const size_t         initial_size
) {
    // check for nullptr
    if (!hash_ptr.hashmap_ptr)
        return SIM_RC_ERR_NULLPTR;

    allocator_ptr = allocator_ptr ? allocator_ptr : sim_get_default_allocator();

    // allocate buckets
    size_t starting_size = _sim_next_prime(initial_size);
    if (starting_size < SIM_HASH_DEFAULT_SIZE)
        starting_size = SIM_HASH_DEFAULT_SIZE;
    void* data_ptr = allocator_ptr->falloc(
        sizeof(void*) * starting_size,
        0
    );
    if (!data_ptr)
        return SIM_RC_ERR_OUTOFMEM;

    Sim_HashMap hashmap = {
        ._key_properties = {
            .type_info = {
                .type = key_type,
                .size = key_size
            },
            .hash_func_ptr = key_hash_func_ptr,
            .predicate_func_ptr = key_predicate_func_ptr
        },
        ._allocator_ptr = allocator_ptr,
        ._initial_size = initial_size,
        ._base_size = initial_size,
        ._allocated = starting_size,
        ._deleted_item = 0xfcfcfcfc,

        .count = 0,
        .data_ptr = data_ptr,

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

    return SIM_RC_SUCCESS;
}

// Clears a hash table.
static Sim_ReturnCode _sim_hash_clear(
    _Sim_HashPtr hash_ptr,
    const bool   is_hashmap
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    if (!hashmap_ptr)
        return SIM_RC_ERR_NULLPTR;

    // destroy hash table nodes
    void** item_ptr = hashmap_ptr->data_ptr;
    for (size_t i = 0; i < hashmap_ptr->_allocated; i++)
        if (item_ptr[i] && item_ptr[i] != &hashmap_ptr->_deleted_item)
            _sim_hash_destroy_node(item_ptr[i], hashmap_ptr->_allocator_ptr);
    
    // zero out hash table data
    memset(
        hashmap_ptr->data_ptr,
        0,
        hashmap_ptr->_allocated * (
            hashmap_ptr->_key_properties.type_info.size +
            (is_hashmap ?
                hashmap_ptr->_value_size :
                0
            )
        )
    );

    // reset count
    hashmap_ptr->count = 0;

    return SIM_RC_SUCCESS;
}

// Destroys a hash table.
static Sim_ReturnCode _sim_hash_destroy(
    _Sim_HashPtr hash_ptr,
    const bool   is_hashmap
) {
    Sim_ReturnCode rc;
    if ((rc = _sim_hash_clear(hash_ptr, is_hashmap)))
        return rc;
    
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    // free bucket array when destroying
    hashmap_ptr->_allocator_ptr->free(hashmap_ptr->data_ptr);

    return rc;
}

// insert new item into hash table
static Sim_ReturnCode _sim_hash_insert(
    _Sim_HashPtr hash_ptr,
    const void*  key_ptr,
    const void*  val_ptr
);

// resize hash table to new size
static Sim_ReturnCode _sim_hash_resize(
    _Sim_HashPtr hash_ptr,
    const size_t new_size,
    const bool   is_hashmap
) {
    // resize only if larger than or equal to the initial size
    if (new_size > hash_ptr.hashmap_ptr->_initial_size) {
        Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

        Sim_HashMap new_hash;
        _Sim_HashPtr new_hash_ptr = {
            .hashmap_ptr = &new_hash
        };
        Sim_ReturnCode rc;

        // initialize new hash table
        if ((rc = _sim_hash_initialize(
                new_hash_ptr,
                hashmap_ptr->_key_properties.type_info.size,
                hashmap_ptr->_key_properties.type_info.type,
                hashmap_ptr->_key_properties.hash_func_ptr,
                hashmap_ptr->_key_properties.predicate_func_ptr,
                is_hashmap ?
                    hashmap_ptr->_value_size :
                    0
                ,
                hashmap_ptr->_allocator_ptr,
                new_size
            )
        ))
            return rc;

        // preserve initial size
        *((union {
            const size_t* const_ptr;
            size_t* ptr;
        }){
            .const_ptr = &new_hash_ptr.hashmap_ptr->_initial_size
        }).ptr = hashmap_ptr->_initial_size;

        // re-hash old data and put into new hash table
        byte** hash_data = hashmap_ptr->data_ptr;
        for (size_t i = 0; i < hashmap_ptr->_allocated; i++) {
            byte* hash_node = hash_data[i];
            if (hash_node && hash_node != (const byte*)&hashmap_ptr->_deleted_item) {
                if ((rc = _sim_hash_insert(
                        new_hash_ptr,
                        hash_node,
                        is_hashmap ?
                            hash_node + hashmap_ptr->_key_properties.type_info.size :
                            NULL
                    )
                )) {
                    _sim_hash_destroy(new_hash_ptr, is_hashmap);
                    return rc;
                }
            }
        }

        _sim_hash_destroy(hash_ptr, is_hashmap);
        memcpy(
            hashmap_ptr,
            &new_hash,
            is_hashmap ?
                sizeof(Sim_HashMap) :
                sizeof(Sim_HashSet)
        );
    }

    return SIM_RC_SUCCESS;
}

#define HASH_IF_CONTAIN(hashmap_ptr, key_ptr, ...)                                          \
    byte** data_ptr = hashmap_ptr->data_ptr;                                                \
    size_t attempt = 0;                                                                     \
    byte* current_node_ptr;                                                                \
    size_t index;                                                                           \
                                                                                            \
    const size_t hash1 = (hashmap_ptr->_key_properties.hash_func_ptr ?                      \
        (*hashmap_ptr->_key_properties.hash_func_ptr)(key_ptr) :                            \
        _sim_siphash(                                                                       \
            key_ptr,                                                                        \
            hashmap_ptr->_key_properties.type_info.size,                                    \
            SIPHASH_KEY1,                                                                   \
            SIPHASH_KEY2                                                                    \
        )                                                                                   \
    ) % hashmap_ptr->_allocated;                                                            \
    size_t hash2 = 0;                                                                       \
                                                                                            \
    do {                                                                                    \
        index = (hash1 + (attempt * (hash2 + 1))) % hashmap_ptr->_allocated;                \
        current_node_ptr = data_ptr[index];                                                 \
                                                                                            \
        if (                                                                                \
            current_node_ptr != (const byte*)&hashmap_ptr->_deleted_item &&                              \
            (*(hashmap_ptr->_key_properties.predicate_func_ptr))(key_ptr, current_node_ptr) \
        ) {                                                                                 \
            UNPACK(__VA_ARGS__);                                                            \
        }                                                                                   \
                                                                                            \
        attempt++;                                                                          \
        if (attempt == 1) {                                                                 \
            hash2 = _sim_siphash(                                                           \
                key_ptr,                                                                    \
                hashmap_ptr->_key_properties.type_info.size,                                \
                SIPHASH_KEY3, SIPHASH_KEY4                                                  \
            ) % hashmap_ptr->_allocated;                                                    \
        }                                                                                   \
    } while (current_node_ptr);

Sim_ReturnCode _sim_hash_insert(
    _Sim_HashPtr hash_ptr,
    const void*  key_ptr,
    const void*  value_ptr
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;
    Sim_ReturnCode rc;

    // check how much of the hash table is used & resize up if necessary
    const size_t load = hashmap_ptr->count * 100 / hashmap_ptr->_allocated;
    if (
        load > 70 && 
        (rc = _sim_hash_resize(
                hash_ptr,
                hashmap_ptr->_base_size * 2,
                !!(value_ptr ?
                    hashmap_ptr->_value_size :
                    0
                )
            )
        )
    )
        return rc;

    HASH_IF_CONTAIN(hashmap_ptr, key_ptr,
        // return FAILURE if hashset
        if (!value_ptr)
            return SIM_RC_FAILURE;
        
        // copy contents of value_ptr to node
        memcpy(
            current_node_ptr + hashmap_ptr->_value_size,
            value_ptr,
            hashmap_ptr->_value_size
        );
        return SIM_RC_SUCCESS;
    );

    // insert new item
    void* node = _sim_hash_create_node(
        key_ptr,
        hashmap_ptr->_key_properties.type_info.size,
        value_ptr,
        value_ptr ?
            hashmap_ptr->_value_size :
            0
        ,
        hashmap_ptr->_allocator_ptr
    );
    if (!node)
        return SIM_RC_ERR_OUTOFMEM;
    
    data_ptr[index] = node;
    hashmap_ptr->count++;
    return SIM_RC_SUCCESS;
}

// Removes an item from a hash table.
static Sim_ReturnCode _sim_hash_remove(
    _Sim_HashPtr hash_ptr,
    const void*  key_ptr,
    const bool   is_hashmap
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;
    Sim_ReturnCode rc;

    // check how much of the hash table is used & resize down if necessary
    const size_t load = hashmap_ptr->count * 100 / hashmap_ptr->_allocated;
    if (
        load < 10 && 
        (rc = _sim_hash_resize(
                hash_ptr,
                hashmap_ptr->_base_size / 2,
                is_hashmap
            )
        )
    )
        return rc;

    HASH_IF_CONTAIN(hashmap_ptr, key_ptr, 
        // destroy node
        _sim_hash_destroy_node(current_node_ptr, hashmap_ptr->_allocator_ptr);
        
        // set data_ptr @ index to deleted node
        data_ptr[index] = ((union {
            const size_t* a;
            byte* b;
        }){.a = &hashmap_ptr->_deleted_item}.b);

        // decrement count
        hashmap_ptr->count--;

        return SIM_RC_SUCCESS;
    );
    
    // failed to remove item; item not in hash table
    return SIM_RC_ERR_NOTFOUND;
}

// Checks if an item/key is contained in a hash table.
inline Sim_ReturnCode _sim_hash_contains(
    _Sim_HashPtr hash_ptr,
    const void* key_ptr
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    // check for nullptrs
    if (!hashmap_ptr || !key_ptr)
        return SIM_RC_ERR_NULLPTR;
    
    HASH_IF_CONTAIN(hashmap_ptr, key_ptr,
        return SIM_RC_SUCCESS;
    );

    return SIM_RC_ERR_NOTFOUND;
}

// Apply a function for each item in hash table.
static Sim_ReturnCode _sim_hash_foreach(
    _Sim_HashPtr       hash_ptr,
    const bool         is_hashmap,
    Sim_ForEachFuncPtr foreach_func_ptr,
    Sim_Variant        userdata
) {
    Sim_HashMap *const hashmap_ptr = hash_ptr.hashmap_ptr;

    // check for nullptrs
    if (!hashmap_ptr || !foreach_func_ptr)
        return SIM_RC_ERR_NULLPTR;

    byte** data_ptr = hashmap_ptr->data_ptr;
    size_t item_num = 0;

    // iterate through allocated
    for (size_t i = 0; i < hashmap_ptr->_allocated; i++) {
        // if the item exists...
        if (data_ptr[i] && data_ptr[i] != (const byte*)&hashmap_ptr->_deleted_item) {
            if (!(*foreach_func_ptr)(
                is_hashmap ?
                    (void*)&((Sim_MapConstKeyValuePair){
                        .key = data_ptr[i],
                        .value = (byte*)data_ptr[i] + hashmap_ptr->_value_size
                    }) :
                    data_ptr[i]
                ,
                userdata,
                item_num
            ))
                return SIM_RC_FAILURE;
            
            item_num++;
        }
        data_ptr++;
    }

    return SIM_RC_SUCCESS;
}

// == HASHSET PUBLIC API ==========================================================================

// sim_hashset_initialize(7): Initializes a new hashset.
Sim_ReturnCode sim_hashset_initialize(
    Sim_HashSet*         hashset_ptr,
    const size_t         item_size,
    const Sim_DataType   item_type,
    Sim_HashFuncPtr      item_hash_func_ptr,
    Sim_PredicateFuncPtr item_predicate_func_ptr,
    const Sim_Allocator* allocator_ptr,
    const size_t         initial_size
) {
    return _sim_hash_initialize(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        item_size,
        item_type,
        item_hash_func_ptr,
        item_predicate_func_ptr,
        0,
        allocator_ptr,
        initial_size
    );
}

// sim_hashset_create(6): Creates a new hashset on the heap.
Sim_HashSet* sim_hashset_create(
    const size_t         item_size,
    const Sim_DataType   item_type,
    Sim_HashFuncPtr      item_hash_func_ptr,
    Sim_PredicateFuncPtr item_predicate_func_ptr,
    const Sim_Allocator* allocator_ptr,
    const size_t         initial_size
) {
    // allocate hashset
    Sim_HashSet* hashset_ptr = malloc(sizeof(Sim_HashSet));

    // initialize hashset & destroy if unsuccessful
    if (!(sim_hashset_initialize(
        hashset_ptr,
        item_size,
        item_type,
        item_hash_func_ptr,
        item_predicate_func_ptr,
        allocator_ptr,
        initial_size
    ) == SIM_RC_SUCCESS)) {
        free(hashset_ptr);
        return NULL;
    }
    
    return hashset_ptr;
}

// sim_hashset_destroy(1): Destroys an initilaized hashset.
Sim_ReturnCode sim_hashset_destroy(
    Sim_HashSet *const hashset_ptr
) {
    return _sim_hash_destroy(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        false
    );
}

// sim_hashset_free(1): Frees & destroys a heap-allocated hashset.
_OBJECT_FREE_FUNCTION(Sim_HashSet, sim_hashset)

// sim_hashset_clear(1): Clears a hashset of all its contents.
Sim_ReturnCode sim_hashset_clear(
    Sim_HashSet* hashset_ptr
) {
    return _sim_hash_clear(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        false
    );
}

// sim_hashset_contains(2): Checks if an item is contained in the hashset.
Sim_ReturnCode C_CALL sim_hashset_contains(
    Sim_HashSet *const hashset_ptr,
    const void *const  item_ptr
) {
    return _sim_hash_contains(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        item_ptr
    );
}

// sim_hashset_resize(2): Resizes the hashset to a new size.
Sim_ReturnCode sim_hashset_resize(
    Sim_HashSet *const hashset_ptr,
    const size_t       new_size
) {
    if (!hashset_ptr)
        return SIM_RC_ERR_NULLPTR;

    if (new_size < hashset_ptr->count)
        return SIM_RC_ERR_INVALARG;
    
    return _sim_hash_resize(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        new_size,
        false
    );
}

// sim_hashset_add(2): Adds an item into a hashset.
Sim_ReturnCode sim_hashset_insert(
    Sim_HashSet *const hashset_ptr,
    const void*        new_item_ptr
) {
    // check for nullptrs
    if (!hashset_ptr || !new_item_ptr)
        return SIM_RC_ERR_NULLPTR;
    
    return _sim_hash_insert(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        new_item_ptr,
        NULL
    );
}

// sim_hashset_remove(2): Removes an item from a hashset.
Sim_ReturnCode sim_hashset_remove(
    Sim_HashSet *const hashset_ptr,
    const void *const  remove_item_ptr
) {
    // check for nullptr
    if (!hashset_ptr)
        return SIM_RC_ERR_NULLPTR;
    
    return _sim_hash_remove(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        remove_item_ptr,
        false
    );
}

// sim_hashset_foreach(3): Applies a given function to each item in the hashset.
Sim_ReturnCode sim_hashset_foreach(
    Sim_HashSet *const      hashset_ptr,
    Sim_ConstForEachFuncPtr foreach_func_ptr,
    Sim_Variant             userdata
) {
    return _sim_hash_foreach(
        ((_Sim_HashPtr){ .hashset_ptr = hashset_ptr }),
        false,
        (Sim_ForEachFuncPtr)foreach_func_ptr,
        userdata
    );
}

// == HASHMAP PUBLIC API ==========================================================================

// sim_hashmap_initialize(8): Initializes a new hashmap.
Sim_ReturnCode sim_hashmap_initialize(
    Sim_HashMap*         hashmap_ptr,
    const size_t         key_size,
    const Sim_DataType   key_type,
    Sim_HashFuncPtr      key_hash_func_ptr,
    Sim_PredicateFuncPtr key_predicate_func_ptr,
    const size_t         value_size,
    const Sim_Allocator* allocator_ptr,
    const size_t         initial_size
) {
    return _sim_hash_initialize(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        key_size,
        key_type,
        key_hash_func_ptr,
        key_predicate_func_ptr,
        value_size,
        allocator_ptr,
        initial_size
    );
}

// sim_hashmap_create(7): Creates a new hashmap on the heap.
Sim_HashMap* sim_hashmap_create(
    const size_t         key_size,
    const Sim_DataType   key_type,
    Sim_HashFuncPtr      key_hash_func_ptr,
    Sim_PredicateFuncPtr key_predicate_func_ptr,
    const size_t         value_size,
    const Sim_Allocator* allocator_ptr,
    const size_t         initial_size
) {
    // allocate hashmap
    Sim_HashMap* hashmap_ptr = malloc(sizeof(Sim_HashMap));

    // initialize hashmap & destroy if unsuccessful
    if (!(sim_hashmap_initialize(
        hashmap_ptr,
        key_size,
        key_type,
        key_hash_func_ptr,
        key_predicate_func_ptr,
        value_size,
        allocator_ptr,
        initial_size
    ) == SIM_RC_SUCCESS)) {
        free(hashmap_ptr);
        return NULL;
    }
    
    return hashmap_ptr;
}

// sim_hashmap_destroy(1): Destroys an initilaized hashmap.
Sim_ReturnCode sim_hashmap_destroy(
    Sim_HashMap* hashmap_ptr
) {
    return _sim_hash_destroy(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        true
    );
}

// sim_hashmap_free(1): Frees & destroys a heap-allocated hashmap.
_OBJECT_FREE_FUNCTION(Sim_HashMap, sim_hashmap)

// sim_hashmap_clear(1): Clears a hashset of all its contents.
Sim_ReturnCode sim_hashmap_clear(
    Sim_HashMap* hashmap_ptr
) {
    return _sim_hash_clear(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        true
    );
}

// sim_hashmap_contains_key(2): Checks if a key is contained in the hashmap.
Sim_ReturnCode sim_hashmap_contains_key(
    Sim_HashMap *const hashmap_ptr,
    const void *const key_ptr
) {
    return _sim_hash_contains(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        key_ptr
    );
}

// sim_hashmap_resize(2): Resize the hashmap to a new size.
Sim_ReturnCode sim_hashmap_resize(
    Sim_HashMap *const hashmap_ptr,
    size_t new_size
) {
    if (!hashmap_ptr)
        return SIM_RC_ERR_NULLPTR;

    if (new_size < hashmap_ptr->count)
        return SIM_RC_ERR_INVALARG;
    
    return _sim_hash_resize(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        new_size,
        true
    );
}

// sim_hashmap_get_ptr(3): Get pointer to value in the hashmap via a particular key.
Sim_ReturnCode sim_hashmap_get_ptr(
    Sim_HashMap *const hashmap_ptr,
    const void*        key_ptr,
    void* *const       out_value_ptr
) {
    if (!hashmap_ptr || !out_value_ptr || !key_ptr)
        return SIM_RC_ERR_NULLPTR;

    HASH_IF_CONTAIN(hashmap_ptr, key_ptr,
        *out_value_ptr = current_node_ptr + hashmap_ptr->_key_properties.type_info.size;
        return SIM_RC_SUCCESS;
    );

    return SIM_RC_ERR_NOTFOUND;
}

// sim_hashmap_get(3): Get a value from the hashmap via a particular key.
Sim_ReturnCode sim_hashmap_get(
    Sim_HashMap *const hashmap_ptr,
    const void*        key_ptr,
    void*              out_value_ptr
) {
    Sim_ReturnCode rc;
    void* value_ptr;

    if (!out_value_ptr)
        return SIM_RC_ERR_NULLPTR;

    // use get_ptr to save on macro-expanded code >:^)
    if ((rc = sim_hashmap_get_ptr(
        hashmap_ptr,
        key_ptr,
        &value_ptr
    )))
        return rc;
    
    memcpy(
        out_value_ptr,
        value_ptr,
        hashmap_ptr->_value_size
    );

    return SIM_RC_SUCCESS;
}

// sim_hashmap_insert(3): Inserts a key-value pair into the hashmap or overwrites a pre-existing
//                        pair if the value is already in the hashmap.
Sim_ReturnCode sim_hashmap_insert(
    Sim_HashMap *const hashmap_ptr,
    const void*        new_key_ptr,
    const void*        value_ptr
) {
    // check for nullptrs
    if (!hashmap_ptr || !new_key_ptr || !value_ptr)
        return SIM_RC_ERR_NULLPTR;

    return _sim_hash_insert(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        new_key_ptr,
        value_ptr
    );
}

// sim_hashmap_remove(2): Removes a key-value pair from the hashmap via a key.
Sim_ReturnCode sim_hashmap_remove(
    Sim_HashMap *const hashmap_ptr,
    const void *const remove_key_ptr
) {
    // check for nullptr
    if (!hashmap_ptr)
        return SIM_RC_ERR_NULLPTR;
    
    return _sim_hash_remove(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        remove_key_ptr,
        true
    );
}

// sim_hashmap_foreach(3): Applies a given function to each key-value pair in the hashmap.
Sim_ReturnCode sim_hashmap_foreach(
    Sim_HashMap *const    hashmap_ptr,
    Sim_MapForEachFuncPtr foreach_func_ptr,
    Sim_Variant           userdata
) {
    return _sim_hash_foreach(
        ((_Sim_HashPtr){ .hashmap_ptr = hashmap_ptr }),
        true,
        (Sim_ForEachFuncPtr)foreach_func_ptr,
        userdata
    );
}

#endif /* SIMSOFT_HASH_C_ */
