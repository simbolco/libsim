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
    __ENUMERATE_RETURN_CODE(ERR_UNIMPLTD)

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */
        /**
         * @union Sim_Variant
         * @headerfile common.h "simsoft/common.h"
         * @brief Union of different C data types.
         * 
         * @var byte  Signed byte.
         * @var ubyte Unsigned byte.
         * 
         * @var signed8  8-bit signed integral.
         * @var signed16 16-bit signed integral.
         * @var signed32 32-bit signed integral.
         * @var signed64 64-bit signed integral.
         * 
         * @var unsigned8  8-bit unsigned integral.
         * @var unsigned16 16-bit unsigned integral.
         * @var unsigned32 32-bit unsigned integral.
         * @var unsigned64 64-bit unsigned integral.
         * 
         * @var float32 Single-precision floating-point value.
         * @var float64 Double-precision floating-point value.
         * 
         * @var signed_char      Signed char.
         * @var signed_short     Signed short.
         * @var signed_int       Signed int.
         * @var signed_long      Signed long.
         * @var signed_long_long Signed long long.
         * 
         * @var unsigned_char      Unsigned char.
         * @var unsigned_short     Unsigned short.
         * @var unsigned_int       Unsigned int.
         * @var unsigned_long      Unsigned long.
         * @var unsigned_long_long Unsigned long long.
         * 
         * @var signed_size   Signed size type.
         * @var unsigned_size Unsigned size type.
         * 
         * @var string  C string.
         * @var pointer Void pointer.
         * 
         * @var upointer Unsigned integer pointer type.
         * @var spointer Signed integer pointer type.
         * @var ptr_diff Pointer difference type.
         * 
         * @var time Time type.
         */
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
         * @headerfile common.n "simsoft/common.h"
         * @brief C enumeration of return values of library functions.
         * 
         * @var SIM_RC_SUCCESS      The function did what it was supposed to do without any issues.
         * @var SIM_RC_FAILURE      The function did what it was supposed to do, but unsuccessfully.
         * @var SIM_RC_ERR_NULLPTR  A nullptr was passed into the function.
         * @var SIM_RC_ERR_OUTOFMEM Ran out of memory when attempting to allocate something.
         * @var SIM_RC_ERR_ZERODIV  Arithmetic zero-division error.
         * @var SIM_RC_ERR_INVALARG Invalid argument passed into function.
         * @var SIM_RC_ERR_OUTOFBND Index argument out of bounds of container.
         * @var SIM_RC_ERR_UNSUPRTD Unsupported operation on given platform.
         * @var SIM_RC_ERR_NOTFOUND Item not found in container.
         * @var SIM_RC_ERR_UNIMPLTD Unimplemented/unfinished operation.
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
         * @typedef Sim_ComparisonFunction
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer for a comparison between two values.
         * 
         * @param[in] item1 Pointer to the first item to compare.
         * @param[in] item2 Pointer to the second item to compare.
         * 
         * @return An int > 0 if item1 > item2; int < 0 if item2 > item1; 0 if item1 == item2.
         */
        typedef int (*Sim_ComparisonFuncPtr)(
            const void* const item1,
            const void* const item2
        );

        /**
         * @typedef Sim_PredicateFuncPtr
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer to a predicate function that takes two values.
         * 
         * @param[in] item1 Pointer to the first item to compare.
         * @param[in] item2 Pointer to the second item to compare.
         * 
         * @return @c true or @c false, depending on what the predicate is used for.
         */
        typedef bool (*Sim_PredicateFuncPtr)(
            const void* const item1,
            const void* const item2
        );

        /**
         * @typedef Sim_ForEachFuncPtr
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
        typedef bool (*Sim_ForEachFuncPtr)(
            void *const item,
            const size_t index,
            Sim_Variant userdata
        );

        /**
         * @typedef Sim_ConstForEachFuncPtr
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
        typedef bool (*Sim_ConstForEachFuncPtr)(
            const void *const item,
            const size_t index,
            Sim_Variant userdata
        );

        /**
         * @typedef Sim_HashFuncPtr
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer that hashes a value.
         * 
         * @param[in] item    Pointer to the item being hashed.
         * @param[in] attempt The hashing attempt; used for double-hashing of values in hash tables.
         * 
         * @return A hash value for the given item.
         */
        typedef size_t (*Sim_HashFuncPtr)(
            const void *const item,
            const size_t attempt
        );

        /**
         * @typedef Sim_FilterFuncPtr
         * @headerfile common.h "simsoft/common.h"
         * @brief Function pointer used when filtering a collection of items.
         * 
         * @param[in] item     Pointer to the item being filtered.
         * @param[in] userdata User-provided callback data.
         *
         * @return @c true  to keep the item in the given container;
         *         @c false to remove.
         */
        typedef bool (*Sim_FilterFuncPtr)(
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