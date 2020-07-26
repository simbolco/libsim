/**
 * @file utf8.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/utf8.h
 * @version 0.2
 * @date 2020-07-07
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_UTF8_C_
#define SIMSOFT_UTF8_C_

#include "simsoft/utf8.h"

#include "simsoft/except.h"

// sim_utf8_to_codepoint(1): Retrieves a UTF codepoint from a UTF-8 multi-byte sequence.
uint32 sim_utf8_to_codepoint(
    const char* utf8_char_ptr
) {
    if (!utf8_char_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    uint32 codepoint = (uint32)-1;

    if ((uint8)utf8_char_ptr[0] <= 0x7f)
        codepoint = (uint32)*utf8_char_ptr;
    else if ((utf8_char_ptr[0] & 0xe0) == 0xc0)
        codepoint = (((uint8)utf8_char_ptr[0] & 0x1f) << 6) +
                    ((uint8)utf8_char_ptr[1] & 0x3f)
        ;
    else if ( /* 0xD800 - 0xDFFF are invalid UTF8 codepoints */
        (uint8)utf8_char_ptr[0] == 0xed && ((uint8)utf8_char_ptr[1] & 0xa0) == 0xa0
    )
        THROW(SIM_ERR_INVALARG, "(%s) Invalid UTF-8 sequence", FUNCTION_NAME);
    else if ((utf8_char_ptr[0] & 0xf0) == 0xe0)
        codepoint = (((uint8)utf8_char_ptr[0] & 0x0f) << 12) +
                    (((uint8)utf8_char_ptr[1] & 0x3f) << 6)  +
                    ((uint8)utf8_char_ptr[2] & 0x3f)
        ;
    else if ((utf8_char_ptr[0] & 0xF8) == 0xf0)
        codepoint = (((uint8)utf8_char_ptr[0] & 0x07) << 18) +
                    (((uint8)utf8_char_ptr[1] & 0x3f) << 12) +
                    (((uint8)utf8_char_ptr[2] & 0x3f) << 6)  +
                    ((uint8)utf8_char_ptr[3] & 0x3f)
        ;
    else
        THROW(SIM_ERR_INVALARG, "(%s) Invalid UTF-8 sequence", FUNCTION_NAME);

    return codepoint;
}

// sim_utf8_from_codepoint(2): Retrieves a UTF-8 multi-byte sequence from a UTF codepoint.
void sim_utf8_from_codepoint(
    uint32 utf_codepoint,
    char utf8_char_array[4]
) {
    if (utf_codepoint <= 0x7f) {
        utf8_char_array[0] = (char)utf_codepoint;
    } else if (utf_codepoint <= 0x7ff) {
        utf8_char_array[0] = 0xc0 | (char)((utf_codepoint >> 6) & 0x1f);
        utf8_char_array[1] = 0x80 | (char)(utf_codepoint & 0x3f);
    } else if (utf_codepoint <= 0xffff) {
        utf8_char_array[0] = 0xe0 | (char)((utf_codepoint >> 12) & 0x0f);
        utf8_char_array[1] = 0x80 | (char)((utf_codepoint >> 6) & 0x3f);
        utf8_char_array[2] = 0x80 | (char)(utf_codepoint & 0x3f);
    } else if (utf_codepoint <= 0x10ffff) {
        utf8_char_array[0] = 0xf0 | (char)((utf_codepoint >> 18) & 0x07);
        utf8_char_array[1] = 0x80 | (char)((utf_codepoint >> 12) & 0x3f);
        utf8_char_array[2] = 0x80 | (char)((utf_codepoint >> 6) & 0x3f);
        utf8_char_array[3] = 0x80 | (char)(utf_codepoint & 0x3f);
    } else
        THROW(SIM_ERR_INVALARG, "(%s) Invalid UTF codepoint", FUNCTION_NAME);
}

// sim_utf8_get_char_size(1): Retrieves the number of bytes of a given UTF-8 multi-byte character.
size_t sim_utf8_get_char_size(const char* utf8_char_ptr) {
    if (!utf8_char_ptr)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);
    
    if (*(const uint8*)utf8_char_ptr <= 0x7f)
        return 1;
    else if ((*utf8_char_ptr & 0xe0) == 0xc0)
        return 2;
    else if ((*utf8_char_ptr & 0xf0) == 0xe0)
        return 3;
    else if ((*utf8_char_ptr & 0xf8) == 0xf0)
        return 4;
    
    THROW(SIM_ERR_INVALARG, "(%s) Invalid UTF-8 sequence", FUNCTION_NAME);
}

// sim_utf8_get_codepoint_size(1): Retrieves the number of bytes needed to store a codepoint as a
//                                 UTF-8 multibyte sequence.
size_t sim_utf8_get_codepoint_size(uint32 utf_codepoint) {
    if (utf_codepoint < 0x7f)
        return 1;
    else if (utf_codepoint <= 0x7ff)
        return 2;
    else if (utf_codepoint >= 0xd800 && utf_codepoint <= 0xdfff)
        THROW(SIM_ERR_INVALARG, "(%s) Invalid UTF codepoint", FUNCTION_NAME);
    else if (utf_codepoint <= 0xffff)
        return 3;
    else if (utf_codepoint <= 0x10ffff)
        return 4;

    THROW(SIM_ERR_INVALARG, "(%s) Invalid UTF codepoint", FUNCTION_NAME);
}

// sim_utf8_next_char(1): Retrieves a pointer to the next UTF-8 character in a UTF-8 string.
char* sim_utf8_next_char(char* utf8_char_ptr) {
    return utf8_char_ptr + sim_utf8_get_char_size(utf8_char_ptr);
}

// sim_utf8_strlen(1): Retrieves the length of a null-terminated UTF-8 string.
size_t sim_utf8_strlen(const char* utf8_string) {
    if (!utf8_string)
        THROW(SIM_ERR_NULLPTR, "(%s) Argument 0 is NULL", FUNCTION_NAME);

    size_t count = (size_t)-1, change;
    do {
        count++;
        change = sim_utf8_get_char_size(utf8_string);
        utf8_string += change;
    } while (*utf8_string && change);

    return count;
}

#endif // SIMSOFT_UTF8_C_
