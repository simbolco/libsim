/**
 * @file macro.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header containing convenience macros.
 * @version 0.2
 * @date 2020-07-07
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

/**
 * @defgroup typedefs
 * @brief General-purpose C macros
 * @par
 * The SimSoft Library defines various general
 */

#ifndef SIMSOFT_TYPEDEFS_H_
#define SIMSOFT_TYPEDEFS_H_

#include "limits.h"
#ifndef __cplusplus
#   include <stdbool.h>
#endif // C bool
#include <stddef.h>
#include <stdint.h>

#include "macro.h"

CPP_NAMESPACE_START(SimSoft)

    /**
     * @ingroup typedefs
     * @brief Signed 8-bit integer.
     */
    typedef int8_t sint8;
    /**
     * @ingroup typedefs
     * @brief Unsigned 8-bit integer.
     */
    typedef uint8_t uint8;
    /**
     * @ingroup typedefs
     * @brief Signed 16-bit integer.
     */
    typedef int16_t sint16;
    /**
     * @ingroup typedefs
     * @brief Unsigned 16-bit integer.
     */
    typedef uint16_t uint16;
    /**
     * @ingroup typedefs
     * @brief Signed 32-bit integer.
     */
    typedef int32_t sint32;
    /**
     * @ingroup typedefs
     * @brief Unsigned 32-bit integer.
     */
    typedef uint32_t uint32;
    /**
     * @ingroup typedefs
     * @brief Signed 64-bit integer.
     */
    typedef int64_t sint64;
    /**
     * @ingroup typedefs
     * @brief Unsigned 64-bit integer.
     */
    typedef uint64_t uint64;

    CPP_NAMESPACE_C_API_START // Start C_API
        /**
         * @ingroup typedefs
         * @brief Union of different C data types.
         */
        typedef union Sim_Variant {
            // Sized signed integer types
            sint8  signed8;  ///< 8-bit signed integral.
            sint16 signed16; ///< 16-bit signed integral.
            sint32 signed32; ///< 32-bit signed integral.
#           if WORD_SIZE == 64
                sint64 signed64; ///< 64-bit signed integral.
#           endif // WORD_SIZE_64
            
            // Sized unsigned integer types
            uint8  unsigned8;  ///< 8-bit unsigned integral.
            uint16 unsigned16; ///< 16-bit unsigned integral.
            uint32 unsigned32; ///< 32-bit unsigned integral.
#           if WORD_SIZE == 64
                uint64 unsigned64; ///< 64-bit unsigned integral.
#           endif // WORD_SIZE_64

            // Floating point types
            float float32; ///< 32-bit floating-point value.
#           if WORD_SIZE == 64
                double float64; ///< 64-bit floating-point value.
#           endif // WORD_SIZE_64

            // C integral types
            signed char signed_char;   ///< C signed char.
            signed short signed_short; ///< C signed short.
            signed int signed_int;     ///< C signed int.
            signed long signed_long;   ///< C signed long.
#           if (LLONG_MAX > 0x7FFFFFFFl && (WORD_SIZE == 64)) || \
               (LLONG_MAX <= 0x7FFFFFFFl && (WORD_SIZE == 32))
                long long signed_long_long; ///< C signed long long.
#           endif // LLONG_MAX

            // Unsigned C integral types
            unsigned char unsigned_char;   ///< C unsigned char.
            unsigned short unsigned_short; ///< C unsigned short.
            unsigned int unsigned_int;     ///< C unsigned int.
            unsigned long unsigned_long;   ///< C unsigned long.
#           if (ULLONG_MAX > 0xFFFFFFFFul && (WORD_SIZE == 64)) || \
               (ULLONG_MAX <= 0xFFFFFFFFul && (WORD_SIZE == 32))
                unsigned long long unsigned_long_long; ///< C unsigned long long.
#           endif // ULLONG_MAX

            // Size types
#           if defined(SSIZE_MAX) || defined(_SSIZE_T_DEFINED)
                ssize_t signed_size; ///< Signed size type.
#           endif // SSIZE_MAX
#           if defined(SIZE_MAX) || defined(_SIZE_T_DEFINED)
                size_t  unsigned_size; ///< Unsigned size type.
#           endif // SIZE_MAX

            // Pointer types
            char* string;  ///< C string.
            void* pointer; ///< Void pointer.
            
            // Pointer math types
#           if defined(UINTPTR_MAX) || defined(_UINTPTR_T_DEFINED) || defined(__uintptr_t_defined)
                uintptr_t upointer; ///< Unsigned integer pointer type.
#           endif // UINTPTR_MAX
#           if defined(INTPTR_MAX) || defined(_INTPTR_T_DEFINED) || defined(__intptr_t_defined)
                intptr_t spointer; ///< Signed integer pointer type.
#           endif // INTPTR_MAX
#           if defined(PTRDIFF_MAX) || defined(_PTRDIFF_T_DEFINED) || defined(_PTRDIFF_T_)
                ptrdiff_t ptr_diff; ///< Pointer difference type.
#           endif // PTRDIFF_MAX

            struct {
#           if WORD_SIZE == 32
                uint32 bit_0  : 1;
                uint32 bit_1  : 1;
                uint32 bit_2  : 1;
                uint32 bit_3  : 1;
                uint32 bit_4  : 1;
                uint32 bit_5  : 1;
                uint32 bit_6  : 1;
                uint32 bit_7  : 1;
                uint32 bit_8  : 1;
                uint32 bit_9  : 1;
                uint32 bit_10 : 1;
                uint32 bit_11 : 1;
                uint32 bit_12 : 1;
                uint32 bit_13 : 1;
                uint32 bit_14 : 1;
                uint32 bit_15 : 1;
                uint32 bit_16 : 1;
                uint32 bit_17 : 1;
                uint32 bit_18 : 1;
                uint32 bit_19 : 1;
                uint32 bit_20 : 1;
                uint32 bit_21 : 1;
                uint32 bit_22 : 1;
                uint32 bit_23 : 1;
                uint32 bit_24 : 1;
                uint32 bit_25 : 1;
                uint32 bit_26 : 1;
                uint32 bit_27 : 1;
                uint32 bit_28 : 1;
                uint32 bit_29 : 1;
                uint32 bit_30 : 1;
                uint32 bit_31 : 1;
#           elif WORD_SIZE == 64
                uint64 bit_0  : 1;
                uint64 bit_1  : 1;
                uint64 bit_2  : 1;
                uint64 bit_3  : 1;
                uint64 bit_4  : 1;
                uint64 bit_5  : 1;
                uint64 bit_6  : 1;
                uint64 bit_7  : 1;
                uint64 bit_8  : 1;
                uint64 bit_9  : 1;
                uint64 bit_10 : 1;
                uint64 bit_11 : 1;
                uint64 bit_12 : 1;
                uint64 bit_13 : 1;
                uint64 bit_14 : 1;
                uint64 bit_15 : 1;
                uint64 bit_16 : 1;
                uint64 bit_17 : 1;
                uint64 bit_18 : 1;
                uint64 bit_19 : 1;
                uint64 bit_20 : 1;
                uint64 bit_21 : 1;
                uint64 bit_22 : 1;
                uint64 bit_23 : 1;
                uint64 bit_24 : 1;
                uint64 bit_25 : 1;
                uint64 bit_26 : 1;
                uint64 bit_27 : 1;
                uint64 bit_28 : 1;
                uint64 bit_29 : 1;
                uint64 bit_30 : 1;
                uint64 bit_31 : 1;
                uint64 bit_32 : 1;
                uint64 bit_33 : 1;
                uint64 bit_34 : 1;
                uint64 bit_35 : 1;
                uint64 bit_36 : 1;
                uint64 bit_37 : 1;
                uint64 bit_38 : 1;
                uint64 bit_39 : 1;
                uint64 bit_40 : 1;
                uint64 bit_41 : 1;
                uint64 bit_42 : 1;
                uint64 bit_43 : 1;
                uint64 bit_44 : 1;
                uint64 bit_45 : 1;
                uint64 bit_46 : 1;
                uint64 bit_47 : 1;
                uint64 bit_48 : 1;
                uint64 bit_49 : 1;
                uint64 bit_50 : 1;
                uint64 bit_51 : 1;
                uint64 bit_52 : 1;
                uint64 bit_53 : 1;
                uint64 bit_54 : 1;
                uint64 bit_55 : 1;
                uint64 bit_56 : 1;
                uint64 bit_57 : 1;
                uint64 bit_58 : 1;
                uint64 bit_59 : 1;
                uint64 bit_60 : 1;
                uint64 bit_61 : 1;
                uint64 bit_62 : 1;
                uint64 bit_63 : 1;
#           endif
            } bit_vector; ///< Bit vector.
        } Sim_Variant;

        /**
         * @ingroup typedefs
         * @brief Macro for declaring a null macro.
         */
#       define SIM_NULL_VARIANT ((Sim_Variant){0})

        /**
         * @ingroup typedefs
         * @brief Structure containing two generic pointers.
         * 
         * @var Sim_PointerPair::item1
         *     First pointer.
         * @var Sim_PointerPair::item2
         *     Second pointer.
         */
        typedef struct Sim_PointerPair {
            void *const item1;
            void *const item2;
        } Sim_PointerPair;

        /**
         * @ingroup typedefs
         * @brief Function pointer for a comparison between two values.
         * 
         * @param[in] item1 Pointer to the first item to compare.
         * @param[in] item2 Pointer to the second item to compare.
         * 
         * @returns An int > 0 if item1 > item2; int < 0 if item2 > item1; 0 if item1 == item2.
         */
        typedef int (*Sim_ComparisonProc)(
            const void* const item1,
            const void* const item2
        );

        /**
         * @ingroup typedefs
         * @brief Function pointer to a predicate function that takes two values.
         * 
         * @param[in] item1 Pointer to the first item to compare.
         * @param[in] item2 Pointer to the second item to compare.
         * 
         * @returns @c true or @c false, depending on what the predicate is used for.
         */
        typedef bool (*Sim_PredicateProc)(
            const void* const item1,
            const void* const item2
        );

        /**
         * @ingroup typedefs
         * @brief Function pointer used when iterating over a collection of items.
         * 
         * @param[in,out] item     Pointer to an item in a collection.
         * @param[in]     index    The item's index in the collection it's contained in.
         * @param[in]     userdata User-provided callback data.
         * 
         * @returns @c false to break out of the calling foreach loop;
         *         @c true  to continue iterating.
         */
        typedef bool (*Sim_ForEachProc)(
            void *const item,
            const size_t index,
            Sim_Variant userdata
        );

        /**
         * @ingroup typedefs
         * @brief Function pointer used when iterating over a collection of const items.
         * 
         * @param[in] item     Pointer to an item in a collection.
         * @param[in] index    The item's index in the collection it's contained in.
         * @param[in] userdata User-provided callback data.
         * 
         * @returns @c false to break out of the calling foreach loop;
         *         @c true  to continue iterating.
         */
        typedef bool (*Sim_ConstForEachProc)(
            const void *const item,
            const size_t index,
            Sim_Variant userdata
        );

        /**
         * @ingroup typedefs
         * @brief Integral type representing hash codes.
         */
        typedef uint64 Sim_HashType;

        /**
         * @ingroup typedefs
         * @brief Function pointer that hashes a value.
         * 
         * @param[in] item    Pointer to the item being hashed.
         * 
         * @returns A hash value for the given item.
         */
        typedef Sim_HashType (*Sim_HashProc)(
            const void *const item
        );

        /**
         * @ingroup typedefs
         * @brief Function pointer used when filtering a collection of items.
         * 
         * @param[in] item     Pointer to the item being filtered.
         * @param[in] userdata User-provided callback data.
         *
         * @returns @c true  to keep the item in the given container;
         *         @c false to remove.
         */
        typedef bool (*Sim_FilterProc)(
            const void *const item,
            Sim_Variant userdata
        );
    CPP_NAMESPACE_C_API_END // End C API
#   ifdef __cplusplus       // Start C++ API
        /**
         * @ingroup typedefs
         * @brief Retrieves the return code from SimSoft library functions.
         */
        typedef C_API::Sim_Variant Variant;

        /**
         * @ingroup typedefs
         * @brief Integral type representing hash codes.
         */
        typedef C_API::Sim_HashType HashType;

#   endif                   // End C++ API
CPP_NAMESPACE_END(SimSoft)

#endif // SIMSOFT_TYPEDEFS_H_