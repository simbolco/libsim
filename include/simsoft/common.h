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
#ifndef SIM_API
#   if defined(__WIN32__) || defined(__WINRT__)
#       ifdef __BORLANDC__
#           ifdef SIM_BUILD
#               define SIM_API
#           else
#               define SIM_API __declspec(dllimport)
#           endif
#       else
#           define SIM_API __declspec(dllexport)
#       endif
#   elif defined(__OS2__)
#       ifdef SIM_BUILD
#           define SIM_API __declspec(dllexport)
#       else
#           define SIM_API
#       endif
#   else
#       if defined(__GNUC__) && __GNUC__ >= 4
#           define SIM_API __attribute__ ((visibility("default")))
#      else
#           define SIM_API
#       endif
#   endif
#   ifdef __SYMBIAN32__
#       undef  SIM_API
#       define SIM_API
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

// ------------------------------------------------------------------------------------------------

#ifdef __cplusplus
#   define DEPRECATED(msg) [[deprecated(msg)]]
#elif defined(__GNUC__) || defined(__GNUG__)
#   define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#   define DEPRECATED(msg) __declspec(deprecated(msg))
#else
#   define DEPRECATED(msg)
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

typedef sint8 byte;
typedef uint8 ubyte;

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

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */
        typedef union Sim_Variant {
            // Bytes
            sint8 byte;
            uint8 ubyte;

            // Sized signed integer types
            sint8  signed8;
            sint16 signed16;
            sint32 signed32;
            sint64 signed64;
            
            // Sized unsigned integer types
            uint8  unsigned8;
            uint16 unsigned16;
            uint32 unsigned32;
            uint64 unsigned64;

            // Floating point types
            float  float32;
            double float64;

            // C integral types
            char      signed_char;
            short     signed_short;
            int       signed_int;
            long      signed_long;
            long long signed_long_long;

            // Unsigned C integral types
            unsigned char      unsigned_char;
            unsigned short     unsigned_short;
            unsigned int       unsigned_int;
            unsigned long      unsigned_long;
            unsigned long long unsigned_long_long;

            // Size types
            ssize_t signed_size;
            size_t  unsigned_size;

            // Pointer types
            char* string;
            void* pointer;
            
            // Pointer math types
            uintptr_t upointer;
            intptr_t  spointer;
            ptrdiff_t ptr_diff;

            // Time type
            time_t time;
        } Sim_Variant;

#       define __ENUMERATE_RETURN_CODE(rc_name) \
            SIM_RC_ ##rc_name,

        /**
         * @enum Sim_ReturnCode
         * @brief C enumeration of return values of library functions
         * 
         */
        typedef enum Sim_ReturnCode {
            ENUMERATE_RETURN_CODES
            SIM_RC_ERR_404 = SIM_RC_ERR_NOTFOUND
        } Sim_ReturnCode;

#       undef __ENUMERATE_RETURN_CODE

        typedef enum Sim_DataType {
            SIM_DATATYPE_OTHER    = 0x0,
            SIM_DATATYPE_INTEGRAL = 0x1,
            SIM_DATATYPE_UNSIGNED = 0x2,
            SIM_DATATYPE_FLOAT    = 0x3
        } Sim_DataType;

        /**
         * @struct Sim_TypeInfo
         * @brief Structure used by collections to optimize sorting when using built-in C
         *        numeric types.
         */
        typedef struct Sim_TypeInfo {
            size_t type : 2;
            size_t size : (sizeof (size_t) * 8) - 2;
        } Sim_TypeInfo;

        typedef struct Sim_PointerPair {
            void *const item1;
            void *const item2;
        } Sim_PointerPair;

        typedef int Sim_ComparisonFunction(const void* const, const void* const);

        typedef bool (*Sim_PredicateFuncPtr)(
            const void* const item1,
            const void* const item2
        );

        typedef int (*Sim_ComparisonFuncPtr)(
            const void* const item1,
            const void* const item2
        );

        typedef bool (*Sim_ForEachFuncPtr)(
            void *const item,
            Sim_Variant userdata,
            const size_t index
        );

        typedef bool (*Sim_ConstForEachFuncPtr)(
            const void *const item,
            Sim_Variant userdata,
            const size_t index
        );

        typedef size_t (*Sim_HashFuncPtr)(
            const void* item
        );

        typedef bool (*Sim_FilterFuncPtr)(
            const void *const item,
            Sim_Variant userdata
        );

    CPP_NAMESPACE_C_API_END /* end C API */
 
#   ifdef __cplusplus /* C++ API */
        typedef C_API::Sim_Variant   Variant;

#       define __ENUMERATE_RETURN_CODE(rc_name) rc_name,
        enum ReturnCode {
            ENUMERATE_RETURN_CODES
            ERR_404 = ERR_NOTFOUND
        };
#       undef __ENUMERATE_RETURN_CODE

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