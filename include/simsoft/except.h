/**
 * @file except.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief C and C++ exceptions header.
 * @version 0.2
 * @date 2020-07-27
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/**
 * @defgroup cexcept C Exceptions
 * @brief Exception handling in C
 * @par
 * The SimSoft Library is built to allow for C-style exception handling using @c setjmp and
 * @c longjmp. Library functions, instead of returning error codes when an error occurs, throws
 * error codes as an exception that must be caught using the library's exception handling functions.
 * If an exception is raised without a TRY()-CATCH()- FINALLY() block to handle the raised
 * exception, the program immediately exits.
 * 
 * @par Notes on C++ Interoperability
 * Users of the SimSoft C++ library should not to intermix native C++ exception handling features
 * and the SimSoft C exception handling functions and macros.
 */

#ifndef SIMSOFT_EXCEPT_H
#define SIMSOFT_EXCEPT_H_

#include <setjmp.h>

#include "macro.h"
#include "typedefs.h"

// == C EXCEPTION MACROS ===========================================================================

/**
 * @ingroup cexcept
 * @brief Throws an error code and sets an error message.
 * 
 * @param rc  Error code to throw.
 * @param ... Format string and parameters.
 */
#define THROW(rc, ...) do {                                                   \
    if (rc) C_API_PREFIX(SimSoft, sim_except_throw(rc, UNPACK(__VA_ARGS__))); \
} while (0)

/**
 * @ingroup cexcept
 * @brief Rethrows the current exception.
 */
#define RETHROW() do {                                                                          \
    if (C_API_PREFIX(SimSoft, sim_except_peek()))                                               \
        C_API_PREFIX(SimSoft, sim_except_throw(C_API_PREFIX(SimSoft, sim_except_pop()), NULL)); \
    else                                                                                        \
        C_API_PREFIX(SimSoft, sim_except_pop());                                                \
} while (0)

/**
 * @ingroup cexcept
 * @brief Execute code under a new exception state.
 * 
 * @param ... Code to be executed where an exception is to be caught.
 * 
 * @remarks TRY() must be followed by any number of @b CATCH blocks, (optionally) a @b CATCH_ALL
 *          block, and a @b FINALLY block. The construct is referred to in its entirety as a
 *          TRY()-CATCH()-FINALLY() block.
 */
#define TRY(...) {                                               \
    switch (setjmp(*C_API_PREFIX(SimSoft, sim_except_push()))) { \
    case 0:                                                      \
        { UNPACK(__VA_ARGS__) }                                  \
        break;

#define _CEXCEPT_CASE(rc) case rc:

/**
 * @def CATCH(rc, ...)
 * @ingroup cexcept
 * @brief Catch a list of given exceptions from a TRY() block.
 * 
 * @param rc  One or more error codes to catch surrounded by parenthesis.
 * @param ... Code to be executed when an error code in @e rc is caught.
 * 
 * @remarks CATCH() must be preceded by a @b TRY block and any number of @b CATCH blocks, and
 *          succeeded by any number of @b CATCH blocks, (optionally) a @b CATCH_ALL block, and a
 *          @b FINALLY block.
 * @remarks Multiple @b CATCH blocks must not contain the same error codes in their @e rc fields.
 */
#define CATCH(rc, ...)                                   \
    FOR_EACH(_CEXCEPT_CASE, INVOKE(UNPACK, rc))          \
        { UNPACK(__VA_ARGS__) }                          \
        C_API_PREFIX(SimSoft, sim_except_reset());       \
        break;

/**
 * @ingroup cexcept
 * @brief Catch any uncaught exception not caught by a previous CATCH() block from a TRY() block.
 * 
 * @param ... Code to be executed when an error code is caught.
 * 
 * @remarks CATCH_ALL() must be preceded by a @b TRY block and any number of @b CATCH blocks, and
 *          succeeded by a @b FINALLY block.
 * @remarks If used, there must only be a single @b CATCH_ALL block following a @b TRY block.
 */
#define CATCH_ANY(...)                             \
    default:                                       \
        { UNPACK(__VA_ARGS__) }                    \
        C_API_PREFIX(SimSoft, sim_except_reset()); \
        break;

/**
 * @ingroup cexcept
 * @brief Clean up code for after a TRY() block, regardless of whether an exception was caught. 
 * 
 * @param ... Code to be executed after a TRY()-CATCH() block.
 * 
 * @remarks FINALLY() must be preceded by a @b TRY block, any number of @b CATCH
 *          blocks, and (optionally) a @b CATCH_ALL block.
 */
#define FINALLY(...)        \
    }                       \
    { UNPACK(__VA_ARGS__) } \
    RETHROW();              \
}

// == EXCEPTIONS LIST ==============================================================================

#define ENUMERATE_EXCEPTIONS \
    __ENUMERATE_EXCEPTION(NullPointer,            NULLPTR)  \
    __ENUMERATE_EXCEPTION(OutOfMemory,            OUTOFMEM) \
    __ENUMERATE_EXCEPTION(CExceptStack,           JUMPBUF)  \
    __ENUMERATE_EXCEPTION(ZeroDivision,           ZERODIV)  \
    __ENUMERATE_EXCEPTION(InvalidArgument,        INVALARG) \
    __ENUMERATE_EXCEPTION(OutOfBounds,            OUTOFBND) \
    __ENUMERATE_EXCEPTION(UnsupportedOperation,   UNSUPRTD) \
    __ENUMERATE_EXCEPTION(UnimplementedOperation, UNIMPLTD) \
    __ENUMERATE_EXCEPTION(BadFilePointer,         BADFILE)

// == PUBLIC API ===================================================================================

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C_API

// ---- Enums and typedefs -------------------------------------------------------------------------

        /**
         * @ingroup cexcept
         * @brief Enumeration of error codes used by the SimSoft library.
         * 
         * @var Sim_ErrorCode::SIM_ERR_NULLPTR
         *     A nullptr was passed into the function.
         * @var Sim_ErrorCode::SIM_ERR_OUTOFMEM
         *     Ran out of memory when attempting to allocate something.
         * @var Sim_ErrorCode::SIM_ERR_JUMPBUF
         *     An error occured with the C exception stack.
         * @var Sim_ErrorCode::SIM_ERR_ZERODIV
         *     Arithmetic zero-division error.
         * @var Sim_ErrorCode::SIM_ERR_INVALARG
         *     Invalid argument passed into function.
         * @var Sim_ErrorCode::SIM_ERR_OUTOFBND
         *     Index argument out of bounds of container.
         * @var Sim_ErrorCode::SIM_ERR_UNSUPRTD
         *     Unsupported operation on given platform.
         * @var Sim_ErrorCode::SIM_ERR_UNIMPLTD
         *     Unimplemented/unfinished operation.
         * @var Sim_ErrorCode::SIM_ERR_BADFILE
         *     Bad or invalid file descriptor.
         */
        typedef enum Sim_ErrorCode {
            SIM_NO_ERROR, ///< Indicates no error.

#           define __ENUMERATE_EXCEPTION(irrelevant, x) SIM_ERR_ ## x,
                ENUMERATE_EXCEPTIONS
#           undef __ENUMERATE_EXCEPTION

            SIM_ERR___COUNT ///< Number of defined error codes.
        } Sim_ErrorCode;

        /**
         * @ingroup cexcept
         * @brief Callback function for when the current exception state is reset.
         * 
         * @param[in] data Data associated with this on_reset function when it was registered.
         */
        typedef void (C_CALL * Sim_ExceptOnResetProc)(Sim_Variant data);

        /**
         * @ingroup cexcept
         * @brief Callback function for when an exception is thrown.
         * 
         * @param[in] error_code    The error code that was thrown.
         * @param[in] error_message The error message associated with the above error code.
         * @param[in] data          Data associated with this on_throw function when it was
         *                          registered.
         */
        typedef void (C_CALL * Sim_ExceptOnThrowProc)(
            int         error_code,
            const char* error_message,
            Sim_Variant data
        );

// ---- C API --------------------------------------------------------------------------------------

        /**
         * @ingroup cexcept
         * @brief Pushes a new jump buffer to the exception stack.
         * 
         * @returns Pointer to a jmp_buf used by @e setjmp.
         * 
         * @throw SIM_ERR_OUTOFMEM if the exception stack is full.
         * 
         * @remarks This function should not be used directly; use the @b THROW macro instead.
         * 
         * @sa sim_except_pop
         */
        DYNAPI_PROC(jmp_buf *const, sim_except_push,, void);

        /**
         * @ingroup cexcept
         * @brief Pops the current exception state off the exception stack.
         * 
         * @returns The error code of the previous exception state.
         * 
         * @throw SIM_ERR_JUMPBUF if the exception stack is empty.
         * 
         * @sa sim_except_push
         */
        DYNAPI_PROC(int, sim_except_pop,, void);

        /**
         * @ingroup cexcept
         * @brief Peeks at the error code of the current exception state.
         * 
         * @returns The error code of the current exception state.
         * 
         * @sa sim_except_reset
         */
        DYNAPI_PROC(int, sim_except_peek,, void);

        /**
         * @ingroup cexcept
         * @brief Resets the error code of the current exception state to @b SIM_NO_ERROR.
         * 
         * @sa sim_except_peek
         * @sa sim_except_on_reset
         */
        DYNAPI_PROC(void, sim_except_reset,, void);

        /**
         * @ingroup cexcept
         * @brief Throws an exception.
         * 
         * @param[in] error_code    The error code to be thrown as an exception.
         * @param[in] error_message A format string explaining why the exception was thrown or
         *                          @b NULL.
         * @param[in] ...           Format string parameters for @e error_message.
         * 
         * @remarks If @e error_code equals @b SIM_NO_ERROR, this function will throw error code 1.
         * @remarks If @e error_message is @b NULL, the error message set by the previous call to
         *          sim_except_throw will be unchanged.
         * 
         * @sa sim_except_on_throw
         * @sa sim_except_on_no_catch
         */
        DYNAPI_PROC(void, sim_except_throw, NO_RETURN PRINTF_SPEC(2, 3),
            int         error_code,
            const char* error_message,
            ...
        );

        /**
         * @ingroup cexcept
         * @brief Registers a callback function to be called when the exception state is reset.
         * 
         * @param[in] callback_proc The function to be called.
         * @param[in] callback_data The @e data parameter to be passed to @e callback_proc.
         * 
         * @remarks The callback is registered until the current exception state is reset; i.e.
         *          when sim_except_reset is called.
         * 
         * @sa sim_except_reset
         * @sa sim_except_on_throw
         * @sa sim_except_on_no_catch
         */
        DYNAPI_PROC(void, sim_except_on_reset,,
            Sim_ExceptOnResetProc callback_proc,
            Sim_Variant           callback_data
        );

        /**
         * @ingroup cexcept
         * @brief Registers a callback function to be called when an exception is thrown.
         * 
         * @param[in] callback_proc The function to be called.
         * @param[in] callback_data The @e data parameter to be passed to @e callback_proc.
         * 
         * @remarks The callback is registered only for the duration of the current exception
         *          state; i.e. until sim_except_pop is called.
         * 
         * @sa sim_except_throw
         * @sa sim_except_on_reset
         * @sa sim_except_on_no_catch
         */
        DYNAPI_PROC(void, sim_except_on_throw,,
            Sim_ExceptOnThrowProc callback_proc,
            Sim_Variant           callback_data
        );

        /**
         * @ingroup cexcept
         * @brief Registers a callback function to be called when an exception is not caught by a
         *        @b CATCH block.
         * 
         * @param[in] callback_proc The function to be called.
         * @param[in] callback_data The @e data parameter to be passed to @e callback_proc.
         * 
         * @remarks The callback is registered only for the duration of the current exception
         *          state; i.e. until sim_except_pop is called.
         * 
         * @sa sim_except_throw
         * @sa sim_except_on_reset
         * @sa sim_except_on_throw
         */
        DYNAPI_PROC(void, sim_except_on_no_catch,,
            Sim_ExceptOnThrowProc callback_proc,
            Sim_Variant           callback_data
        );

    CPP_NAMESPACE_C_API_END // end C API
#   ifdef __cplusplus       // start C++ API

#   endif // __cplusplus    // end C++ API
CPP_NAMESPACE_END(SimSoft)

#endif // SIMSOFT_EXCEPT_H_
