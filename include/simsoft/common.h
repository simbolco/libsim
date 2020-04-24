/**
 * @file common.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Common header used by other header files
 * @version 0.1
 * @date 2019-12-22
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

/**
 * @mainpage
 * 
 * @tableofcontents
 * 
 * @section intro_sec Introduction
 * 
 * The <b>Sim</b>on Bolivar & Company <b>Soft</b>ware <b>Library</b></i> is (intended to be) a
 * cross-platform multilingual library containing various useful features to ease software
 * development.
 * 
 */

/**
 * @defgroup return_code Return Codes
 * @brief Library return codes
 * @par
 * Functions in the SimSoft library use return codes, alongside normal function return values, to
 * communicate their status -- namely if a given operation was successful or not. Return codes can
 * broadly be split into two groups: @b expected return codes -- i.e. return codes that denote
 * expected behavior, namely when a function successfully executes (not necessarily that the
 * operation the function is executing was itself successful) -- and @b exceptional return codes
 * (a.k.a. error codes) -- i.e. return codes that denote exceptional behavior, namely when the
 * function fails to execute its given operation.
 */

/**
 * @defgroup c_exception C Exceptions
 * @brief Exception handling in C
 * @par
 * The SimSoft Library is built to allow for C-style exception handling using @c setjmp and
 * @c longjmp. Library functions, instead of returning error codes when an error occurs, throws
 * error codes as an exception that must be caught using the library's exception handling functions.
 * If an exception is raised without a TRY()-CATCH()- FINALLY() block to handle the raised
 * exception, the program immediately exits.
 * 
 * @par Notes on C++ Interoperability
 * Users of the SimSoft C++ library should be careful not to intermix native C++ exceptions and
 * SimSoft C exceptions. These users should instead use the classes and functionality defined in
 * @link simsoft/exception.hpp @endlink.
 */

/**
 * @defgroup c_macro Common Macros
 * @brief General-purpose C macros
 * @par
 * The SimSoft Library provides a variety of general purpose and common-use C macros to ease
 * cross-compiler software development and/or general convenience. 
 */

/**
 * @defgroup containers Container Types
 * @brief Generalized containers
 * @par
 * The SimSoft Library provides various container types for a variety of needs.
 */

#ifndef SIMSOFT_COMMON_H_
#define SIMSOFT_COMMON_H_

#include <limits.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Include C stdbool when not in C++ mode
#ifndef __cplusplus
#   include <stdbool.h>
#endif

// -- CRAZY C PREPROCESSOR SHIT FOR ALL SORTS OF THINGS --------------------------------------------

// ---- Specialized variadic macros ----------------------------------------------------------------

/**
 * @def VA_ARGS_COUNT(...)
 * @ingroup c_macro
 * @brief Cross-compiler macro that counts up to 32 variadic arguments passed into it.
 * 
 * @param ... Argument list to be counted.
 */

#ifdef _MSC_VER
#   define __EXPAND(x) x
#   define __VA_ARGS_COUNT(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, \
                           _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                           _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                           _31, _32,   N, ...)   N
#   define __VA_ARGS_COUNT_(...)                      \
        __EXPAND(__VA_ARGS_COUNT(__VA_ARGS__, 31, 30, \
              29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
              19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
              9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#   define AUGMENTER(...) unused, __VA_ARGS__
#   define VA_ARGS_COUNT(...) __VA_ARGS_COUNT_(AUGMENTER(__VA_ARGS__))
#else
#   define _VA_ARGS_COUNT(...)                           \
        __VA_ARGS_COUNT_(0, ## __VA_ARGS__, 32, 31, 30, \
                29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
                19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                9,  8,  7,  6,  5,  4,  3,  2,  1,  0)
#   define __VA_ARGS_COUNT_(_00, _01, _02, _03, _04, _05, _06, _07, _08, _09, \
                            _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                            _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
                            _30, _31, _32,   N, ...)   N
#   ifdef __GNUC__
#       define VA_ARGS_COUNT(...) _VA_ARGS_COUNT(...)
#   else
#   endif
#endif

#define _VAR_MACRO_(name, n) name##n
#define _VAR_MACRO(name, n) _VAR_MACRO_(name, n)

/**
 * @def VAR_MACRO(macro_name, ...) 
 * @ingroup c_macro
 * @brief Allows for the definition of specialized varidic macros -- macros that change based on
 *        the number of arguments provided to them.
 * 
 * @param macro_name The name of the variadic macro used as a prefix
 * @param ...        Arguments passed to the variadic macro.
 */
#define VAR_MACRO(macro_name, ...) _VAR_MACRO(macro_name, VA_ARGS_COUNT(__VA_ARGS__)) (__VA_ARGS__)

// ---- Foreach macro ------------------------------------------------------------------------------

#define FOR_EACH1(macro_name) macro_name
#define FOR_EACH2(macro_name, arg)       macro_name(arg)
#define FOR_EACH3(macro_name, arg, ...)  macro_name(arg)  FOR_EACH2(macroname, __VA_ARGS__)
#define FOR_EACH4(macro_name, arg, ...)  macro_name(arg)  FOR_EACH3(macroname, __VA_ARGS__)
#define FOR_EACH5(macro_name, arg, ...)  macro_name(arg)  FOR_EACH4(macroname, __VA_ARGS__)
#define FOR_EACH6(macro_name, arg, ...)  macro_name(arg)  FOR_EACH5(macroname, __VA_ARGS__)
#define FOR_EACH7(macro_name, arg, ...)  macro_name(arg)  FOR_EACH6(macroname, __VA_ARGS__)
#define FOR_EACH8(macro_name, arg, ...)  macro_name(arg)  FOR_EACH7(macroname, __VA_ARGS__)
#define FOR_EACH9(macro_name, arg, ...)  macro_name(arg)  FOR_EACH8(macroname, __VA_ARGS__)
#define FOR_EACH10(macro_name, arg, ...) macro_name(arg)  FOR_EACH9(macroname, __VA_ARGS__)
#define FOR_EACH11(macro_name, arg, ...) macro_name(arg) FOR_EACH10(macroname, __VA_ARGS__)
#define FOR_EACH12(macro_name, arg, ...) macro_name(arg) FOR_EACH11(macroname, __VA_ARGS__)
#define FOR_EACH13(macro_name, arg, ...) macro_name(arg) FOR_EACH12(macroname, __VA_ARGS__)
#define FOR_EACH14(macro_name, arg, ...) macro_name(arg) FOR_EACH13(macroname, __VA_ARGS__)
#define FOR_EACH15(macro_name, arg, ...) macro_name(arg) FOR_EACH14(macroname, __VA_ARGS__)
#define FOR_EACH16(macro_name, arg, ...) macro_name(arg) FOR_EACH15(macroname, __VA_ARGS__)
#define FOR_EACH17(macro_name, arg, ...) macro_name(arg) FOR_EACH16(macroname, __VA_ARGS__)
#define FOR_EACH18(macro_name, arg, ...) macro_name(arg) FOR_EACH17(macroname, __VA_ARGS__)
#define FOR_EACH19(macro_name, arg, ...) macro_name(arg) FOR_EACH18(macroname, __VA_ARGS__)
#define FOR_EACH20(macro_name, arg, ...) macro_name(arg) FOR_EACH19(macroname, __VA_ARGS__)
#define FOR_EACH21(macro_name, arg, ...) macro_name(arg) FOR_EACH20(macroname, __VA_ARGS__)
#define FOR_EACH22(macro_name, arg, ...) macro_name(arg) FOR_EACH21(macroname, __VA_ARGS__)
#define FOR_EACH23(macro_name, arg, ...) macro_name(arg) FOR_EACH22(macroname, __VA_ARGS__)
#define FOR_EACH24(macro_name, arg, ...) macro_name(arg) FOR_EACH23(macroname, __VA_ARGS__)
#define FOR_EACH25(macro_name, arg, ...) macro_name(arg) FOR_EACH24(macroname, __VA_ARGS__)
#define FOR_EACH26(macro_name, arg, ...) macro_name(arg) FOR_EACH25(macroname, __VA_ARGS__)
#define FOR_EACH27(macro_name, arg, ...) macro_name(arg) FOR_EACH26(macroname, __VA_ARGS__)
#define FOR_EACH28(macro_name, arg, ...) macro_name(arg) FOR_EACH27(macroname, __VA_ARGS__)
#define FOR_EACH29(macro_name, arg, ...) macro_name(arg) FOR_EACH28(macroname, __VA_ARGS__)
#define FOR_EACH30(macro_name, arg, ...) macro_name(arg) FOR_EACH29(macroname, __VA_ARGS__)
#define FOR_EACH31(macro_name, arg, ...) macro_name(arg) FOR_EACH30(macroname, __VA_ARGS__)
#define FOR_EACH32(macro_name, arg, ...) macro_name(arg) FOR_EACH31(macroname, __VA_ARGS__)

#define FOR_EACH(macro_name, ...) VAR_MACRO(FOR_EACH, macro_name, __VA_ARGS__)

// ---- Macro convenience macros lol ---------------------------------------------------------------

/**
 * @def UNPACK(...)
 * @ingroup c_macro
 * @brief Convenience macro to unpack variadic arguments and treat as a single argument.
 * 
 * @param ... Arguments to combine.
 */
#ifndef UNPACK
#   define UNPACK(...) __VA_ARGS__
#endif

/**
 * @def STRING(...)
 * @ingroup c_macro
 * @brief Convenience macro to convert variadic arguments and turn into a string.
 * 
 * @param ... Arguments to stringify.
 */
#ifndef STRING
#   define STRING(...) #__VA_ARGS__
#endif

// ---- C calling convention (borrowed from SDL source code) ---------------------------------------

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

// ---- Dynamic library / Shared object export macros (borrowed from SDL source code) --------------

/**
 * @def EXPORT
 * @ingroup c_macro
 * @brief Corss-compiler and cross-platform macro for exporting classes & functions to libraries.
 */
#ifndef EXPORT
#   if defined(_WIN32) || defined(__WINRT__)
#       ifdef __BORLANDC__
#           ifdef SIM_BUILD
#               define EXPORT
#           else
#               define EXPORT __declspec(dllimport)
#           endif
#       elif defined(SIM_BUILD)
#           define EXPORT __declspec(dllexport)
#       else
#           define EXPORT
#       endif
#   elif defined(__OS2__)
#       ifdef SIM_BUILD
#           define EXPORT __declspec(dllexport)
#       else
#           define EXPORT
#       endif
#   else
#       if defined(__GNUC__) && __GNUC__ >= 4
#           define EXPORT __attribute__ ((visibility("default")))
#      else
#           define EXPORT
#       endif
#   endif
#   ifdef __SYMBIAN32__
#       undef  EXPORT
#       define EXPORT
#   endif
#endif

// ---- Namespace declaration & C API handling in C++ ----------------------------------------------

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
#ifdef __cplusplus
#   define CPP_NAMESPACE_START(name) namespace name {
#   define CPP_NAMESPACE_END(name)   }
#   define CPP_NAMESPACE_C_API_START namespace C_API { extern "C" {
#   define CPP_NAMESPACE_C_API_END   } }
#else
#   define CPP_NAMESPACE_START(name)
#   define CPP_NAMESPACE_END(name) 
#   define CPP_NAMESPACE_C_API_START
#   define CPP_NAMESPACE_C_API_END
#endif

// ---- Thread-local storage -----------------------------------------------------------------------

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

// -- C Static assertions --------------------------------------------------------------------------

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
#       define STATIC_ASSERT(cond, msg)
#   endif
#endif

// -- C No return specifier ------------------------------------------------------------------------

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

// -- C Deprecation statement ----------------------------------------------------------------------

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

// -- Machine word size check ----------------------------------------------------------------------

#if defined(__uintptr_t_defined) || defined(_UINTPTR_T_DEFINED) || defined(UINTPTR_MAX)
#   if UINTPTR_MAX == UINT64_MAX
#       define WORD_SIZE_64
#   elif UINTPTR_MAX == UINT32_MAX
#       define WORD_SIZE_32
#   endif
#elif defined(__WORDSIZE)
#   if __WORDSIZE == 64
#       define WORD_SIZE_64
#   elif __WORDSIZE == 32
#       define WORD_SIZE_32
#   endif
#elif defined(__x86_64__) || defined(_WIN64)
#   define WORD_SIZE_64
#elif defined(__i386__) || defined(_WIN32)
#   define WORD_SIZE_32
#endif

// -- Error and return codes -----------------------------------------------------------------------

/**
 * @def ENUMERATE_ERROR_CODES
 * @brief A user-expandable macro for the various exceptional library return codes.
 * @details Define __ENUMERATE_ERROR_CODE(x) to use this macro.
 */
#define ENUMERATE_ERROR_CODES        \
    __ENUMERATE_ERROR_CODE(NULLPTR)  \
    __ENUMERATE_ERROR_CODE(OUTOFMEM) \
    __ENUMERATE_ERROR_CODE(ZERODIV)  \
    __ENUMERATE_ERROR_CODE(INVALARG) \
    __ENUMERATE_ERROR_CODE(OUTOFBND) \
    __ENUMERATE_ERROR_CODE(UNSUPRTD) \
    __ENUMERATE_ERROR_CODE(UNIMPLTD) \
    __ENUMERATE_ERROR_CODE(BADFILE)  \
    __ENUMERATE_ERROR_CODE(__LAST)   \

/**
 * @def ENUMERATE_EXPECTED_CODES
 * @brief A user-expandable macro for the various expected library return codes.
 * @details Define __ENUMERATE_EXPECTED_CODE(x) to use this macro.
 */
#define ENUMERATE_EXPECTED_CODES \
    __ENUMERATE_EXPECTED_CODE(SUCCESS)   \
    __ENUMERATE_EXPECTED_CODE(FAILURE)   \
    __ENUMERATE_EXPECTED_CODE(NOT_FOUND) \
    __ENUMERATE_EXPECTED_CODE(NO_JMPBUF) \
    __ENUMERATE_EXPECTED_CODE(__COUNT)

// -- C exception handling -------------------------------------------------------------------------

/**
 * @def THROW(...)
 * @ingroup c_exception
 * @brief Throws an exception.
 * 
 * @details There are two overloads of this macro: one with zero parameters & one with a single
 *          parameter.
 * 
 * @remarks @b THROW(0) - Rethrows the current exception in a TRY()-CATCH()-FINALLY() block.
 * @remarks @b THROW(1) - Throws a given error code (first argument) as an exception.
 * @remarks @b THORW(2) - Throws a given error code (first argument) as an exception alongside a
 *                        given message (second argument).
 * 
 * @remarks All of the above overloads are no-ops if the return code used for them is an expected
 *          return code.
 * 
 * @sa sim_cexcept_throw()
 */
// if 
#define THROW(...)      do { VAR_MACRO(THROW, __VA_ARGS__) } while (0)
#define THROW0()        if (sim_get_return_code() < 0) sim_cexcept_throw(sim_cexcept_pop(), NULL);
#define THROW1(rc)      if ((rc) < 0) sim_cexcept_throw((rc), NULL);
#define THROW2(rc, str) if ((rc) < 0) sim_cexcept_throw((rc), (str));

/**
 * @def TRY(...)
 * @ingroup c_exception
 * @brief Starts a TRY()-CATCH() block.
 * @details Tries to execute code, catching any exceptions via CATCH() if any are raised.
 * 
 * @param ... Code block to execute in the TRY statement.
 */
/**
 * @def CATCH(rc, ...)
 * @ingroup c_exception
 * @brief Catches a specific exception in a TRY()-CATCH() block.
 * 
 * @param rc  Return error code to check for.
 * @param ... Code block to execute in the EXCEPT statement.
 */
/**
 * @def CATCH_ANY(...)
 * @ingroup c_exception
 * @brief Catches all exceptions not caught by any previous CATCH() statements.
 * 
 * @param ... Code block to execute in the EXCEPT statement.
 */
/**
 * @def FINALLY(...)
 * @ingroup c_exception
 * @brief Ends a TRY()-CATCH() block.
 * 
 * @param ... Code block to execute regardless of success or not. Can be empty.
 */
#define TRY(...) {                         \
    switch (setjmp(*sim_cexcept_push())) { \
    case 0:                                \
        UNPACK(__VA_ARGS__)                \
        break;
#define CATCH(rc, ...) case (rc): { UNPACK(__VA_ARGS__) } sim_cexcept_reset(); break;
#define CATCH_ANY(...) default: { UNPACK(__VA_ARGS__) } sim_cexcept_reset(); break;
#define FINALLY(...) } { UNPACK(__VA_ARGS__) } THROW0(); }

// -- Typedefs, function declarations, & struct declarations, oh my! -------------------------------

/**
 * @if CPLUSPLUS 
 *     @namespace SimSoft
 *     @brief The namespace where all SimSoft library functions are contained when using C++.
 * @endif
 */
CPP_NAMESPACE_START(SimSoft)
    /**
     * @typedef sint8
     * @brief Signed 8-bit integer.
     */
    typedef int8_t   sint8;
    /**
     * @typedef uint8
     * @brief Unsigned 8-bit integer.
     */
    typedef uint8_t  uint8;
    /**
     * @typedef sint16
     * @brief Signed 16-bit integer.
     */
    typedef int16_t  sint16;
    /**
     * @typedef uint16
     * @brief Unsigned 16-bit integer.
     */
    typedef uint16_t uint16;
    /**
     * @typedef sint32
     * @brief Signed 32-bit integer.
     */
    typedef int32_t  sint32;
    /**
     * @typedef uint32
     * @brief Unsigned 32-bit integer.
     */
    typedef uint32_t uint32;
    /**
     * @typedef sint64
     * @brief Signed 64-bit integer.
     */
    typedef int64_t  sint64;
    /**
     * @typedef uint64
     * @brief Unsigned 64-bit integer.
     */
    typedef uint64_t uint64;

    /**
     * @if CPLUSPLUS
     *     @namespace SimSoft::C_API
     *     @brief The namespace where the SimSoft library's C API is contained when using C++.
     * @endif
     */
    CPP_NAMESPACE_C_API_START /* C API */

        /**
         * @union Sim_Variant
         * @headerfile common.h "simsoft/common.h"
         * @brief Union of different C data types.
         * 
         * @var Sim_Variant::signed8
         *     8-bit signed integral.
         * @var Sim_Variant::signed16
         *     16-bit signed integral.
         * @var Sim_Variant::signed32
         *     32-bit signed integral.
         * @var Sim_Variant::signed64
         *     64-bit signed integral (excluded from 32-bit builds).
         * 
         * @var Sim_Variant::unsigned8
         *     8-bit unsigned integral.
         * @var Sim_Variant::unsigned16
         *     16-bit unsigned integral.
         * @var Sim_Variant::unsigned32
         *     32-bit unsigned integral.
         * @var Sim_Variant::unsigned64
         *     64-bit unsigned integral (excluded from 32-bit builds).
         * 
         * @var Sim_Variant::float32
         *     Single-precision floating-point value.
         * @var Sim_Variant::float64
         *     Double-precision floating-point value (excluded from 32-bit builds).
         * 
         * @var Sim_Variant::signed_char
         *     Signed char.
         * @var Sim_Variant::signed_short
         *     Signed short.
         * @var Sim_Variant::signed_int
         *     Signed int.
         * @var Sim_Variant::signed_long
         *     Signed long.
         * @var Sim_Variant::signed_long_long
         *     Signed long long (excluded if larger than word size).
         * 
         * @var Sim_Variant::unsigned_char
         *     Unsigned char.
         * @var Sim_Variant::unsigned_short
         *     Unsigned short.
         * @var Sim_Variant::unsigned_int
         *     Unsigned int.
         * @var Sim_Variant::unsigned_long
         *     Unsigned long.
         * @var Sim_Variant::unsigned_long_long
         *     Unsigned long long (excluded if larger than word size).
         * 
         * @var Sim_Variant::signed_size
         *     Signed size type.
         * @var Sim_Variant::unsigned_size
         *     Unsigned size type.
         * 
         * @var Sim_Variant::string
         *     C string.
         * @var Sim_Variant::pointer
         *     Void pointer.
         * 
         * @var Sim_Variant::upointer
         *     Unsigned integer pointer type (if uintpr_t is defined).
         * @var Sim_Variant::spointer
         *     Signed integer pointer type (if intptr_t is defined).
         * @var Sim_Variant::ptr_diff
         *     Pointer difference type (if ptrdiff_t is defined).
         */
        typedef union Sim_Variant {
            // Sized signed integer types
            sint8  signed8;
            sint16 signed16;
            sint32 signed32;
#           ifdef WORD_SIZE_64
                sint64 signed64;
#           endif /* WORD_SIZE_64 */
            
            // Sized unsigned integer types
            uint8  unsigned8;
            uint16 unsigned16;
            uint32 unsigned32;
#           ifdef WORD_SIZE_64
                uint64 unsigned64;
#           endif /* WORD_SIZE_64 */

            // Floating point types
            float  float32;
#           ifdef WORD_SIZE_64
                double float64;
#           endif /* WORD_SIZE_64 */

            // C integral types
            char      signed_char;
            short     signed_short;
            int       signed_int;
            long      signed_long;
#           if (LLONG_MAX > 0x7FFFFFFFl && defined(WORD_SIZE_64)) || \
               (LLONG_MAX <= 0x7FFFFFFFl && defined(WORD_SIZE_32))
                long long signed_long_long;
#           endif /* LLONG_MAX */

            // Unsigned C integral types
            unsigned char      unsigned_char;
            unsigned short     unsigned_short;
            unsigned int       unsigned_int;
            unsigned long      unsigned_long;
#           if (ULLONG_MAX > 0xFFFFFFFFul && defined(WORD_SIZE_64)) || \
               (ULLONG_MAX <= 0xFFFFFFFFul && defined(WORD_SIZE_32))
                unsigned long long unsigned_long_long;
#           endif /* ULLONG_MAX */

            // Size types
#           ifdef SSIZE_MAX
                ssize_t signed_size;
#           endif
#           ifdef SIZE_MAX
                size_t  unsigned_size;
#           endif

            // Pointer types
            char* string;
            void* pointer;
            
            // Pointer math types
#           ifdef UINTPTR_MAX
                uintptr_t upointer;
#           endif
#           ifdef INTPTR_MAX
                intptr_t  spointer;
#           endif
#           ifdef PTRDIFF_MAX
                ptrdiff_t ptr_diff;
#           endif
        } Sim_Variant;

#       define __ENUMERATE_ERROR_CODE(id)    SIM_RC_ERR_ ##id,
#       define __ENUMERATE_EXPECTED_CODE(id) SIM_RC_ ##id,
            /**
             * @enum Sim_ReturnCode
             * @headerfile common.h "simsoft/common.h"
             * @ingroup return_code
             * @brief C enumeration of return codes of library functions.
             * 
             * @remarks Return codes >= 0 are expected (i.e. unexceptional) return codes.
             * @remarks Return codes < 0 are exceptional return codes.
             * 
             * @var Sim_ReturnCode::SIM_RC_SUCCESS
             *     The function returned successfully.
             * @var Sim_ReturnCode::SIM_RC_FAILURE
             *     General unexpectional failure.
             * @var Sim_ReturnCode::SIM_RC_NOT_FOUND
             *     Item not found in container.
             * @var Sim_ReturnCode::SIM_RC_NO_JMPBUF
             * @ingroup c_exceptions
             *     Jump buffer stack is empty.
             * @var Sim_ReturnCode::SIM_RC___COUNT
             *     Number of expected return codes.
             * 
             * @var Sim_ReturnCode::SIM_RC_ERR_NULLPTR
             *     A nullptr was passed into the function.
             * @var Sim_ReturnCode::SIM_RC_ERR_OUTOFMEM
             *     Ran out of memory when attempting to allocate something.
             * @var Sim_ReturnCode::SIM_RC_ERR_ZERODIV
             *     Arithmetic zero-division error.
             * @var Sim_ReturnCode::SIM_RC_ERR_INVALARG
             *     Invalid argument passed into function.
             * @var Sim_ReturnCode::SIM_RC_ERR_OUTOFBND
             *     Index argument out of bounds of container.
             * @var Sim_ReturnCode::SIM_RC_ERR_UNSUPRTD
             *     Unsupported operation on given platform.
             * @var Sim_ReturnCode::SIM_RC_ERR_UNIMPLTD
             *     Unimplemented/unfinished operation.
             * @var Sim_ReturnCode::SIM_RC_ERR_BADFILE
             *     Bad or invalid file.
             * @var Sim_ReturnCode::SIM_RC_ERR___COUNT
             *     Number of exceptional return codes.
             */
            typedef enum Sim_ReturnCode {
                ENUMERATE_EXPECTED_CODES
                
                SIM_RC_ERR___FIRST = INT_MIN,
                ENUMERATE_ERROR_CODES
                SIM_RC_ERR___COUNT = SIM_RC_ERR___LAST - SIM_RC_ERR___FIRST - 1
            } Sim_ReturnCode;
#       undef __ENUMERATE_EXPECTED_CODE
#       undef __ENUMERATE_ERROR_CODE

        /**
         * @struct Sim_PointerPair
         * @headerfile common.h "simsoft/common.h"
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
         * @typedef Sim_ComparisonProc
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer for a comparison between two values.
         * 
         * @param[in] item1 Pointer to the first item to compare.
         * @param[in] item2 Pointer to the second item to compare.
         * 
         * @return An int > 0 if item1 > item2; int < 0 if item2 > item1; 0 if item1 == item2.
         */
        typedef int (*Sim_ComparisonProc)(
            const void* const item1,
            const void* const item2
        );

        /**
         * @typedef Sim_PredicateProc
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer to a predicate function that takes two values.
         * 
         * @param[in] item1 Pointer to the first item to compare.
         * @param[in] item2 Pointer to the second item to compare.
         * 
         * @return @c true or @c false, depending on what the predicate is used for.
         */
        typedef bool (*Sim_PredicateProc)(
            const void* const item1,
            const void* const item2
        );

        /**
         * @typedef Sim_ForEachProc
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer used when iterating over a collection of items.
         * 
         * @param[in,out] item     Pointer to an item in a collection.
         * @param[in]     index    The item's index in the collection it's contained in.
         * @param[in]     userdata User-provided callback data.
         * 
         * @return @c false to break out of the calling foreach loop;
         *         @c true  to continue iterating.
         */
        typedef bool (*Sim_ForEachProc)(
            void *const item,
            const size_t index,
            Sim_Variant userdata
        );

        /**
         * @typedef Sim_ConstForEachProc
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer used when iterating over a collection of const items.
         * 
         * @param[in] item     Pointer to an item in a collection.
         * @param[in] index    The item's index in the collection it's contained in.
         * @param[in] userdata User-provided callback data.
         * 
         * @return @c false to break out of the calling foreach loop;
         *         @c true  to continue iterating.
         */
        typedef bool (*Sim_ConstForEachProc)(
            const void *const item,
            const size_t index,
            Sim_Variant userdata
        );

        /**
         * @typedef Sim_HashType
         * @brief Integral type representing hashes.
         */
        typedef uint64 Sim_HashType;

        /**
         * @typedef Sim_HashProc
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer that hashes a value.
         * 
         * @param[in] item    Pointer to the item being hashed.
         * @param[in] attempt The hashing attempt; used for double-hashing of values in hash tables.
         * 
         * @return A hash value for the given item.
         */
        typedef Sim_HashType (*Sim_HashProc)(
            const void *const item,
            const size_t attempt
        );

        /**
         * @typedef Sim_FilterProc
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer used when filtering a collection of items.
         * 
         * @param[in] item     Pointer to the item being filtered.
         * @param[in] userdata User-provided callback data.
         *
         * @return @c true  to keep the item in the given container;
         *         @c false to remove.
         */
        typedef bool (*Sim_FilterProc)(
            const void *const item,
            Sim_Variant userdata
        );

        /**
         * @typedef Sim_CExceptOnResetProc
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Function pointer used when @e sim_cexcept_reset is called.
         */
        typedef void (*Sim_CExceptOnResetProc)(void);

        /**
         * @typedef Sim_CExceptOnThrowProc
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Function pointer used for when an error code is thrown.
         * 
         * @param[in] error_code The error code thrown.
         */
        typedef void (*Sim_CExceptOnThrowProc)(
            Sim_ReturnCode error_code
        );

        /**
         * @fn Sim_ReturnCode sim_get_return_code(void)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup return_code
         * @brief Retrieves the return code from SimSoft library functions.
         * 
         * @return A thread-local internal return code from the previous library function call.
         * 
         * @sa sim_set_return_code()
         * @sa sim_get_return_code_string()
         */
        extern EXPORT Sim_ReturnCode C_CALL sim_get_return_code(void);

        /**
         * @fn void sim_set_return_code(Sim_ReturnCode)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup return_code
         * @brief Sets the return code to a given value.
         * 
         * @param[in] return_code The return code to set.
         * 
         * @sa sim_get_return_code()
         */
        extern EXPORT void C_CALL sim_set_return_code(Sim_ReturnCode return_code);

        /**
         * @fn const char* sim_get_return_code_string(Sim_ReturnCode)
         * @headerfile debug.h "simsoft/debug.h"
         * @ingroup return_code
         * @brief Converts a return code to a human-readable string.
         * 
         * @param[in] return_code Return code to convert to a string.
         * 
         * @return A string corresponding to the given return code; @c NULL if @e return_code is
         *         invalid.
         * 
         * @sa sim_get_return_code()
         */
        extern EXPORT const char* C_CALL sim_get_return_code_string(
            Sim_ReturnCode return_code
        );

        /**
         * @fn jmp_buf *const sim_cexcept_push(void)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Retrieves a new jump buffer for exception handling.
         * 
         * @returns Pointer to a jmp_buf struct to use in exception handling.
         */
        extern EXPORT jmp_buf *const C_CALL sim_cexcept_push(void);

        /**
         * @fn Sim_ReturnCode sim_cexcept_pop(void) 
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Deletes the current jump buffer.
         * @details Deletes the jump buffer from the previous call to sim_push_jump_buffer().
         * 
         * @returns The return code of the current jump buffer. @b SIM_RC_NO_JMPBUF if there
         *          are no jump buffers remaining.
         */
        extern EXPORT Sim_ReturnCode C_CALL sim_cexcept_pop(void);

        /**
         * @fn void sim_cexcept_reset(void)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Resets the error status of the current jump buffer.
         * 
         * @remarks This function is a noop if there no current jump buffer is set.
         */
        extern EXPORT void C_CALL sim_cexcept_reset(void);

        /**
         * @fn void sim_cexcept_throw(Sim_ReturnCode, const char*)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Throws an error code as an exception.
         * @details Synchronous signal that exceptional behavior has taken place. Execution
         *          jumps to the last jump buffer created by sim_cexcept_push().
         * 
         * @param[in] error_code    Error code to throw as an exception.
         * @param[in] error_message Error message clarifying or explaining the exception.
         * 
         * @remarks If there are no remaining jump buffers, the program exits.
         * @remarks If @e error_message is @c NULL, the error message is unchanged.
         */
        NO_RETURN extern EXPORT void C_CALL sim_cexcept_throw(
            Sim_ReturnCode error_code,
            const char*    error_message
        );

        /**
         * @fn const char* sim_cexcept_get_error_message(void)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Returns the error message set by the latest exception throw.
         * 
         * @returns The last set error string.
         */
        extern EXPORT const char* C_CALL sim_cexcept_get_error_message(void);

        /**
         * @fn void sim_cexcept_on_reset(Sim_CExceptOnResetProc)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Registers a function to be called when @e sim_cexcept_reset() is called.
         * 
         * @param[in] on_reset_proc Function pointer to be called.
         */
        extern EXPORT void C_CALL sim_cexcept_on_reset(
            Sim_CExceptOnResetProc on_reset_proc
        );

        /**
         * @fn void sim_cexcept_on_throw(Sim_CExceptOnThrowProc)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup c_exception
         * @brief Registers a function to be called when @e sim_cexcept_throw() is called.
         * 
         * @param[in] on_reset_proc Function pointer to be called.
         */
        extern EXPORT void C_CALL sim_cexcept_on_throw(
            Sim_CExceptOnThrowProc on_throw_proc
        );

    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        /**
         * @typedef CVariant
         * @headerfile common.h "simsoft/common.h"
         * @brief Retrieves the return code from SimSoft library functions.
         */
        typedef C_API::Sim_Variant CVariant;

#       define __ENUMERATE_ERROR_CODE(id)    ERR_ ##id = C_API::Sim_ReturnCode::SIM_RC_ERR_ ##id,
#       define __ENUMERATE_EXPECTED_CODE(id) id = C_API::Sim_ReturnCode::SIM_RC_ ##id,
            /**
             * @enum ReturnCode
             * @headerfile common.h "simsoft/common.h"
             * @ingroup return_code
             * @brief Enumeration of return codes of library functions.
             * 
             * @sa C_API::Sim_ReturnCode
             * 
             * @remarks Return codes >= 0 are expected (i.e. unexceptional) return codes.
             * @remarks Return codes < 0 are exceptional return codes.
             * 
             * @var ReturnCode::SUCCESS
             *     The function returned successfully.
             * @var ReturnCode::FAILURE
             *     General unexceptional failure.
             * @var ReturnCode::NOT_FOUND
             *     Item not found in container.
             * @var ReturnCode::NO_JMPBUF
             * @ingroup c_exceptions
             *     Jump buffer stack empty.
             * @var ReturnCode::__COUNT
             *     Number of expected return codes.
             * 
             * @var ReturnCode::ERR_NULLPTR
             *     A nullptr was passed into the function.
             * @var ReturnCode::ERR_OUTOFMEM
             *     Ran out of memory when attempting to allocate something.
             * @var ReturnCode::ERR_ZERODIV
             *     Arithmetic zero-division error.
             * @var ReturnCode::ERR_INVALARG
             *     Invalid argument passed into function.
             * @var ReturnCode::ERR_OUTOFBND
             *     Index argument out of bounds of container.
             * @var ReturnCode::ERR_UNSUPRTD
             *     Unsupported operation on given platform.
             * @var ReturnCode::ERR_UNIMPLTD
             *     Unimplemented/unfinished operation.
             * @var ReturnCode::ERR_BADFILE
             *     Bad or invalid file.
             * @var ReturnCode::ERR___COUNT
             *     Number of exceptional error codes.
             */
            enum ReturnCode {
                ENUMERATE_EXPECTED_CODES
                ENUMERATE_ERROR_CODES
                ERR___COUNT = C_API::Sim_ReturnCode::SIM_RC_ERR___COUNT
            };
#       undef __ENUMERATE_EXPECTED_CODE
#       undef __ENUMERATE_ERROR_CODE

        /**
         * @fn ReturnCode get_return_code()
         * @headerfile common.h "simsoft/common.h"
         * @ingroup return_code
         * @brief Retrieves the return code from SimSoft library functions.
         * 
         * @return A thread-local internal return code from the previous library function call.
         * 
         * @sa get_return_code_string
         */
        extern EXPORT ReturnCode get_return_code();

        /**
         * @fn const char* get_return_code_string(ReturnCode)
         * @headerfile common.h "simsoft/common.h"
         * @ingroup return_code
         * @brief Converts a return code to a human-readable string.
         * 
         * @param[in] return_code Return code to convert to a string.
         * 
         * @return A string corresponding to the given return code; @c NULL if @e return_code is
         *         invalid.
         * 
         * @sa get_return_code
         */
        extern EXPORT const char* C_CALL get_return_code_string(ReturnCode return_code);

        template <class RT, class FUNCTOR, class...ARGS>
        RT c_back_porch_functor_wrapper(ARGS... args, FUNCTOR* functor) {
            return (*functor)(args...);
        }

        template <class RT, class FUNCTOR, class...ARGS>
        RT c_front_porch_functor_wrapper(FUNCTOR* functor, ARGS... args) {
            return (*functor)(args...);
        }

        template <class T>
        struct Predicate_Equal {
            bool operator()(const T& item1, const T& item2) const {
                return item1 == item2;
            }
        
        public:
            typedef T argument_1_type, argument_2_type;
            typedef bool return_type;
        };

        template <class T>
        struct Predicate_NotEqual {
            bool operator()(const T& item1, const T& item2) const {
                return item1 != item2;
            }
        
        public:
            typedef T argument_1_type, argument_2_type;
            typedef bool return_type;
        };

        template <class T>
        struct Predicate_Less {
            bool operator()(const T& item1, const T& item2) const {
                return item1 < item2;
            }
        
        public:
            typedef T argument_1_type, argument_2_type;
            typedef bool return_type;
        };

        template <class T>
        struct Predicate_LessOrEqual {
            bool operator()(const T& item1, const T& item2) const {
                return item1 <= item2;
            }
        
        public:
            typedef T argument_1_type, argument_2_type;
            typedef bool return_type;
        };

        template <class T>
        struct Predicate_Greater {
            bool operator()(const T& item1, const T& item2) const {
                return item1 > item2;
            }
        
        public:
            typedef T argument_1_type, argument_2_type;
            typedef bool return_type;
        };

        template <class T>
        struct Predicate_GreaterOrEqual {
            bool operator()(const T& item1, const T& item2) const {
                return item1 >= item2;
            }
        
        public:
            typedef T argument_1_type, argument_2_type;
            typedef bool return_type;
        };

        template <class T>
        struct Compare {
            int operator()(const T& item1, const T& item2) const {
                return Predicate_Greater<T>::operator()(item1, item2) ?
                    1 :
                    (Predicate_Less<T>::operator()(item1, item2) ?
                        -1 :
                        0
                    )
                ;
            }
        
        public:
            typedef T argument_1_type, argument_2_type;
            typedef int return_type;
        };

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_COMMON_H_ */