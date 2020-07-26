/**
 * @file macro.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header containing convenience macros.
 * @version 0.2
 * @date 2020-07-03
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

/**
 * @defgroup c_macro Common Macros
 * @brief General-purpose C macros
 * @par
 * The SimSoft Library provides a variety of general purpose and common-use C macros to ease
 * cross-compiler software development and/or general convenience. 
 */

#ifndef SIMSOFT_MACRO_H_
#define SIMSOFT_MACRO_H_

#include <stdint.h>

// == CONVENIENCE MACROS ===========================================================================

/**
 * @def UNPACK(...)
 * @ingroup c_macro
 * @brief Convenience macro to unpack variadic arguments and treat as a single argument.
 * 
 * @param ... Arguments to combine.
 */
#define UNPACK(...) __VA_ARGS__

/**
 * @def STRING(...)
 * @ingroup c_macro
 * @brief Convenience macro to convert variadic arguments and turn into a string.
 * 
 * @param ... Arguments to stringify.
 */
#define STRING(...) #__VA_ARGS__

/**
 * @def CONCAT(a, b)
 * @ingroup c_macro
 * @brief Cross-compiler macro that concatenates two tokens together.
 * 
 * @param a Token @e b is concatenated to
 * @param b Token to concatenate to @e a
 */
#define __CONCAT_(a, b) a ## b
#define _CONCAT(a, b)  __CONCAT_(a, b)
#define CONCAT(a, b)   _CONCAT(a, b)

/**
 * @def INOVKE(macro, args)
 * @ingroup c_macro
 * @brief Duct tape header for used internally for macro metaprogramming.
 * 
 * @param macro Macro name to invoke
 * @param args  Arguments to pass into the function macro surrounded by parenthesis.
 */
#define INVOKE(macro, args) macro args

// == MACRO ARGUMENT COUNTING ======================================================================

#define _RSEQ_N()                     64, 63, 62, 61, 60, \
                  59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
                  49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
                  39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
                  29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
                  19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                   9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define __VA_ARGS_COUNT(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, \
                        _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                        _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                        _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
                        _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
                        _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
                        _61, _62, _63, _64,   N, ...)   N

#define _VA_ARGS_COUNT(...) INVOKE(__VA_ARGS_COUNT, (__VA_ARGS__))

/**
 * @def VA_ARGS_COUNT(...)
 * @ingroup c_macro
 * @brief Cross-compiler macro that counts the number of arguments passed into it.
 * 
 * @param ... Argument list to be counted.
 */
#define VA_ARGS_COUNT(...) _VA_ARGS_COUNT(__VA_ARGS__, _RSEQ_N())

/**
 * @def VAR_MACRO(macro_name, ...) 
 * @ingroup c_macro
 * @brief Allows for the definition of specialized varidic macros -- macros that change based on
 *        the number of arguments provided to them.
 * 
 * @param macro_name The name of the variadic macro used as a prefix
 * @param ...        Arguments passed to the variadic macro.
 */
#define VAR_MACRO(macro_name, ...) CONCAT(macro_name, VA_ARGS_COUNT(__VA_ARGS__)) (__VA_ARGS__)

// == FOR-EACH MACRO ===============================================================================

#ifdef _MSC_VER // MSVC = trash compiler with trash preprocessor
#define FOR_EACH2(macro_name, arg)       INVOKE(macro_name, (arg) )
#define FOR_EACH3(macro_name, arg, ...)  INVOKE(macro_name, (arg) ) \
                                             INVOKE(FOR_EACH2,  (macro_name, __VA_ARGS__) )
#define FOR_EACH4(macro_name, arg, ...)  INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH3,  (macro_name, __VA_ARGS__) )
#define FOR_EACH5(macro_name, arg, ...)  INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH4,  (macro_name, __VA_ARGS__) )
#define FOR_EACH6(macro_name, arg, ...)  INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH5,  (macro_name, __VA_ARGS__) )
#define FOR_EACH7(macro_name, arg, ...)  INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH6,  (macro_name, __VA_ARGS__) )
#define FOR_EACH8(macro_name, arg, ...)  INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH7,  (macro_name, __VA_ARGS__) )
#define FOR_EACH9(macro_name, arg, ...)  INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH8,  (macro_name, __VA_ARGS__) )
#define FOR_EACH10(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH9,  (macro_name, __VA_ARGS__) )
#define FOR_EACH11(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH10, (macro_name, __VA_ARGS__) )
#define FOR_EACH12(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH11, (macro_name, __VA_ARGS__) )
#define FOR_EACH13(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH12, (macro_name, __VA_ARGS__) )
#define FOR_EACH14(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH13, (macro_name, __VA_ARGS__) )
#define FOR_EACH15(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH14, (macro_name, __VA_ARGS__) )
#define FOR_EACH16(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH15, (macro_name, __VA_ARGS__) )
#define FOR_EACH17(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH16, (macro_name, __VA_ARGS__) )
#define FOR_EACH18(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH17, (macro_name, __VA_ARGS__) )
#define FOR_EACH19(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH18, (macro_name, __VA_ARGS__) )
#define FOR_EACH20(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH19, (macro_name, __VA_ARGS__) )
#define FOR_EACH21(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH20, (macro_name, __VA_ARGS__) )
#define FOR_EACH22(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH21, (macro_name, __VA_ARGS__) )
#define FOR_EACH23(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH22, (macro_name, __VA_ARGS__) )
#define FOR_EACH24(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH23, (macro_name, __VA_ARGS__) )
#define FOR_EACH25(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH24, (macro_name, __VA_ARGS__) )
#define FOR_EACH26(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH25, (macro_name, __VA_ARGS__) )
#define FOR_EACH27(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH26, (macro_name, __VA_ARGS__) )
#define FOR_EACH28(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH27, (macro_name, __VA_ARGS__) )
#define FOR_EACH29(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH28, (macro_name, __VA_ARGS__) )
#define FOR_EACH30(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH29, (macro_name, __VA_ARGS__) )
#define FOR_EACH31(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH30, (macro_name, __VA_ARGS__) )
#define FOR_EACH32(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH31, (macro_name, __VA_ARGS__) )
#define FOR_EACH33(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH32, (macro_name, __VA_ARGS__) )
#define FOR_EACH34(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH33, (macro_name, __VA_ARGS__) )
#define FOR_EACH35(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH34, (macro_name, __VA_ARGS__) )
#define FOR_EACH36(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH35, (macro_name, __VA_ARGS__) )
#define FOR_EACH37(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH36, (macro_name, __VA_ARGS__) )
#define FOR_EACH38(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH37, (macro_name, __VA_ARGS__) )
#define FOR_EACH39(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH38, (macro_name, __VA_ARGS__) )
#define FOR_EACH40(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH39, (macro_name, __VA_ARGS__) )
#define FOR_EACH41(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH40, (macro_name, __VA_ARGS__) )
#define FOR_EACH42(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH41, (macro_name, __VA_ARGS__) )
#define FOR_EACH43(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH42, (macro_name, __VA_ARGS__) )
#define FOR_EACH44(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH43, (macro_name, __VA_ARGS__) )
#define FOR_EACH45(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH44, (macro_name, __VA_ARGS__) )
#define FOR_EACH46(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH45, (macro_name, __VA_ARGS__) )
#define FOR_EACH47(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH46, (macro_name, __VA_ARGS__) )
#define FOR_EACH48(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH47, (macro_name, __VA_ARGS__) )
#define FOR_EACH49(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH48, (macro_name, __VA_ARGS__) )
#define FOR_EACH50(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH49, (macro_name, __VA_ARGS__) )
#define FOR_EACH51(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH50, (macro_name, __VA_ARGS__) )
#define FOR_EACH52(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH51, (macro_name, __VA_ARGS__) )
#define FOR_EACH53(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH52, (macro_name, __VA_ARGS__) )
#define FOR_EACH54(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH53, (macro_name, __VA_ARGS__) )
#define FOR_EACH55(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH54, (macro_name, __VA_ARGS__) )
#define FOR_EACH56(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH55, (macro_name, __VA_ARGS__) )
#define FOR_EACH57(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH56, (macro_name, __VA_ARGS__) )
#define FOR_EACH58(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH57, (macro_name, __VA_ARGS__) )
#define FOR_EACH59(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH58, (macro_name, __VA_ARGS__) )
#define FOR_EACH60(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH59, (macro_name, __VA_ARGS__) )
#define FOR_EACH61(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH60, (macro_name, __VA_ARGS__) )
#define FOR_EACH62(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH61, (macro_name, __VA_ARGS__) )
#define FOR_EACH63(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH62, (macro_name, __VA_ARGS__) )
#define FOR_EACH64(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH63, (macro_name, __VA_ARGS__) )
#define FOR_EACH65(macro_name, arg, ...) INVOKE(macro_name, (arg)) \
                                             INVOKE(FOR_EACH64, (macro_name, __VA_ARGS__) )

#define FOR_EACH_X3(macro_name, x, arg) INVOKE(macro_name, (x, arg))
#define FOR_EACH_X4(macro_name, x, arg, ...)  INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X3, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X5(macro_name, x, arg, ...)  INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X4, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X6(macro_name, x, arg, ...)  INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X5, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X7(macro_name, x, arg, ...)  INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X6, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X8(macro_name, x, arg, ...)  INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X7, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X9(macro_name, x, arg, ...)  INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X8, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X10(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X9, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X11(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X10, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X12(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X11, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X13(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X12, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X14(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X13, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X15(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X14, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X16(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X15, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X17(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X16, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X18(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X17, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X19(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X18, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X20(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X19, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X21(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X20, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X22(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X21, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X23(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X22, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X24(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X23, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X25(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X24, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X26(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X25, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X27(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X26, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X28(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X27, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X29(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X28, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X30(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X29, (macro_name, x, __VA_ARGS__) )
#define FOR_EACH_X31(macro_name, x, arg, ...) INVOKE(macro_name, (x, arg)) \
                                              INVOKE(FOR_EACH_X30, (macro_name, x, __VA_ARGS__) )

#else // for sane preprocessor implementations
#define FOR_EACH2(macro_name, arg)       macro_name(arg)
#define FOR_EACH3(macro_name, arg, ...)  macro_name(arg) \
                                             FOR_EACH2(macro_name, __VA_ARGS__)
#define FOR_EACH4(macro_name, arg, ...)  macro_name(arg) \
                                             FOR_EACH3(macro_name, __VA_ARGS__)
#define FOR_EACH5(macro_name, arg, ...)  macro_name(arg) \
                                             FOR_EACH4(macro_name, __VA_ARGS__)
#define FOR_EACH6(macro_name, arg, ...)  macro_name(arg) \
                                             FOR_EACH5(macro_name, __VA_ARGS__)
#define FOR_EACH7(macro_name, arg, ...)  macro_name(arg) \
                                             FOR_EACH6(macro_name, __VA_ARGS__)
#define FOR_EACH8(macro_name, arg, ...)  macro_name(arg) \
                                             FOR_EACH7(macro_name, __VA_ARGS__)
#define FOR_EACH9(macro_name, arg, ...)  macro_name(arg) \
                                             FOR_EACH8(macro_name, __VA_ARGS__)
#define FOR_EACH10(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH9(macro_name, __VA_ARGS__)
#define FOR_EACH11(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH10(macro_name, __VA_ARGS__)
#define FOR_EACH12(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH11(macro_name, __VA_ARGS__)
#define FOR_EACH13(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH12(macro_name, __VA_ARGS__)
#define FOR_EACH14(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH13(macro_name, __VA_ARGS__)
#define FOR_EACH15(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH14(macro_name, __VA_ARGS__)
#define FOR_EACH16(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH15(macro_name, __VA_ARGS__)
#define FOR_EACH17(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH16(macro_name, __VA_ARGS__)
#define FOR_EACH18(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH17(macro_name, __VA_ARGS__)
#define FOR_EACH19(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH18(macro_name, __VA_ARGS__)
#define FOR_EACH20(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH19(macro_name, __VA_ARGS__)
#define FOR_EACH21(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH20(macro_name, __VA_ARGS__)
#define FOR_EACH22(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH21(macro_name, __VA_ARGS__)
#define FOR_EACH23(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH22(macro_name, __VA_ARGS__)
#define FOR_EACH24(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH23(macro_name, __VA_ARGS__)
#define FOR_EACH25(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH24(macro_name, __VA_ARGS__)
#define FOR_EACH26(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH25(macro_name, __VA_ARGS__)
#define FOR_EACH27(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH26(macro_name, __VA_ARGS__)
#define FOR_EACH28(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH27(macro_name, __VA_ARGS__)
#define FOR_EACH29(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH28(macro_name, __VA_ARGS__)
#define FOR_EACH30(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH29(macro_name, __VA_ARGS__)
#define FOR_EACH31(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH30(macro_name, __VA_ARGS__)
#define FOR_EACH32(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH31(macro_name, __VA_ARGS__)
#define FOR_EACH33(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH32(macro_name, __VA_ARGS__)
#define FOR_EACH34(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH33(macro_name, __VA_ARGS__)
#define FOR_EACH35(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH34(macro_name, __VA_ARGS__)
#define FOR_EACH36(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH35(macro_name, __VA_ARGS__)
#define FOR_EACH37(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH36(macro_name, __VA_ARGS__)
#define FOR_EACH38(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH37(macro_name, __VA_ARGS__)
#define FOR_EACH39(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH38(macro_name, __VA_ARGS__)
#define FOR_EACH40(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH39(macro_name, __VA_ARGS__)
#define FOR_EACH41(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH40(macro_name, __VA_ARGS__)
#define FOR_EACH42(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH41(macro_name, __VA_ARGS__)
#define FOR_EACH43(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH42(macro_name, __VA_ARGS__)
#define FOR_EACH44(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH43(macro_name, __VA_ARGS__)
#define FOR_EACH45(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH44(macro_name, __VA_ARGS__)
#define FOR_EACH46(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH45(macro_name, __VA_ARGS__)
#define FOR_EACH47(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH46(macro_name, __VA_ARGS__)
#define FOR_EACH48(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH47(macro_name, __VA_ARGS__)
#define FOR_EACH49(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH48(macro_name, __VA_ARGS__)
#define FOR_EACH50(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH49(macro_name, __VA_ARGS__)
#define FOR_EACH51(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH50(macro_name, __VA_ARGS__)
#define FOR_EACH52(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH51(macro_name, __VA_ARGS__)
#define FOR_EACH53(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH52(macro_name, __VA_ARGS__)
#define FOR_EACH54(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH53(macro_name, __VA_ARGS__)
#define FOR_EACH55(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH54(macro_name, __VA_ARGS__)
#define FOR_EACH56(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH55(macro_name, __VA_ARGS__)
#define FOR_EACH57(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH56(macro_name, __VA_ARGS__)
#define FOR_EACH58(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH57(macro_name, __VA_ARGS__)
#define FOR_EACH59(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH58(macro_name, __VA_ARGS__)
#define FOR_EACH60(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH59(macro_name, __VA_ARGS__)
#define FOR_EACH61(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH60(macro_name, __VA_ARGS__)
#define FOR_EACH62(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH61(macro_name, __VA_ARGS__)
#define FOR_EACH63(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH62(macro_name, __VA_ARGS__)
#define FOR_EACH64(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH63(macro_name, __VA_ARGS__)
#define FOR_EACH65(macro_name, arg, ...) macro_name(arg) \
                                             FOR_EACH64(macro_name, __VA_ARGS__)

#define FOR_EACH_X3(macro_name, x, arg) macro_name(x, arg)
#define FOR_EACH_X4(macro_name, x, arg, ...)  macro_name(x, arg) \
                                              FOR_EACH_X3(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X5(macro_name, x, arg, ...)  macro_name(x, arg) \
                                              FOR_EACH_X4(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X6(macro_name, x, arg, ...)  macro_name(x, arg) \
                                              FOR_EACH_X5(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X7(macro_name, x, arg, ...)  macro_name(x, arg) \
                                              FOR_EACH_X6(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X8(macro_name, x, arg, ...)  macro_name(x, arg) \
                                              FOR_EACH_X7(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X9(macro_name, x, arg, ...)  macro_name(x, arg) \
                                              FOR_EACH_X8(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X10(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X9(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X11(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X10(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X12(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X11(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X13(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X12(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X14(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X13(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X15(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X14(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X16(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X15(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X17(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X16(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X18(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X17(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X19(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X18(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X20(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X19(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X21(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X20(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X22(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X21(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X23(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X22(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X24(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X23(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X25(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X24(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X26(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X25(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X27(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X26(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X28(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X27(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X29(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X28(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X30(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X29(macro_name, x, __VA_ARGS__)
#define FOR_EACH_X31(macro_name, x, arg, ...) macro_name(x, arg) \
                                              FOR_EACH_X30(macro_name, x, __VA_ARGS__)
#endif

/**
 * @def FOR_EACH(macro_name, ...)
 * @ingroup c_macro
 * @brief Expands such that each variadic argument is applied to the given macro name.
 * 
 * @param macro_name Name of a single-parameter macro.
 * @param ...        List of single arguments passed to @e macro_name.
 */
#define FOR_EACH(macro_name, ...) VAR_MACRO(FOR_EACH, macro_name, __VA_ARGS__)

/**
 * @def FOR_EACH_X(macro_name, x, ...)
 * @ingroup c_macro
 * @brief Expands such that each variadic argument and an additional argument, x, is applied to the
 *        given macro name.
 * 
 * @param macro_name Name of a two-parameter macro
 * @param x          The first argument passed to @e macro_name.
 * @param ...        List of second arguments passed to @e macro_name.
 */
#define FOR_EACH_X(macro_name, x, ...) VAR_MACRO(FOR_EACH_X, macro_name, x, __VA_ARGS__)

// == GET SPECIFIC VARIADIC ARGUMENT FROM MACRO ====================================================

#define GET_VAR_ARG1(_1, ...) _1
#define GET_VAR_ARG2(_1, _2, ...) _2
#define GET_VAR_ARG3(_1, _2, _3, ...) _3
#define GET_VAR_ARG4(_1, _2, _3, _4, ...) _4
#define GET_VAR_ARG5(_1, _2, _3, _4, _5, ...) _5
#define GET_VAR_ARG6(_1, _2, _3, _4, _5, _6, ...) _6
#define GET_VAR_ARG7(_1, _2, _3, _4, _5, _6, _7, ...) _7
#define GET_VAR_ARG8(_1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define GET_VAR_ARG9(_1, _2, _3, _4, _5, _6, _7, _8, _9, ...) _9
#define GET_VAR_ARG10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) _10
#define GET_VAR_ARG11(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, ...) _11
#define GET_VAR_ARG12(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, ...) _12
#define GET_VAR_ARG13(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, ...) _13
#define GET_VAR_ARG14(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, ...) _14
#define GET_VAR_ARG15(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, ...) _15
#define GET_VAR_ARG16(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, ...) _16
#define GET_VAR_ARG17(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, ...) _17
#define GET_VAR_ARG18(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, ...) _18
#define GET_VAR_ARG19(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, ...) \
                      _19
#define GET_VAR_ARG20(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      ...) _20
#define GET_VAR_ARG21(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, ...) _21
#define GET_VAR_ARG22(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, ...) _22
#define GET_VAR_ARG23(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, ...) _23
#define GET_VAR_ARG24(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, ...) _24
#define GET_VAR_ARG25(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, _25, ...) _25
#define GET_VAR_ARG26(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, _25, _26, ...) _26
#define GET_VAR_ARG27(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, _25, _26, _27, ...) _27
#define GET_VAR_ARG28(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, _25, _26, _27, _28, ...) _28
#define GET_VAR_ARG29(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, _25, _26, _27, _28, _29, ...) \
                      _29
#define GET_VAR_ARG30(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                      ...) _30
#define GET_VAR_ARG31(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                      _31, ...) _31
#define GET_VAR_ARG32(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                      _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                      _31, _32, ...) _32

/**
 * @def GET_VAR_ARG(num, ...)
 * @ingroup c_macro
 * @brief Retrieves a given argument from a list of variadic macro arguments.
 * 
 * @param num Which argument to receive from the list.
 * @param ... List of variadic macro arguments.
 */
#define GET_VAR_ARG(num, ...) CONCAT(GET_VAR_ARG, num)(__VA_ARGS__)

/**
 * @def GET_LAST_ARG(...)
 * @ingroup c_macro
 * @brief Retrieves the last argument from a list of variadic macro arguments.
 * 
 * @param ... List of variadic macro arguments.
 */
#define GET_LAST_ARG(...) VAR_MACRO(GET_VAR_ARG, __VA_ARGS__)

// == REPEAT MACRO X TIMES =========================================================================

#define __REP_(args, irrelevant) GET_VAR_ARG1 args (GET_VAR_ARG2 args)
#define REPEAT(macro_name, x, arg) FOR_EACH_X ## x(__REP_, (macro_name, arg), _RSEQ_N())

// == CALLING CONVENTION MACRO =====================================================================

/**
 * @def C_CALL
 * @ingroup c_macro
 * @brief Cross-compiler and cross-platform macro for consistent calling convention.
 */
#ifndef C_CALL
#   if (defined(_WIN32) || defined(__WINRT__)) && !defined(__GNUC__)
#       define C_CALL __cdecl
#   elif defined(__OS2__) || defined(__EMX__)
#       define C_CALL _System
#       if defined (__GNUC__) && !defined(_System)
#           define _System /* for old EMX/GCC compatability  */
#       endif
#   else
#       define C_CALL
#   endif
#endif

// == DYNAMIC LIBRARY VISIBILITY MACROS ============================================================

/**
 * @def EXPORT
 * @ingroup c_macro
 * @brief Corss-compiler and cross-platform macro for exporting classes & functions to libraries.
 */
/**
 * @def LOCAL
 * @ingroup c_macro
 * @brief Corss-compiler and cross-platform macro for non-exported classes & functions in libraries.
 */
#if defined(_WIN32) || defined(__WINRT__) || defined(__OS2__)
#   ifdef DYNLIB_BUILD
#       define EXPORT __declspec(dllexport)
#       define LOCAL
#   elif defined(DYNLIB_IMPORT)
#       define EXPORT __declspec(dllimport)
#       define LOCAL
#   else
#       define EXPORT
#       define LOCAL 
#   endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define EXPORT __attribute__((visibility("default")))
#   define LOCAL  __attribute__((visibility("hidden")))
#else
#   define EXPORT
#   define LOCAL
#endif
#ifdef __SYMBIAN32__
#   undef  EXPORT
#   undef  LOCAL
#   define EXPORT
#   define LOCAL
#endif

// == NAMESPACE DECLARATIONS & C API HANDLING IN C++ ===============================================

/**
 * @def CPP_NAMESPACE_START(name)
 * @ingroup c_macro
 * @brief Starts a namespace in C++; no-op in C.
 * 
 * @param name The namespace name.
 */
/**
 * @def CPP_NAMESPACE_END(name)
 * @ingroup c_macro
 * @brief Ends a namespace in C++; no-op in C.
 * 
 * @param name The namespace name.
 */
/**
 * @def CPP_NAMESPACE_C_API_START
 * @ingroup c_macro
 * @brief Starts a C++ C_API namespace. Used to contain the C API in C++.
 */
/**
 * @def CPP_NAMESPACE_C_API_END
 * @ingroup c_macro
 * @brief Ends a C++ C_API namespace.
 */
/**
 * @def C_API_PREFIX(namespace, symbol)
 * @ingroup c_macro
 * @brief Prefixes a C symbol with its C_API namespace in C++; no-op in C.
 * 
 * @param namespace C++ namespace where the C API containing @e symbol is located.
 * @param symbol The C symbol located in @e namespace's C API.
 */
#ifdef __cplusplus
#   define CPP_NAMESPACE_START(name) namespace name {
#   define CPP_NAMESPACE_END(name)   }
#   define CPP_NAMESPACE_C_API_START namespace C_API { extern "C" {
#   define CPP_NAMESPACE_C_API_END   } }

#   define C_API_PREFIX(namespace, symbol) namespace::C_API::symbol
#else
#   define CPP_NAMESPACE_START(name)
#   define CPP_NAMESPACE_END(name) 
#   define CPP_NAMESPACE_C_API_START
#   define CPP_NAMESPACE_C_API_END

#   define C_API_PREFIX(namespace, symbol) symbol
#endif

// == STATIC ASSERTIONS IN C/C++ ===================================================================

/**
 * @def STATIC_ASSERT(cond, msg)
 * @ingroup c_macro
 * @brief Cross-compiler and cross-language static assertion.
 * 
 * @param cond Boolean expression for the compiler to check.
 * @param msg  Error message for the compiler to display.
 */
#ifndef STATIC_ASSERT
#   if defined(__cplusplus) && __cplusplus >= 201103L
#       define STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#   elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#       define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#   elif defined(__GNUC__) || defined(__GNUG__)
#       define STATIC_ASSERT(cond, msg) \
            ({                                                                     \
                extern int __attribute__((error("assertion failure: '" msg "'"))) \
                compile_time_check();                                              \
                ((_cond) ? 0 : compile_time_check()), 0;                           \
            })
#   else
#       define STATIC_ASSERT(cond, msg) typedef char static_assertion_##__LINE__[(cond)?1:-1]
#   endif
#endif

// == THREAD-LOCAL STORAGE SPECIFIER ===============================================================

/**
 * @def THREAD_LOCAL
 * @ingroup c_macro
 * @brief Cross-compiler and cross-language thread local storage specifier.
 */
#ifndef THREAD_LOCAL
#   if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#       define THREAD_LOCAL _Thread_local
#   elif defined(__GNUC__) || defined(__GNUG__)
#       define THREAD_LOCAL __thread
#   elif defined(_MSC_VER)
#       define THREAD_LOCAL __declspec(thread)
#   else
#       define THREAD_LOCAL
#   endif
#endif

// -- NO-RETURN SPECIFIER ==========================================================================

/**
 * @def NO_RETURN
 * @ingroup c_macro
 * @brief Cross-compiler and cross-language noreturn function specifier.
 */
#ifndef NO_RETURN
#   if defined(__cplusplus) && __cplusplus >= 201103L
#       define NO_RETURN [[noreturn]]
#   elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#       define NO_RETURN _Noreturn
#   elif defined(__GNUC__) || defined(__GNUG__)
#       define NO_RETURN __attribute__((noreturn))
#   elif defined(_MSC_VER)
#       define NO_RETURN __declspec(noreturn)
#   else
#       define NO_RETURN
#   endif
#endif

// -- DEPRECATED ATTRIBUTE =========================================================================

/**
 * @def DEPRECATED(msg)
 * @ingroup c_macro
 * @brief Cross-compiler and cross-language deprecation attribute.
 * 
 * @param msg Message for the compiler to warn the user.
 */
#ifdef __cplusplus
#   define DEPRECATED(msg) [[deprecated(msg)]]
#elif defined(__GNUC__) || defined(__GNUG__)
#   define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#   define DEPRECATED(msg) __declspec(deprecated(msg))
#else
#   define DEPRECATED(msg)
#endif

// == COMPILER WARNING & ERROR VIA PREPROCESSOR ====================================================

/**
 * @def WARNING(msg)
 * @ingroup c_macro
 * @brief Cross-compiler pragma that issues a compiler warning.
 * 
 * @param msg Message for the compiler to warn the user.
 */
#ifdef _MSC_VER
#   define WARNING(msg) __pragma(warning(msg))
#elif defined(__GNUC__) || defined(__GNUG__)
#   define WARNING(msg) _Pragma("GCC warning \"" msg "\"")
#else
#   define WARNING(msg)
#endif

/**
 * @brief ERROR(msg)
 * @ingroup c_macro
 * @brief Cross-compiler pragma that issues a compiler error. If not possible, issues a compiler
 *        warning if possible.
 * 
 * @param msg Message for the compiler to warn the user.
 */
#if defined(__GNUC__) || defined(__GNUG__)
#   define ERROR(msg) _Pragma("GCC error \"" msg "\"")
#elif defined(WARNING)
#   define ERROR(msg) WARNING(msg)
#else
#   define ERROR(msg)
#endif

// == PRINTF FUNCTION SPECIFIER ====================================================================

/**
 * @def PRINTF_SPEC
 * @ingroup c_macro
 * @brief Cross-compiler function specifier that 
 * 
 * @param format_str_index The argument number where the format string is located.
 * @param variadic_index   The argument number where variadic arguments for the function string are
 *                         located.
 */
#if defined(__GNUC__) || defined(__GNUG__)
#   define PRINTF_SPEC(format_str_index, variadic_index) \
        __attribute__((format(printf, format_str_index, variadic_index)))
#else
#   define PRINTF_SPEC(format_str_index, variadic_index)
#endif

// == FUNCTION NAME IDENTIFIER =====================================================================

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) // C99+ supports __func__
#   define FUNCTION_NAME __func__
#elif ((__GNUC__ >= 2) || defined(_MSC_VER) || defined (__WATCOMC__))
#   define FUNCTION_NAME __FUNCTION__
#else
#   define FUNCTION_NAME "(unknown)"
#endif // FUNCTION_NAME

// == UNUSED PARAMETES =============================================================================

#define UNUSED_PARAM(p) (void)p;

/**
 * @def UNUSED(...)
 * @ingroup c_macro
 * @brief Macro used to declare unused parameters.
 * 
 * @param ... Parameter list.
 */
#define UNUSED(...) FOR_EACH(UNUSED_PARAM, __VA_ARGS__)

// == MIN & MAX ====================================================================================

/**
 * @def MIN(x, y)
 * @ingroup c_macro
 * @brief Retrieves the minimum value of the two numbers.
 * 
 * @param x A number.
 * @param y A number.
 */
/**
 * @def MAX(x, y)
 * @ingroup c_macro
 * @brief Retrieves the maximum value of the two numbers.
 * 
 * @param x A number.
 * @param y A number.
 */
#if defined(__GNUC__) || defined(__GNUG__)
#   define MIN(x, y)               \
        ({ __typeof__(x) _x = (x); \
           __typeof__(y) _y = (y); \
           _x < _y ? _x : _y; })
#   define MAX(x, y)               \
        ({ __typeof__(x) _x = (x); \
           __typeof__(y) _y = (y); \
           _x > _y ? _x : _y; })
#else
#   define MIN(x, y) (((x) < (y) ? (x) : (y)))
#   define MAX(x, y) (((x) > (y) ? (x) : (y)))
#endif

// == MACHINE WORD SIZE CHECK ======================================================================

/**
 * @def WORD_SIZE
 * @ingroup c_macro
 * @brief Macro used to check for the maximum word size of the given environment in bits.
 */
#if defined(__uintptr_t_defined) || defined(_UINTPTR_T_DEFINED) || defined(UINTPTR_MAX)
#   if UINTPTR_MAX == UINT64_MAX
#       define WORD_SIZE 64
#   elif UINTPTR_MAX == UINT32_MAX
#       define WORD_SIZE 32
#   endif
#elif defined(__WORDSIZE)
#   define WORD_SIZE __WORDSIZE
#elif defined(__x86_64__) || defined(_WIN64)
#   define WORD_SIZE 64
#elif defined(__i386__) || defined(_WIN32)
#   define WORD_SIZE 32
#endif

// == DYNAMIC LIBRARY HELPERS ======================================================================

/**
 * @def DYNAPI_PROC(return_type, proc_name, ...)
 * @ingroup c_macro
 * @brief Macro used for ease of defining exported functions in dynamic library APIs.
 * 
 * @param return_type Return type of the function.
 * @param proc_name   Exported function name.
 * @param specifiers  Function specifiers.
 * @param ...         Function parameters.
 */
#ifdef DYNAPI
#   define DYNAPI_PROC(return_type, proc_name, specifiers, ...) \
        typedef return_type (C_CALL * PROC_ ## proc_name)(UNPACK(__VA_ARGS__))
#else
#   define DYNAPI_PROC(return_type, proc_name, specifiers, ...)                     \
        extern EXPORT specifiers return_type C_CALL proc_name(UNPACK(__VA_ARGS__)); \
        typedef return_type (C_CALL * PROC_ ## proc_name)(UNPACK(__VA_ARGS__))
#endif // DYNAPI

// == C VIRTUAL FUNCTION CALL ======================================================================

#define VPROC_CALL2(obj, vfunc)       (((obj)->vfunc) ? (obj)->vfunc(obj) : 0)
#define VPROC_CALL3(obj, vfunc, ...)  (((obj)->vfunc) ? (obj)->vfunc(obj, __VA_ARGS__) : 0)
#define VPROC_CALL4(obj, vfunc, ...)  VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL5(obj, vfunc, ...)  VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL6(obj, vfunc, ...)  VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL7(obj, vfunc, ...)  VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL8(obj, vfunc, ...)  VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL9(obj, vfunc, ...)  VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL10(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL11(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL12(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL13(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL14(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL15(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL16(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL17(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL18(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL19(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))
#define VPROC_CALL20(obj, vfunc, ...) VPROC_CALL3(obj, vfunc, UNPACK(__VA_ARGS__))

/**
 * @brief Calls a virtual function (i.e. function pointer) contained in a C struct.
 * 
 * @param obj C struct containing the virtual function
 * @param ... Virtual function name, followed by arguments
 */
#define VPROC_CALL(obj, ...) VAR_MACRO(VPROC_CALL, obj, __VA_ARGS__)

// == ARRAY SIZE MACRO =============================================================================

/**
 * @brief Retrieves the number of elements in an static array.
 * 
 * @param arr Array to find the size of.
 */
#define ARR_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))

// == CROSS-LANGUAGE RESTRICT KEYWORD ==============================================================

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && !defined(__cplusplus)
#   define RESTRICT restrict
#elif defined(_MSC_VER) || defined(__GNUC__) || defined(__GNUG__)
#   define RESTRICT __restrict
#else
#   define RESTRICT
#endif // RESTRICT

// == CONSISTENT UNIX MACRO CHECK ==================================================================

#if defined(unix) || defined(__unix__) || defined(__unix)
#   ifndef __unix__
#       define __unix__
#   endif
#endif

// =================================================================================================

#endif // SIMSOFT_MACRO_H_