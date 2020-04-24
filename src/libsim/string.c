/**
 * @file string.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/string.h
 * @version 0.1
 * @date 2020-03-31
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_STRING_C_
#define SIMSOFT_STRING_C_

#include <stdarg.h>

#include "simsoft/string.h"
#include "simsoft/util.h"

#include "./_internal.h"

#define _SIM_STRING_INTERNAL_SIZE(string_ptr) \
    (sizeof (string_ptr)->_internal_data / sizeof (string_ptr)->_internal_data[0])

// == PRIVATE API - HELPER FUNCTIONS ===============================================================

// Reizes a string to a given capacity
static bool _sim_string_resize(Sim_String *const string_ptr, size_t new_size) {
    if (string_ptr->_is_large_string) /* resize large string */ {
        // resize if there is no allocated space left
        if (new_size > string_ptr->_allocated) {
            string_ptr->_allocated = new_size;
            
            // resize array & raise error on fail
            char* resized_c_string = (char*)string_ptr->_allocator_ptr->realloc(
                string_ptr->c_string,
                new_size
            );
            if (!resized_c_string)
                return false;

            string_ptr->c_string = resized_c_string;
        }
    } else /* move & resize small string to heap if new size is too large */ {
        if (new_size > _SIM_STRING_INTERNAL_SIZE(string_ptr)) {
            char* new_c_string = (char*)string_ptr->_allocator_ptr->malloc(new_size);
            if (!new_c_string)
                return false;

            // copy contents of old string into new
            memmove(new_c_string, string_ptr->c_string, string_ptr->length + 1);
            string_ptr->c_string = new_c_string;

            string_ptr->_is_large_string = true;
            string_ptr->_allocated = new_size;
        }
    }

    return true;
}

// default string hashing function
static Sim_HashType _sim_string_default_hash(
    const Sim_String *const string_ptr,
    const size_t attempt
) {
    
#   define SIPHASH_KEY1 0x90d6346e7b77f546ULL
#   define SIPHASH_KEY2 0x1e0a6097372b5de5ULL
#   define SIPHASH_KEY3 0x62d76395429756a9ULL
#   define SIPHASH_KEY4 0xe26534637479058cULL

    Sim_HashType hash = sim_siphash(
        (const uint8*)string_ptr->c_string,
        string_ptr->length,
        (Sim_HashKey){ SIPHASH_KEY1, SIPHASH_KEY2 }
    );

    if (attempt > 0) {
        const Sim_HashType hash2 = sim_siphash(
            (const uint8*)string_ptr->c_string,
            string_ptr->length,
            (Sim_HashKey){ SIPHASH_KEY3, SIPHASH_KEY4 }
        );

        hash += (hash2 * attempt) + attempt;
    }

    return hash;

#   undef SIPHASH_KEY1
#   undef SIPHASH_KEY2
#   undef SIPHASH_KEY3
#   undef SIPHASH_KEY4
}

static Sim_HashProc _sim_string_hash_proc = (Sim_HashProc)_sim_string_default_hash;

// == PUBLIC API ===================================================================================

// sim_string_construct(4): Constructs a string.
void sim_string_construct(
    Sim_String *const     string_ptr,
    const Sim_IAllocator* allocator_ptr,
    size_t                c_string_length,
    const char*           c_string
) {
    if (!string_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

    // use default allocator if none is provided
    if (!allocator_ptr)
        allocator_ptr = sim_allocator_get_default();

    memcpy(
        (uint8*)string_ptr + offsetof(Sim_String, _allocator_ptr),
        &allocator_ptr,
        sizeof allocator_ptr
    );
    string_ptr->_hash_dirty = false;

    // c_string or c_string length are NULL/0 - empty string
    if (!c_string || !c_string_length) {
        string_ptr->_internal_data[0] = '\0';
        string_ptr->_is_large_string = false;

        string_ptr->length = 0;
        string_ptr->c_string = string_ptr->_internal_data;
    } else {
        string_ptr->length = c_string_length;

        // large string
        if (c_string_length > _SIM_STRING_INTERNAL_SIZE(string_ptr)) {
            size_t capacity = c_string_length + 1;
            string_ptr->_allocated = capacity;
            string_ptr->_is_large_string = true;

            // allocate string from heap; fail if unsuccessful
            char* allocated_c_string = (char*)allocator_ptr->malloc(capacity);
            if (!allocated_c_string)
                THROW(SIM_RC_ERR_OUTOFMEM);

            string_ptr->c_string = allocated_c_string;
        } else /* short string */ {
            string_ptr->_is_large_string = false;
            string_ptr->c_string = string_ptr->_internal_data;
        }
    }

    // copy c_string to memory pointed to by string.c_string
    memcpy(string_ptr->c_string, c_string, c_string_length);
    string_ptr->c_string[c_string_length] = '\0';

    // get hash values
    string_ptr->_hash1 = _sim_string_hash_proc(string_ptr, 0);
    string_ptr->_hash2 = _sim_string_hash_proc(string_ptr, 1) - string_ptr->_hash1 - 1;

    RETURN(SIM_RC_SUCCESS,);
}

// sim_string_construct_format(4+): Constructs a string given a format string.
void sim_string_construct_format(
    Sim_String *const     string_ptr,
    const Sim_IAllocator* allocator_ptr,
    size_t                string_length,
    const char*           format_string,
    ...
) {
    if (!format_string) {
        sim_string_construct(string_ptr, allocator_ptr, string_length, NULL);
        return;
    }

    if (!string_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

    // use default allocator if none is provided
    if (!allocator_ptr)
        allocator_ptr = sim_allocator_get_default();

    memcpy(
        (uint8*)string_ptr + offsetof(Sim_String, _allocator_ptr),
        &allocator_ptr,
        sizeof allocator_ptr
    );
    string_ptr->_hash_dirty = false;

    // variadic args list
    va_list args;

    // grab string length to fit if not provided via vsnprintf
    if (!string_length) {
        va_start(args, format_string);
        string_length = vsnprintf(NULL, 0, format_string, args);
        va_end(args);
    }

    // large string
    if (string_length > _SIM_STRING_INTERNAL_SIZE(string_ptr)) {
        size_t capacity = string_length + 1;
        string_ptr->_allocated = capacity;
        string_ptr->_is_large_string = true;

        // allocate string from heap; fail if unsuccessful
        char* allocated_c_string = (char*)allocator_ptr->malloc(capacity);
        if (!allocated_c_string)
            THROW(SIM_RC_ERR_OUTOFMEM);

        string_ptr->c_string = allocated_c_string;
    } else /* short string */ {
        string_ptr->_is_large_string = false;
        string_ptr->c_string = string_ptr->_internal_data;
    }

    // deformat the given string with args
    va_start(args, format_string);
    vsnprintf(string_ptr->c_string, string_length, format_string, args);
    va_end(args);

    // null terminator
    string_ptr->c_string[string_length] = '\0';

    // get hash values
    string_ptr->_hash1 = _sim_string_hash_proc(string_ptr, 0);
    string_ptr->_hash2 = _sim_string_hash_proc(string_ptr, 1) - string_ptr->_hash1 - 1;

    RETURN(SIM_RC_SUCCESS,);
}

// sim_string_copy(2): Copies the contents of one string into another.
void sim_string_copy(
    Sim_String *const string_from_ptr,
    Sim_String *const string_to_ptr
) {
    if (!string_from_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!string_to_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    memcpy((uint8*)string_to_ptr, (uint8*)string_from_ptr, sizeof *string_from_ptr);

    // copy string to new buffer if large string
    if (string_to_ptr->_is_large_string) {
        char* new_string_buffer = (char*)string_to_ptr->_allocator_ptr->malloc(
            string_to_ptr->length + 1
        );
        if (!new_string_buffer)
            THROW(SIM_RC_ERR_OUTOFMEM);

        string_to_ptr->_allocated = string_to_ptr->length + 1;
        memcpy(new_string_buffer, string_to_ptr->c_string, string_to_ptr->length + 1);
        string_to_ptr->c_string = new_string_buffer;
    }

    RETURN(SIM_RC_SUCCESS,);
}

// sim_string_move(2): Moves the contents of one string to another.
void sim_string_move(
    Sim_String *const string_from_ptr,
    Sim_String *const string_to_ptr
) {
    if (!string_from_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!string_to_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

    // copy length & large string flag
    string_to_ptr->length = string_from_ptr->length;
    string_to_ptr->_is_large_string = string_from_ptr->_is_large_string;

    // copy allocator_ptr
    memcpy(
        (uint8*)string_to_ptr + offsetof(Sim_String, _allocator_ptr),
        (uint8*)string_from_ptr + offsetof(Sim_String, _allocator_ptr),
        sizeof string_from_ptr->_allocator_ptr
    );

    if (string_from_ptr->_is_large_string) /* switch ownership of large string */ {
        string_to_ptr->_allocated = string_from_ptr->_allocated;
        
        string_to_ptr->c_string = string_from_ptr->c_string;
        string_from_ptr->_is_large_string = false; // set to false so destructor won't free
    } else /* copy small string */ {
        string_to_ptr->c_string = string_to_ptr->_internal_data;
        memcpy(string_to_ptr->c_string, string_from_ptr->_internal_data, string_to_ptr->length + 1);
    }

    // copy string hash
    string_to_ptr->_hash_dirty = string_from_ptr->_hash_dirty;
    if (!string_to_ptr->_hash_dirty) {
        string_to_ptr->_hash1 = string_from_ptr->_hash1;
        string_to_ptr->_hash2 = string_from_ptr->_hash2;
    }

    RETURN(SIM_RC_SUCCESS,);
}

// sim_string_destroy(1): Destroys a string.
void sim_string_destroy(Sim_String *const string_ptr) {
    if (!string_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

    // free large C string
    if (string_ptr->_is_large_string)
        free(string_ptr->c_string);

    RETURN(SIM_RC_SUCCESS,);
}

// sim_string_is_empty(1): Checks if a string is empty.
bool sim_string_is_empty(Sim_String *const string_ptr) {
    if (!string_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    RETURN(SIM_RC_SUCCESS, string_ptr->length == 0);
}

// sim_string_get_allocated(1): Retrieves the number of bytes in memory that is allocated by the
//                              string.
size_t sim_string_get_allocated(Sim_String *const string_ptr) {
    if (!string_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

    if (string_ptr->_is_large_string)
        RETURN(SIM_RC_SUCCESS, string_ptr->_allocated);
    RETURN(SIM_RC_SUCCESS, _SIM_STRING_INTERNAL_SIZE(string_ptr));
}

// sim_string_get_hash(1): Retrieves the hash value of a string.
Sim_HashType sim_string_get_hash(
    Sim_String *const string_ptr,
    const size_t      attempt
) {
    if (!string_ptr)
        return 0;
    
    // update hash values if dirty
    if (string_ptr->_hash_dirty) {
        string_ptr->_hash1 = _sim_string_hash_proc(string_ptr, 0);
        string_ptr->_hash2 = _sim_string_hash_proc(string_ptr, 1) - string_ptr->_hash1 - 1;
        string_ptr->_hash_dirty = false;
    }

    return string_ptr->_hash1 + (string_ptr->_hash2 * attempt) + attempt;
}

// sim_string_insert(4): Inserts a new string at a given position.
bool sim_string_insert(
    Sim_String *const string_ptr,
    const size_t      new_string_length,
    const char*       new_string,
    const size_t      index
) {
    if (!string_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (index > string_ptr->length)
        THROW(SIM_RC_ERR_OUTOFBND);

    // empty || zero length = NOP
    if (!new_string || new_string_length == 0)
        RETURN(SIM_RC_SUCCESS, true);

    // resize string
    size_t new_length = string_ptr->length + new_string_length;
    if (!_sim_string_resize(string_ptr, new_length + 1))
        THROW(SIM_RC_ERR_OUTOFMEM);

    // move contents right of index to right-hand side of index
    memmove(
        &string_ptr->c_string[index + new_string_length],
        &string_ptr->c_string[index],
        string_ptr->length - index
    );
    memcpy(&string_ptr->c_string[index], new_string, new_string_length);

    // add null terminator
    string_ptr->c_string[new_length] = '\0';
    string_ptr->length = new_length;

    // set hash dirty bit
    string_ptr->_hash_dirty = true;

    RETURN(SIM_RC_SUCCESS, true);
}

// sim_string_append(3): Appends a new string to the back of a string.
bool sim_string_append(
    Sim_String *const string_ptr,
    const size_t      new_string_length,
    const char*       new_string
) {
    return sim_string_insert(string_ptr, new_string_length, new_string, string_ptr->length);
}

// sim_string_remove(3): Removes a section of a string.
void sim_string_remove(
    Sim_String *const string_ptr,
    const size_t      index,
    const size_t      length
) {
    if (!string_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (index >= string_ptr->length || index + length >= string_ptr->length)
        THROW(SIM_RC_ERR_OUTOFBND);

    size_t new_length = string_ptr->length - length;
    
    // move the right side of removed section
    memmove(
        &string_ptr->c_string[index],
        &string_ptr->c_string[index + length],
        string_ptr->length - index - length
    );

    // add null terminator
    string_ptr->c_string[new_length] = '\0';
    string_ptr->length = new_length;

    // set hash dirty bit
    string_ptr->_hash_dirty = true;

    RETURN(SIM_RC_SUCCESS,);
}

// sim_string_find(4): Finds a substring within a string.
size_t sim_string_find(
    Sim_String *const string_ptr,
    const size_t      substring_length,
    const char*       substring,
    const size_t      starting_index
) {
    if (!string_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    if (!substring)
        THROW(SIM_RC_ERR_NULLPTR);
    if (starting_index >= string_ptr->length)
        THROW(SIM_RC_ERR_OUTOFBND);

    size_t i = starting_index, j = 0;
    while (i < string_ptr->length - substring_length + j) {
        // check j'th character of substring
        if (string_ptr->c_string[i] == substring[j])
            j++;
        else
            j = 0;

        if (substring[j] == '\0' || j == substring_length)
            RETURN(SIM_RC_SUCCESS, i - j + 1);

        i++;
    }

    RETURN(SIM_RC_NOT_FOUND, (size_t)-1);
}

// sim_string_replace(6): Replaces a substring within a string with another string.
size_t sim_string_replace(
    Sim_String *const string_ptr,
    const size_t      find_string_length,
    const char*       find_string,
    const size_t      replace_string_length,
    const char*       replace_string,
    const size_t      starting_index
) {
    if (!replace_string)
        THROW(SIM_RC_ERR_NULLPTR);

    // string_find validates input and gives us an index to replace at
    size_t find_index = sim_string_find(
        string_ptr,
        find_string_length,
        find_string,
        starting_index
    );
    if (find_index == (size_t)-1)
        return find_index;

    // remove find_string from string_ptr
    sim_string_remove(string_ptr, find_index, find_string_length);
    
    // insert if possible
    if (!sim_string_insert(string_ptr, replace_string_length, replace_string, find_index))
        return (size_t)-1;

    return find_index;
}

// sim_string_get_default_hash_proc(0): Retrieves the string hash function.
Sim_HashProc sim_string_get_default_hash_proc(void) {
    return _sim_string_hash_proc;
}

// sim_string_set_default_hash_proc(1): Sets the string hash funciton.
void sim_string_set_default_hash_proc(Sim_HashProc hash_proc) {
    _sim_string_hash_proc = hash_proc ? hash_proc : (Sim_HashProc)_sim_string_default_hash;
}

#endif /* SIMSOFT_STRING_C_ */
