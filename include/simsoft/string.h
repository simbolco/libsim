/**
 * @file string.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for strings & related functions
 * @version 0.2
 * @date 2020-07-26
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_STRING_H_
#define SIMSOFT_STRING_H_

#include "macro.h"
#include "typedefs.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API

// == BASIC STRING =================================================================================

#       ifndef SIM_STRING_INTERNAL_CAPACITY
#           define SIM_STRING_INTERNAL_CAPACITY (16)
#       endif

        /**
         * @brief Immutable string type
         */
        typedef struct Sim_String {
            const char* c_string;   ///< Pointer to the array containing the string.
            size_t length;          ///< The number of characters in the string.

            Sim_HashType hash;      ///< hash code.

            bool _owns_string;
        } Sim_String;

        /**
         * @relates @capi{Sim_String}
         * @brief Constructs a new string.
         * 
         * @param[in,out] string_ptr  Pointer to an empty string to construct.
         * @param[in]     c_string    A C string.
         * @param[in]     owns_string @c true if @e c_string should be freed when @e string_ptr
         *                            is destroyed.
         * 
         * @throw SIM_ERR_NULLPTR  if @e string_ptr is @c NULL
         * 
         * @remarks If @e c_string is @c NULL, an empty string will be created.
         * 
         * @sa sim_string_construct_format
         * @sa sim_string_copy
         * @sa sim_string_move
         * @sa sim_string_destroy
         */
        DYNAPI_PROC(void, sim_string_construct,,
            Sim_String *const string_ptr,
            const char*       c_string,
            bool              owns_string
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Constructs a new string using a format string.
         * 
         * @param[in,out] string_ptr    Pointer to an empty string to construct.
         * @param[in]     format_string A format string.
         * @param[in]     ...           Format string parameters.
         * 
         * @throw SIM_ERR_NULLPTR  if @e string_ptr is @c NULL.
         * @throw SIM_ERR_OUTOFMEM if the generated format string couldn't be allocated.
         * 
         * @remarks If @e string_length equals 0 and @e format_string != @c NULL, a string 
         *          will be allocated based on how large the deformatted string is.
         * @remarks If @e format_string is @c NULL, an empty string will be created.
         * 
         * @sa sim_string_construct
         * @sa sim_string_copy
         * @sa sim_string_move
         * @sa sim_string_destroy
         */
        DYNAPI_PROC(void, sim_string_construct_format, PRINTF_SPEC(2, 3),
            Sim_String *const string_ptr,
            const char*       format_string,
            ...
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Copies the contents of one string into another.
         * 
         * @param[in,out] string_from_ptr Pointer to a string to copy contents from.
         * @param[in,out] string_to_ptr   Pointer to a string to paste contents into.
         * 
         * @throw SIM_ERR_NULLPTR  if @e string_from_ptr or @e string_to_ptr are @c NULL.
         * @throw SIM_ERR_OUTOFMEM if @e string_from_ptr couldn't be duplicated.
         * 
         * @sa sim_string_construct
         * @sa sim_string_construct_format
         * @sa sim_string_move
         * @sa sim_string_destroy
         */
        DYNAPI_PROC(void, sim_string_copy,,
            Sim_String *const RESTRICT string_from_ptr,
            Sim_String *const RESTRICT string_to_ptr
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Moves the contents of one string to another.
         * 
         * @param[in,out] string_from_ptr Pointer to a string to take contents from.
         * @param[in,out] string_to_ptr   Pointer to a string to move contents into.
         * 
         * @throw SIM_ERR_NULLPTR if @e string_from_ptr or @e string_to_ptr are @c NULL.
         * 
         * @sa sim_string_construct
         * @sa sim_string_construct_format
         * @sa sim_string_move
         * @sa sim_string_destroy
         */
        DYNAPI_PROC(void, sim_string_move,,
            Sim_String *const RESTRICT string_from_ptr,
            Sim_String *const RESTRICT string_to_ptr
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Destroys a string.
         * 
         * @param[in,out] string_ptr Pointer to a string to destroy.
         * 
         * @throw SIM_ERR_NULLPTR if @e string_ptr is @c NULL.
         * 
         * @sa sim_string_construct
         * @sa sim_string_construct_format
         * @sa sim_string_copy
         * @sa sim_string_move
         */
        DYNAPI_PROC(void, sim_string_destroy,,
            Sim_String *const string_ptr
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Checks if a string is empty.
         * 
         * @param[in,out] string_ptr Pointer to a string to check.
         * 
         * @returns @c true if empty; @c false otherwise.
         * 
         * @throw SIM_ERR_NULLPTR if @e string_ptr is @c NULL.
         */
        DYNAPI_PROC(bool, sim_string_is_empty,,
            Sim_String *const string_ptr
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Concatenates two strings together, placing the result into a third string.
         * 
         * @param[in]     left_string_ptr  Pointer to left-hand string. 
         * @param[in]     right_string_ptr Pointer to right-hand string.
         * @param[in,out] string_to_ptr    Pointer to an empty string to populate.
         * 
         * @throw SIM_ERR_NULLPTR if @e left_string_ptr, @e right_string_ptr, or @e string_to_ptr
         *                        are @c NULL.
         */
        DYNAPI_PROC(void, sim_string_concat,,
            Sim_String *const          left_string_ptr,
            Sim_String *const          right_string_ptr,
            Sim_String *const RESTRICT string_to_ptr 
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Inserts a new string at a given position.
         * 
         * @param[in,out] string_ptr        Pointer to a string to insert the combined string into.
         * @param[in]     old_string_length The length of @e old_string.
         * @param[in]     old_string        The string to insert @e new_string into.
         * @param[in]     new_string_length The length of the to-be inserted string.
         * @param[in]     new_string        The string to be inserted.
         * @param[in]     index             Where in @e string_ptr to insert @e new_string.
         * 
         * @throw SIM_ERR_NULLPTR  if @e string_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFBND if @e index > @e old_string_length;
         * @throw SIM_ERR_OUTOFMEM if @e string_ptr couldn't be resized.
         * 
         * @remarks if @e new_string is @c NULL, @e old_string is copied into @e string_ptr.
         * @remarks if @e old_string is @c NULL, @e new_string is copied into @e string_ptr.
         */
        DYNAPI_PROC(void, sim_string_insert,,
            Sim_String *const string_ptr,
            size_t            old_string_length,
            const char*       old_string,
            size_t            new_string_length,
            const char*       new_string,
            size_t            index
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Removes a section of a string.
         * 
         * @param[in,out] string_ptr        Pointer to a string to insert the sliced string into.
         * @param[in]     old_string_length The length of @e old_string.
         * @param[in]     old_string        The string to be sliced.
         * @param[in]     index             The index to start removing from.
         * @param[in]     length            How many chars to remove from the string.
         * 
         * @throw SIM_ERR_NULLPTR  if @e string_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFBND if @e index or @e index + @e length >= @e old_string_length.
         * @throw SIM_ERR_OUTOFMEM
         * 
         * @remarks if @e old_string is @c NULL, a blank string is copied into @e string_ptr.
         * @remarks if @e index + @e length is 0, @e old_string is copied into @e string_ptr.
         * 
         * @warning This function is currently unimplemented.
         */
        DYNAPI_PROC(void, sim_string_remove,,
            Sim_String *const string_ptr,
            size_t            old_string_length,
            const char*       old_string,
            size_t            index,
            size_t            length
        );

        /**
         * @relates @capi{Sim_String}
         * @brief Replaces a substring within a string with another string.
         * 
         * @param[in,out] string_ptr            Pointer to a string to insert the substituted
         *                                      string into.
         * @param[in]     source_string_length  Length of @e source_string.
         * @param[in]     source_string         The string to substitute substrings from.
         * @param[in]     find_string_length    Length of @e find_string.
         * @param[in]     find_string           The substring to replace.
         * @param[in]     replace_string_length Length of @e replace_string.
         * @param[in]     replace_string        The string to replace @e find_string.
         * @param[in]     starting_index        The index to start replacing from.
         * 
         * @returns The index where @e replace_string was inserted into;
         *          @c (size_t)-1 if no substitutions were made.
         * 
         * @throw SIM_ERR_NULLPTR  if @e string_ptr is @c NULL;
         * @throw SIM_ERR_OUTOFBND if @e starting_index >= @e source_string_length;
         * @throw SIM_ERR_OUTOFMEM if @e string_ptr couldn't be resized to fit @e replace_string
         *                            in place of @e find_string.
         * 
         * @warning This function is currently unimplemented.
         */
        DYNAPI_PROC(size_t, sim_string_replace,,
            Sim_String *const string_ptr,
            size_t            source_string_length,
            const char*       source_string,
            size_t            find_string_length,
            const char*       find_string,
            size_t            replace_string_length,
            const char*       replace_string,
            size_t            starting_index
        );
    
    CPP_NAMESPACE_C_API_END // end C API
#   ifdef __cplusplus       // start C++ API
        
        //

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft)

#endif // SIMSOFT_STRING_H_
