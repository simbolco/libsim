/**
 * @file utf8.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for UTF-8 utility functions
 * @version 0.1
 * @date 2020-03-09
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_UTF8_H_
#define SIMSOFT_UTF8_H_

#include "./common.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

        /**
         * @fn uint32 sim_utf8_to_codepoint(1)
         * @brief Retrieves a UTF codepoint from a UTF-8 multi-byte sequence.
         * 
         * @param[in] utf8_char Pointer to the start of a UTF-8 multi-byte character.
         * 
         * @return @c (uint32)-1 if the given multi-byte sequence is invalid or on error (see
         *         remarks); the UTF codepoint corresponding to the given multi-byte sequence
         *         otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e utf8_char_ptr is @c NULL;
         *     @b SIM_RC_FAILURE     on an invalid UTF-8 multi-byte sequence;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT uint32 C_CALL sim_utf8_to_codepoint(
            const char* utf8_char_ptr
        );

        /**
         * @fn bool sim_utf8_from_codepoint(2)
         * @brief Retrieves a UTF-8 multi-byte sequence from a UTF codepoint.
         * 
         * @param[in] utf_codepoint   A UTF codepoint to convert to UTF-8.
         * @param[in] utf8_char_array A 4-entry char array to be filled with the UTF-8 character.
         * 
         * @return @c false if @e utf_codepoint is invalid or on error (see remarks);
         *         @c true otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_FAILURE if @e utf_codepoint is an invalid UTF codepoint;
         *     @b SIM_RC_SUCCESS otherwise.
         */
        extern EXPORT bool C_CALL sim_utf8_from_codepoint(
            uint32 utf_codepoint,
            char utf8_char_array[4]
        );

        /**
         * @fn size_t sim_utf8_get_codepoint(1)
         * @brief Retrieves the number of bytes of a given UTF-8 multi-byte character.
         * 
         * @param[in] utf8_char_ptr Pointer to the start of a UTF-8 multi-byte sequence.
         * 
         * @return 0 if the given multi-byte sequence is invalid or on error (see
         *         remarks); the number of bytes in the UTF-8 character pointed to by
         *         @e utf8_char_ptr othewrwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e utf8_char_ptr is @c NULL;
         *     @b SIM_RC_ERR_INVALARG if @e utf8_char_ptr is an invalid UTF-8 mutli-byte sequence;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT size_t C_CALL sim_utf8_get_char_size(
            const char* utf8_char_ptr
        );

        /**
         * @fn char* sim_utf8_next_char(1)
         * @brief Retrieves a pointer to the next UTF-8 character in a UTF-8 string.
         * 
         * @param[in] utf8_char_ptr Pointer to a UTF-8 multi-byte character.
         * 
         * @return Pointer to the next UTF-8 character in the given sequence.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e utf8_char_ptr is @c NULL;
         *     @b SIM_RC_ERR_INVALARG if @e utf8_char_ptr is an invalid UTF-8 mutli-byte sequence;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT char* C_CALL sim_utf8_next_char(
            char* utf8_char_ptr
        );

        /**
         * @fn size_t sim_utf8_strlen(1)
         * @brief Retrieves the length of a null-terminated UTF-8 string.
         * 
         * @param[in] utf8_string Null-terminated UTF-8 string to find the length of.
         * 
         * @return @c (size_t)-1 on error (see remarks); the number of UTF codepoints in the
         *         given string otherwise.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR  if @e utf8_string is @c NULL;
         *     @b SIM_RC_ERR_INVALARG if @e utf8_string is an invalid sequence;
         *     @b SIM_RC_SUCCESS      otherwise.
         */
        extern EXPORT size_t C_CALL sim_utf8_strlen(
            const char* utf8_string
        );
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        //

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_UTF8_H_ */
