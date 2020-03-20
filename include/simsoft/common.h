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

#ifndef SIMSOFT_COMMON_H
#define SIMSOFT_COMMON_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <math.h>

// Include C stdbool when not in C++ mode
#ifndef __cplusplus
#   include <stdbool.h>
#endif

// -- CRAZY C PREPROCESSOR SHIT FOR ALL SORTS OF THINGS -------------------------------------------

// ---- Used for use of code blocks in macros -----------------------------------------------------
#ifndef UNPACK
#   define UNPACK(...) __VA_ARGS__
#endif
#ifndef STRING
#   define STRING(...) #__VA_ARGS__
#endif

// ---- C calling convention (borrowed from SDL source code) --------------------------------------

#ifndef C_CALL
#   if (defined(__WIN32__) || defined(__WINRT__)) && !defined(__GNUC__)
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

// ---- Dynamic library / Shared object export macros ---------------------------------------------

// (borrowed from SDL source code)
#ifndef EXPORT
#   if defined(__WIN32__) || defined(__WINRT__)
#       ifdef __BORLANDC__
#           ifdef SIM_BUILD
#               define EXPORT
#           else
#               define EXPORT __declspec(dllimport)
#           endif
#       else
#           define EXPORT __declspec(dllexport)
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

// ---- Namespace declaration & C API handling in C++ ---------------------------------------------

#ifdef __cplusplus
#   define CPP_NAMESPACE_START(_name) \
        namespace _name {
#   define CPP_NAMESPACE_END(_name) \
        }
#   define CPP_NAMESPACE_C_API_START \
        namespace C_API { extern "C" {
#   define CPP_NAMESPACE_C_API_END \
        } }
#else
#   define CPP_NAMESPACE_START(_name)
#   define CPP_NAMESPACE_END(_name) 
#   define CPP_NAMESPACE_C_API_START
#   define CPP_NAMESPACE_C_API_END
#endif

// ---- Thread-local storage ----------------------------------------------------------------------

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

// -- C Static assertions -------------------------------------------------------------------------

#ifndef STATIC_ASSERT
#   if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#       define STATIC_ASSERT(_cond, _msg) _Static_assert(_cond, _msg)
#   elif defined(__GNUC__) || defined(__GNUG__)
#       define STATIC_ASSERT(_cond, _msg) \
            ({                                                                     \
                extern int __attribute__((error("assertion failure: '" _msg "'"))) \
                compile_time_check();                                              \
                ((_cond) ? 0 : compile_time_check()), 0;                           \
            })
#   else
#       define STATIC_ASSERT(_cond, _msg)
#   endif
#endif

// -- C Deprecation statement ---------------------------------------------------------------------

#ifdef __cplusplus
#   define DEPRECATED(msg) [[deprecated(msg)]]
#elif defined(__GNUC__) || defined(__GNUG__)
#   define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#   define DEPRECATED(msg) __declspec(deprecated(msg))
#else
#   define DEPRECATED(msg)
#endif

// -- Machine word size check ---------------------------------------------------------------------

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

// ------------------------------------------------------------------------------------------------

typedef int8_t   sint8;
typedef uint8_t  uint8;
typedef int16_t  sint16;
typedef uint16_t uint16;
typedef int32_t  sint32;
typedef uint32_t uint32;
typedef int64_t  sint64;
typedef uint64_t uint64;

#define ENUMERATE_RETURN_CODES            \
    __ENUMERATE_RETURN_CODE(SUCCESS)      \
    __ENUMERATE_RETURN_CODE(FAILURE)      \
    __ENUMERATE_RETURN_CODE(ERR_NULLPTR)  \
    __ENUMERATE_RETURN_CODE(ERR_OUTOFMEM) \
    __ENUMERATE_RETURN_CODE(ERR_ZERODIV)  \
    __ENUMERATE_RETURN_CODE(ERR_INVALARG) \
    __ENUMERATE_RETURN_CODE(ERR_OUTOFBND) \
    __ENUMERATE_RETURN_CODE(ERR_UNSUPRTD) \
    __ENUMERATE_RETURN_CODE(ERR_NOTFOUND) \
    __ENUMERATE_RETURN_CODE(ERR_UNIMPLTD) \
    __ENUMERATE_RETURN_CODE(ERR_BADFILE)

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */
        /**
         * @union Sim_Variant
         * @headerfile common.h "simsoft/common.h"
         * @brief Union of different C data types.
         * 
         * @property signed8  8-bit signed integral.
         * @property signed16 16-bit signed integral.
         * @property signed32 32-bit signed integral.
         * @property signed64 64-bit signed integral (excluded from 32-bit builds).
         * 
         * @property unsigned8  8-bit unsigned integral.
         * @property unsigned16 16-bit unsigned integral.
         * @property unsigned32 32-bit unsigned integral.
         * @property unsigned64 64-bit unsigned integral (excluded from 32-bit builds).
         * 
         * @property float32 Single-precision floating-point value.
         * @property float64 Double-precision floating-point value (excluded from 32-bit builds).
         * 
         * @property signed_char      Signed char.
         * @property signed_short     Signed short.
         * @property signed_int       Signed int.
         * @property signed_long      Signed long.
         * @property signed_long_long Signed long long (excluded if larger than word size).
         * 
         * @property unsigned_char      Unsigned char.
         * @property unsigned_short     Unsigned short.
         * @property unsigned_int       Unsigned int.
         * @property unsigned_long      Unsigned long.
         * @property unsigned_long_long Unsigned long long (excluded if larger than word size).
         * 
         * @property signed_size   Signed size type.
         * @property unsigned_size Unsigned size type.
         * 
         * @property string  C string.
         * @property pointer Void pointer.
         * 
         * @property upointer Unsigned integer pointer type.
         * @property spointer Signed integer pointer type.
         * @property ptr_diff Pointer difference type.
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

#       define __ENUMERATE_RETURN_CODE(rc_name) \
            SIM_RC_ ##rc_name,

        /**
         * @enum Sim_ReturnCode
         * @headerfile common.h "simsoft/common.h"
         * @brief C enumeration of return values of library functions.
         * 
         * @property SIM_RC_SUCCESS      The function did what it was supposed to do without any
         *                               issues.
         * @property SIM_RC_FAILURE      The function did what it was supposed to do, but
         *                               unsuccessfully.
         * @property SIM_RC_ERR_NULLPTR  A nullptr was passed into the function.
         * @property SIM_RC_ERR_OUTOFMEM Ran out of memory when attempting to allocate something.
         * @property SIM_RC_ERR_ZERODIV  Arithmetic zero-division error.
         * @property SIM_RC_ERR_INVALARG Invalid argument passed into function.
         * @property SIM_RC_ERR_OUTOFBND Index argument out of bounds of container.
         * @property SIM_RC_ERR_UNSUPRTD Unsupported operation on given platform.
         * @property SIM_RC_ERR_NOTFOUND Item not found in container.
         * @property SIM_RC_ERR_UNIMPLTD Unimplemented/unfinished operation.
         * @property SIM_RC_ERR_BADFILE  Bad or invalid file.
         */
        typedef enum Sim_ReturnCode {
            ENUMERATE_RETURN_CODES
            SIM_RC_ERR_404 = SIM_RC_ERR_NOTFOUND
        } Sim_ReturnCode;

#       undef __ENUMERATE_RETURN_CODE

        /**
         * @enum Sim_DataType
         * @headerfile common.h "simsoft/common.h"
         * @brief C enumeration of data type info.
         * 
         * @var SIM_DATATYPE_OTHER    User-defined type.
         * @var SIM_DATATYPE_INTEGRAL Signed integral type.
         * @var SIM_DATATYPE_UNSIGNED Unsigned integral type.
         * @var SIM_DATATYPE_FLOAT    Floating-point type.
         */
        typedef enum Sim_DataType {
            SIM_DATATYPE_OTHER    = 0x0,
            SIM_DATATYPE_INTEGRAL = 0x1,
            SIM_DATATYPE_UNSIGNED = 0x2,
            SIM_DATATYPE_FLOAT    = 0x3
        } Sim_DataType;

        /**
         * @struct Sim_TypeInfo
         * @headerfile common.h "simsoft/common.h"
         * @brief Structure used by collections to optimize sorting when using built-in C
         *        numeric types.
         * 
         * @property type The data type being represented; one of four Sim_DataType values.
         * @property size The size of the type in bytes.
         */
        typedef struct Sim_TypeInfo {
            size_t type : 2;
            size_t size : (sizeof (size_t) * 8) - 2;
        } Sim_TypeInfo;

        /**
         * @struct Sim_PointerPair
         * @headerfile common.h "simsoft/common.h"
         * @brief Structure containing two generic pointers.
         * 
         * @property item1 First pointer.
         * @property item2 Second pointer.
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
         * @typedef Sim_HashProc
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer that hashes a value.
         * 
         * @param[in] item    Pointer to the item being hashed.
         * @param[in] attempt The hashing attempt; used for double-hashing of values in hash tables.
         * 
         * @return A hash value for the given item.
         */
        typedef size_t (*Sim_HashProc)(
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
         * @fn Sim_ReturnCode sim_return_code(0)
         * @headerfile common.h "simsoft/common.h"''
         * @brief Gets the return code from SimSoft library functions.
         * 
         * @return A thread-local internal return code from the previous library function call.
         */
        extern EXPORT Sim_ReturnCode C_CALL sim_return_code(void);

    CPP_NAMESPACE_C_API_END /* end C API */
 
#   ifdef __cplusplus /* C++ API */
        typedef C_API::Sim_Variant Variant;

#       define __ENUMERATE_RETURN_CODE(rc_name) rc_name,
        enum ReturnCode {
            ENUMERATE_RETURN_CODES
            ERR_404 = ERR_NOTFOUND
        };
#       undef __ENUMERATE_RETURN_CODE

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
        
        /*
        template <bool B, class T = void>
        struct EnableIf {
        };

        template <class T>
        struct EnableIf<true, T> {
            typedef T Type;
        };

        template <class T, T v>
        struct IntegralConstant {
            static constexpr T value = v;
            typedef T ValueType;
            typedef IntegralConstant Type;
            constexpr operator ValueType() const { return value; }
            constexpr ValueType operator()() const { return value; }
        };

        typedef IntegralConstant<bool, false> FalseType;
        typedef IntegralConstant<bool, true> TrueType;

        template <class T>
        struct __IsPointerHelper : FalseType {
        };

        template <class T>
        struct __IsPointerHelper<T*> : TrueType {
        };

        template <class T>
        struct IsPointer : __IsPointerHelper<typename RemoveCV<T>::Type> {
        };

        template <class>
        struct IsFunction : FalseType {
        };

        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...)> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...)> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) const> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) const> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) volatile> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) volatile> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) const volatile> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) const volatile> : TrueType {
        };

        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...)&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...)&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) const&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) const&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) volatile&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) volatile&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) const volatile&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) const volatile&> : TrueType {
        };

        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) &&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) &&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) const&&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) const&&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) volatile&&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) volatile&&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args...) const volatile&&> : TrueType {
        };
        template <class Ret, class... Args>
        struct IsFunction<Ret(Args..., ...) const volatile&&> : TrueType {
        };
        */

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_COMMON_H */