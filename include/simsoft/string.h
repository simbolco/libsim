/**
 * @file string.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for strings & related functions
 * @version 0.1
 * @date 2020-03-31
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_STRING_H_
#define SIMSOFT_STRING_H_

#include "./common.h"
#include "./allocator.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

// == BASIC STRING =================================================================================

#       ifndef SIM_STRING_INTERNAL_CAPACITY
#           define SIM_STRING_INTERNAL_CAPACITY 16
#       endif

        /**
         * @struct Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Basic string type
         * 
         * @tparam _allocator_ptr Pointer to allocator used when allocating large strings.
         * @tparam _hash_proc     Function pointer for the hash function for this string to use.
         * 
         * @property c_string Pointer to the array containing the string.
         * @property length   The number of chars in the string.
         */
        typedef struct Sim_String {
            char*  c_string;
            size_t length;
            
            // the last byte of _internal_data is a flag bit:
            //   0 = small string + null terminator;
            //   1 = large string
            union {
                size_t _allocated;
                struct {
                    char _internal_data[SIM_STRING_INTERNAL_CAPACITY - 1];
                    char _is_large_string;
                };
            };

            const Sim_IAllocator *const _allocator_ptr;
            Sim_HashProc _hash_proc;

            Sim_HashType _hash1;
            Sim_HashType _hash2;
            bool _hash_dirty;
        } Sim_String;

        /**
         * @fn void sim_string_construct(5)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Constructs a string.
         * 
         * @param[in,out] string_ptr      Pointer to an empty string to construct.
         * @param[in]     allocator_ptr   Pointer to an allocator to use for large strings.
         * @param[in]     hash_proc       The hash function to use when hashing the string.
         * @param[in]     c_string_length The number of chars in @e c_string.
         * @param[in]     c_string        A C string.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e string_ptr is @c NULL;
         *     @b SIM_RC_ERR_OUTOFMEM if @e c_string is a large string and couldn't be allocated;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @remarks If @e c_string is @c NULL or @e c_string_length equals 0, an empty string will
         *          be allocated.
         * @remarks If @e hash_proc is @c NULL, sim_string_default_hash will be used.
         * 
         * @sa sim_string_construct_format
         * @sa sim_string_move
         * @sa sim_string_destroy
         */
        extern EXPORT void C_CALL sim_string_construct(
            Sim_String *const     string_ptr,
            const Sim_IAllocator* allocator_ptr,
            Sim_HashProc          hash_proc,
            size_t                c_string_length,
            const char*           c_string
        );

        /**
         * @fn void sim_string_construct_format(5+)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Constructs a string given a format string.
         * 
         * @param[in,out] string_ptr    Pointer to an empty string to construct.
         * @param[in]     allocator_ptr Pointer to an allocator to use for large strings.
         * @param[in]     hash_proc     The hash function to use when hashing the string.
         * @param[in]     string_length The length of the string to be constructed.
         * @param[in]     format_string A format string.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e string_ptr is @c NULL;
         *     @b SIM_RC_ERR_OUTOFMEM if the unformatted string is a large string and couldn't be
         *                             allocated;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @remarks If @e string_length equals 0 and @e format_string != @c NULL, a string 
         *          will be allocated based on how large the deformatted string is.
         * @remarks If @e format_string is @c NULL, an empty string will be
         *          allocated.
         * @remarks If @e hash_proc is @c NULL, an internal function using SipHash will be used.
         * 
         * @sa sim_string_construct
         * @sa sim_string_move
         * @sa sim_string_destroy
         */
        extern EXPORT void C_CALL sim_string_construct_format(
            Sim_String *const     string_ptr,
            const Sim_IAllocator* allocator_ptr,
            Sim_HashProc          hash_proc,
            size_t                string_length,
            const char*           format_string,
            ...
        );

        /**
         * @fn void sim_string_move(2)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Moves the contents of one string to another.
         * 
         * @param[in,out] string_from_ptr Pointer to a string to take contents from.
         * @param[in,out] string_to_ptr   Pointer to a string to move contents into.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e string_from_ptr or @e string_to_ptr are @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if the vector size requested couldn't be allocated;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @sa sim_string_construct
         * @sa sim_string_construct_format
         * @sa sim_string_destroy
         */
        extern EXPORT void C_CALL sim_string_move(
            Sim_String *const string_from_ptr,
            Sim_String *const string_to_ptr
        );

        /**
         * @fn void sim_string_destroy(1)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Destroys a string.
         * 
         * @param[in,out] string_ptr Pointer to a string to destroy.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e string_ptr is @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @sa sim_string_construct
         * @sa sim_string_construct_format
         * @sa sim_string_move
         */
        extern EXPORT void C_CALL sim_string_destroy(
            Sim_String *const string_ptr
        );

        /**
         * @fn void sim_string_is_empty(1)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Checks if a vector is empty.
         * 
         * @param[in,out] string_ptr Pointer to a string to check.
         * 
         * @returns @c true if empty; @c false otherwise (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e string_ptr is @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT bool C_CALL sim_string_is_empty(
            Sim_String *const string_ptr
        );

        /**
         * @fn size_t sim_string_get_allocated(1)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Retrieves the number of bytes in memory that is allocated by the string.
         * 
         * @param[in,out] string_ptr Pointer to a string to check.
         * 
         * @returns The number of bytes @c string_ptr->c_string takes up; -1 on error (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e string_ptr is @c NULL ;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT size_t C_CALL sim_string_get_allocated(
            Sim_String *const string_ptr
        );

        /**
         * @fn Sim_HashType sim_string_get_hash(2)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Retrieves the hash value of a string.
         * 
         * @param[in,out] string_ptr Pointer to a string to check.
         * @param[in]     attempt    The hashing attempt.
         * 
         * @returns 0 if @e string_ptr is @c NULL; the hash value of the given string otherwise.
         *          sim_return_code() is not set.
         */
        extern EXPORT Sim_HashType C_CALL sim_string_get_hash(
            Sim_String *const string_ptr,
            const size_t      attempt
        );

        /**
         * @fn bool sim_string_insert(4)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Inserts a new string at a given position.
         * 
         * @param[in,out] string_ptr        Pointer to a string to insert the new string into.
         * @param[in]     new_string_length The length of the to-be inserted string.
         * @param[in]     new_string        The string to be inserted.
         * @param[in]     index             Where in @e string_ptr to insert @e new_string.
         * 
         * @returns @c true if successful; @c false otherwise (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e string_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if @e string_ptr couldn't be resized;
         *     @b SIM_RC_ERR_OUTOFBND if @e index > @c string_ptr->length;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @remarks if @e new_string is @c NULL, nothing happens and is considered a success.
         */
        extern EXPORT bool C_CALL sim_string_insert(
            Sim_String *const string_ptr,
            const size_t      new_string_length,
            const char*       new_string,
            const size_t      index
        );

        /**
         * @fn bool sim_string_append(3)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Appends a new string to the back of a string.
         * 
         * @param[in,out] string_ptr        Pointer to a string to insert the new string into.
         * @param[in]     new_string_length The length of the to-be inserted string.
         * @param[in]     new_string        The string to be appended.
         * 
         * @returns @c true if successful; @c false otherwise (see remarks).
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e string_ptr is @c NULL ;
         *     @b SIM_RC_ERR_OUTOFMEM if @e string_ptr couldn't be resized;
         *     @b SIM_RC_SUCCESS      otherwise.
         * 
         * @remarks if @e new_string is @c NULL, nothing happens and is considered a success.
         */
        extern EXPORT bool C_CALL sim_string_append(
            Sim_String *const string_ptr,
            const size_t      new_string_length,
            const char*       new_string
        );

        /**
         * @fn void sim_string_remove(3)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Removes a section of a string.
         * 
         * @param[in,out] string_ptr Pointer to a string to remove a section from. 
         * @param[in]     index      The index to start removing from.
         * @param[in]     length     How many chars to remove from the string.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e string_ptr is @c NULL;
         *     @b SIM_RC_ERR_OUTOFBND if @e index or @e index + @e length >= @c string_ptr->length;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT void C_CALL sim_string_remove(
            Sim_String *const string_ptr,
            const size_t      index,
            const size_t      length
        );

        /**
         * @fn size_t sim_string_find(4)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Finds a substring within a string.
         * 
         * @param[in,out] string_ptr       Pointer to the string to find within.
         * @param[in]     substring_length Length of @e substring.
         * @param[in]     substring        The substring to find.
         * @param[in]     starting_index   The index to start searching from.
         * 
         * @returns (size_t)-1 on error/failure (see remarks); the index of the substring otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e string_ptr or @e substring are @c NULL;
         *     @b SIM_RC_ERR_OUTOFBND if @e starting_index >= @c string_ptr->length;
         *     @b SIM_RC_FAILURE      if @e substring wasn't contained in @e string_ptr;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT size_t C_CALL sim_string_find(
            Sim_String *const string_ptr,
            const size_t      substring_length,
            const char*       substring,
            const size_t      starting_index
        );

        /**
         * @fn size_t sim_string_replace(6)
         * @relates Sim_String
         * @headerfile string.h "simsoft/string.h"
         * @brief Replaces a substring within a string with another string.
         * 
         * @param[in,out] string_ptr            Pointer to the string to replace within.
         * @param[in]     find_string_length    Length of @e find_string.
         * @param[in]     find_string           The substring to replace.
         * @param[in]     replace_string_length Length of @e replace_string.
         * @param[in]     replace_string        The string to replace @e find_string.
         * @param[in]     starting_index        The index to start searching from.
         * 
         * @returns (size_t)-1 on error/failure (see remarks); the index where @e replace_string was
         *          inserted into otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e string_ptr, @e find_string, or @e replace_string are
         *                            @c NULL;
         *     @b SIM_RC_ERR_OUTOFBND if @e starting_index >= @c string_ptr->length;
         *     @b SIM_RC_ERR_OUTOFMEM if @e string_ptr couldn't be resized to fit @e replace_string
         *                            in place of @e find_string;
         *     @b SIM_RC_FAILURE      if @e find_string wasn't contained in @e string_ptr;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT size_t C_CALL sim_string_replace(
            Sim_String *const string_ptr,
            const size_t      find_string_length,
            const char*       find_string,
            const size_t      replace_string_length,
            const char*       replace_string,
            const size_t      starting_index
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        //

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_STRING_H_ */
