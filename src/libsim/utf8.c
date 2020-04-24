/**
 * @file utf8.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/utf8.h
 * @version 0.1
 * @date 2020-03-09
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_UTF8_C_
#define SIMSOFT_UTF8_C_

#include "simsoft/utf8.h"
#include "./_internal.h"

// sim_utf8_to_codepoint(1): Retrieves a UTF codepoint from a UTF-8 multi-byte sequence.
uint32 sim_utf8_to_codepoint(
    const char* utf8_char_ptr
) {
    if (!utf8_char_ptr)
        THROW(SIM_RC_ERR_NULLPTR);

    uint32 codepoint = (uint32)-1;

    if ((uint8)utf8_char_ptr[0] <= 0x7F)
        codepoint = (uint32)*utf8_char_ptr;
    else if ((utf8_char_ptr[0] & 0xE0) == 0xC0)
        codepoint = ((uint8)utf8_char_ptr[0] & 0x1F) * 64 +
                    ((uint8)utf8_char_ptr[1] & 0x3F)
        ;
    else if ( /* 0xD800 - 0xDFFF are invalid UTF8 codepoints */
        (uint8)utf8_char_ptr[0] == 0xED && ((uint8)utf8_char_ptr[1] & 0xA0) == 0xA0
    )
        THROW(SIM_RC_ERR_INVALARG);
    else if ((utf8_char_ptr[0] & 0xF0) == 0xE0)
        codepoint = ((uint8)utf8_char_ptr[0] & 0x0F) * 4096 +
                    ((uint8)utf8_char_ptr[1] & 0x3F) * 64   +
                    ((uint8)utf8_char_ptr[2] & 0x3F)
        ;
    else if ((utf8_char_ptr[0] & 0xF8) == 0xF0)
        codepoint = ((uint8)utf8_char_ptr[0] & 0x07) * 262144 +
                    ((uint8)utf8_char_ptr[1] & 0x3F) * 4096   +
                    ((uint8)utf8_char_ptr[2] & 0x3F) * 64     +
                    ((uint8)utf8_char_ptr[3] & 0x3F)
        ;
    else
        THROW(SIM_RC_ERR_INVALARG);

    RETURN(SIM_RC_SUCCESS, codepoint);
}

// sim_utf8_from_codepoint(2): Retrieves a UTF-8 multi-byte sequence from a UTF codepoint.
bool sim_utf8_from_codepoint(
    uint32 utf_codepoint,
    char utf8_char_array[4]
) {
    if (utf_codepoint <= 0x7F) {
        utf8_char_array[0] = (char)utf_codepoint;
    } else if (utf_codepoint <= 0x7FF) {
        utf8_char_array[0] = 0xC0 | (char)((utf_codepoint >> 6) & 0x1F);
        utf8_char_array[1] = 0x80 | (char)(utf_codepoint & 0x3F);
    } else if (utf_codepoint <= 0xFFFF) {
        utf8_char_array[0] = 0xE0 | (char)((utf_codepoint >> 12) & 0x0F);
        utf8_char_array[1] = 0x80 | (char)((utf_codepoint >> 6) & 0x3F);
        utf8_char_array[2] = 0x80 | (char)(utf_codepoint & 0x3F);
    } else if (utf_codepoint <= 0x10FFFF) {
        utf8_char_array[0] = 0xE0 | (char)((utf_codepoint >> 18) & 0x0F);
        utf8_char_array[1] = 0x80 | (char)((utf_codepoint >> 12) & 0x3F);
        utf8_char_array[2] = 0x80 | (char)((utf_codepoint >> 6) & 0x3F);
        utf8_char_array[3] = 0x80 | (char)(utf_codepoint & 0x3F);
    } else
        THROW(SIM_RC_ERR_INVALARG);
    RETURN(SIM_RC_SUCCESS, true);
}

// sim_utf8_get_char_size(1): Retrieves the number of bytes of a given UTF-8 multi-byte character.
size_t sim_utf8_get_char_size(const char* utf8_char_ptr) {
    if (!utf8_char_ptr)
        THROW(SIM_RC_ERR_NULLPTR);
    
    if (*(const uint8*)utf8_char_ptr <= 0x7F)
        RETURN(SIM_RC_SUCCESS, 1);
    else if ((*utf8_char_ptr & 0xE0) == 0xC0)
        RETURN(SIM_RC_SUCCESS, 2);
    else if ((*utf8_char_ptr & 0xF0) == 0xE0)
        RETURN(SIM_RC_SUCCESS, 3);
    else if ((*utf8_char_ptr & 0xF8) == 0xF0)
        RETURN(SIM_RC_SUCCESS, 4);
    
    THROW(SIM_RC_ERR_INVALARG);
}

// sim_utf8_next_char(1): Retrieves a pointer to the next UTF-8 character in a UTF-8 string.
char* sim_utf8_next_char(char* utf8_char_ptr) {
    return utf8_char_ptr + sim_utf8_get_char_size(utf8_char_ptr);
}

// sim_utf8_strlen(1): Retrieves the length of a null-terminated UTF-8 string.
size_t sim_utf8_strlen(const char* utf8_string) {
    if (!utf8_string)
        THROW(SIM_RC_ERR_NULLPTR);

    size_t count = 0, change;
    while (*utf8_string) {
        change = sim_utf8_get_char_size(utf8_string);
        if (change == 0)
            THROW(SIM_RC_ERR_INVALARG);

        utf8_string += change;
        count++;
    }

    RETURN(SIM_RC_SUCCESS, count);
}

#endif /* SIMSOFT_UTF8_C_ */
