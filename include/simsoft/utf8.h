/**
 * @file utf8.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for UTF-8 utility functions
 * @version 0.2
 * @date 2020-07-07
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_UTF8_H_
#define SIMSOFT_UTF8_H_

#include "macro.h"
#include "typedefs.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API

        /**
         * @brief Retrieves a UTF codepoint from a UTF-8 multi-byte sequence.
         * 
         * @param[in] utf8_char Pointer to the start of a UTF-8 multi-byte character.
         * 
         * @returns @c (uint32)-1 if the given multi-byte sequence is invalid; the UTF codepoint
         *          corresponding to the given multi-byte sequence
         *          otherwise.
         * 
         * @throw SIM_ERR_NULLPTR  if @e utf8_char_ptr is @c NULL;
         * @throw SIM_ERR_INVALARG if @e utf8_char_ptr is an invalid UTF-8 multi-byte sequence.
         * 
         * @sa sim_utf8_from_codepoint
         */
        DYNAPI_PROC(uint32, sim_utf8_to_codepoint,,
            const char* utf8_char_ptr
        );

        /**
         * @brief Retrieves a UTF-8 multi-byte sequence from a UTF codepoint.
         * 
         * @param[in] utf_codepoint   A UTF codepoint to convert to UTF-8.
         * @param[in] utf8_char_array A 4-entry char array to be filled with the UTF-8 character.
         * 
         * @throw SIM_ERR_INVALARG if @e utf_codepoint is an invalid UTF codepoint.
         * 
         * @sa sim_utf8_from_codepoint
         */
        DYNAPI_PROC(void, sim_utf8_from_codepoint,,
            uint32 utf_codepoint,
            char utf8_char_array[4]
        );

        /**
         * @brief Retrieves the number of bytes of a given UTF-8 multi-byte character.
         * 
         * @param[in] utf8_char_ptr Pointer to the start of a UTF-8 multi-byte sequence.
         * 
         * @returns The number of bytes in the UTF-8 character pointed to by @e utf8_char_ptr.
         * 
         * @throw SIM_ERR_NULLPTR  if @e utf8_char_ptr is @c NULL;
         * @throw SIM_ERR_INVALARG if @e utf8_char_ptr is an invalid UTF-8 multi-byte sequence.
         */
        DYNAPI_PROC(size_t, sim_utf8_get_char_size,,
            const char* utf8_char_ptr
        );

        /**
         * @brief Retrieves the number of bytes to store a UTF codepoint as a multibyte sequence.
         * 
         * @param[in] utf_codepoint A UTF-8 codepoint.
         * 
         * @returns 0 if the given codepoint is invalid; the number of bytes needed to store
         *          @e codepoint in UTF-8 otherwise.
         */
        DYNAPI_PROC(size_t, sim_utf8_get_codepoint_size,,
            uint32 utf_codepoint
        );

        /**
         * @brief Retrieves a pointer to the next UTF-8 character in a UTF-8 string.
         * 
         * @param[in] utf8_char_ptr Pointer to a UTF-8 multi-byte character.
         * 
         * @returns Pointer to the next UTF-8 character in the given sequence.
         * 
         * @throw SIM_ERR_NULLPTR  if @e utf8_char_ptr is @c NULL;
         * @throw SIM_ERR_INVALARG if @e utf8_char_ptr is an invalid UTF-8 multi-byte sequence.
         */
        DYNAPI_PROC(char*, sim_utf8_next_char,,
            char* utf8_char_ptr
        );

        /**
         * @brief Retrieves the length of a null-terminated UTF-8 string.
         * 
         * @param[in] utf8_string Null-terminated UTF-8 string to find the length of.
         * 
         * @returns The number of UTF codepoints in the given string.
         * 
         * @throw SIM_ERR_NULLPTR  if @e utf8_char_ptr is @c NULL;
         * @throw SIM_ERR_INVALARG if @e utf8_char_ptr is an invalid UTF-8 multi-byte sequence.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e utf8_string is @c NULL;
         *     @b SIM_RC_ERR_INVALARG if @e utf8_string is an invalid sequence;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        DYNAPI_PROC(size_t, sim_utf8_strlen,,
            const char* utf8_string
        );
    
    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus // start C++ API
        
        //

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft) // end SimSoft namespace

#endif // SIMSOFT_UTF8_H_
