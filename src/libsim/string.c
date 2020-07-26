/**
 * @file string.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/string.h
 * @version 0.2
 * @date 2020-07-26
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_STRING_C_
#define SIMSOFT_STRING_C_

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "simsoft/string.h"

#include "simsoft/except.h"
#include "simsoft/util.h"

#include "./_internal.h"

#define _STRING_HASH_KEY ((Sim_HashKey){ 0x90d6346e7b77f546ULL, 0x1e0a6097372b5de5ULL })

// == PRIVATE HELPERS ==============================================================================

static void _sim_string_construct(
    Sim_String *const string_ptr,
    const char*       c_string,
    size_t            length,
    bool              owns_string
) {
    *string_ptr = (Sim_String){
        .c_string     = c_string,
        .length       = length,
        ._owns_string = owns_string,
        .hash         = sim_util_siphash((const uint8*)c_string, length, _STRING_HASH_KEY)
    };
}

// == PUBLIC API ===================================================================================

// sim_string_construct(3): Constructs a new string.
void sim_string_construct(
    Sim_String *const string_ptr,
    const char*       c_string,
    bool              owns_string
) {
    if (!string_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    
    if (!c_string) 
        _sim_string_construct(string_ptr, "", 0, false);
    else
        _sim_string_construct(string_ptr, c_string, strlen(c_string), owns_string);
}

// sim_string_construct_format(2+): Constructs a new string using a format string.
void sim_string_construct_format(
    Sim_String *const string_ptr,
    const char*       format_string,
    ...
) {
    if (!format_string) {
        sim_string_construct(string_ptr, NULL, false);
        return;
    }

    va_list args;
    va_start(args, format_string);
    size_t length = (size_t)vsnprintf(NULL, 0, format_string, args);
    va_end(args);

    char* buffer = malloc(sizeof(char) * (length + 1));
    if (!buffer)
        THROW(SIM_ERR_OUTOFMEM, "Failed to allocate format string buffer");

    va_start(args, format_string);
    vsnprintf(buffer, length + 1, format_string, args);
    va_end(args);

    sim_string_construct(string_ptr, buffer, true);
}

// sim_string_copy(2): Copies the contents of one string into another.
void sim_string_copy(
    Sim_String *const RESTRICT string_from_ptr,
    Sim_String *const RESTRICT string_to_ptr
) {
    if (!string_from_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    if (!string_to_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);
    
    char* buffer = malloc(sizeof(char) * (string_from_ptr->length + 1));
    if (!buffer)
        THROW(SIM_ERR_OUTOFMEM, "Failed to duplicate string");
    memmove(buffer, string_from_ptr->c_string, string_from_ptr->length + 1);

    *string_to_ptr = (Sim_String){
        .c_string     = buffer,
        .length       = string_from_ptr->length,
        .hash         = string_from_ptr->hash,
        ._owns_string = true
    };
}

// sim_string_move(2): Moves the contents from one string to another.
void sim_string_move(
    Sim_String *const RESTRICT string_from_ptr,
    Sim_String *const RESTRICT string_to_ptr
) {
    if (!string_from_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    if (!string_to_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);
    
    *string_to_ptr = *string_from_ptr;
    if (string_from_ptr->_owns_string)
        string_from_ptr->_owns_string = false;
}

// sim_string_destroy(1): Destroys a string.
void sim_string_destroy(Sim_String *const string_ptr) {
    if (!string_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    
    if (string_ptr->_owns_string) {
        union {
            const char* c;
            char* v;
        } u = { .c = string_ptr->c_string };

        free(u.v);
    }
    
    string_ptr->c_string = NULL;
    string_ptr->length = 0;
}

// sim_string_concat(3): Concatenates two strings together.
void sim_string_concat(
    Sim_String *const          left_string_ptr,
    Sim_String *const          right_string_ptr,
    Sim_String *const RESTRICT string_to_ptr 
) {
    if (!left_string_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    if (!right_string_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 1 is NULL", FUNCTION_NAME);
    if (!string_to_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 2 is NULL", FUNCTION_NAME);
    
    char* buffer = malloc(sizeof(char) * (left_string_ptr->length + right_string_ptr->length + 1));
    if (!buffer)
        THROW(SIM_ERR_OUTOFMEM,
            "(%s) Failed to allocate buffer for concatenated string",
            FUNCTION_NAME
        );
    memmove(buffer, left_string_ptr->c_string, left_string_ptr->length);
    memmove(
        buffer + left_string_ptr->length,
        right_string_ptr->c_string,
        right_string_ptr->length + 1
    );

    _sim_string_construct(
        string_to_ptr,
        buffer,
        left_string_ptr->length + right_string_ptr->length,
        true
    );
}

// sim_string_insert(6): Inserts a new string at a given position.
void sim_string_insert(
    Sim_String *const string_ptr,
    size_t            old_string_length,
    const char*       old_string,
    size_t            new_string_length,
    const char*       new_string,
    size_t            index
) {
    if (!string_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 of is NULL", FUNCTION_NAME);

    if (!old_string)
        old_string_length = 0;
    if (!new_string)
        new_string_length = 0;
    
    if (index >= old_string_length)
        THROW(SIM_ERR_OUTOFBND, "(%s) String index out of bounds", FUNCTION_NAME);
    
    size_t new_len = new_string_length + old_string_length;
    if (new_len == 0) {
        _sim_string_construct(string_ptr, "", 0, false);
        return;
    }

    char* buffer = malloc(sizeof(char) * (new_len + 1));
    if (!buffer)
        THROW(SIM_ERR_OUTOFMEM,
            "(%s) Failed to allocate buffer for concatenated string",
            FUNCTION_NAME
        );
    memmove(buffer, old_string, index);
    memmove(buffer + index, new_string, new_string_length);
    buffer[new_len + 1] = '\0';

    _sim_string_construct(string_ptr, buffer, new_len, true);
}

#endif // SIMSOFT_STRING_C_
