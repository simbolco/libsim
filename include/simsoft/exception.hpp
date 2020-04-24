/**
 * @file exception.hpp
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief C++ header for exception types
 * @version 0.1
 * @date 2020-01-21
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_EXCEPTION_HPP_
#define SIMSOFT_EXCEPTION_HPP_

#include "./common.h"
#include "./debug.h"

CPP_NAMESPACE_START(SimSoft)
#   ifdef __cplusplus /* C++ API */
        /**
         * @def ENUMERATE_EXCEPTIONS
         * @brief A user-expandable macro for the various C++ exception types.
         * @details Define __ENUMERATE_EXCEPTION(class, rc) to use this macro.
         */
#       define ENUMERATE_EXCEPTIONS                                     \
            __ENUMERATE_EXCEPTION(NullPointer,            ERR_NULLPTR)  \
            __ENUMERATE_EXCEPTION(OutOfMemory,            ERR_OUTOFMEM) \
            __ENUMERATE_EXCEPTION(ZeroDivision,           ERR_ZERODIV)  \
            __ENUMERATE_EXCEPTION(InvalidArgument,        ERR_INVALARG) \
            __ENUMERATE_EXCEPTION(OutOfBounds,            ERR_OUTOFBND) \
            __ENUMERATE_EXCEPTION(UnsupportedOperation,   ERR_UNSUPRTD) \
            __ENUMERATE_EXCEPTION(UnimplementedOperation, ERR_UNIMPLTD) \
            __ENUMERATE_EXCEPTION(BadFilePointer,         ERR_BADFILE)

        /**
         * @class SimSoft::Exception
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Base class for C++ exceptions.
         */
        class EXPORT Exception {
        private:
            static constexpr size_t _BACKTRACE_SIZE = 16;

            BacktraceInfo _backtrace_array[_BACKTRACE_SIZE] { 0,0,0,0 };
            bool _has_backtrace { false };

        protected:
            const char* _message;
            bool _owns_message { false };

        public:
            /**
             * @fn Exception(const char*)
             * @brief Constructs a new Exception with a custom message.
             * 
             * @param[in] message User-specified error message.
             */
            Exception(const char* message) noexcept;
            /**
             * @fn Exception(const SimSoft::Exception&)
             * @brief Constructs a new Exception by copying the contents of another.
             * 
             * @param[in] other Exception to copy.
             */
            Exception(const Exception& other) noexcept;
            /**
             * @fn Exception(SimSoft::Exception&&)
             * @brief Constructs a new Exception by moving the contents of another.
             * 
             * @param[in] other Exception to move.
             */
            Exception(Exception&& other) noexcept;
            /**
             * @fn Exception(SimSoft::ReturnCode)
             * @brief Constructs a new Exception from a SimSoft::ReturnCode.
             * 
             * @param[in] rc Return code.
             */
            Exception(ReturnCode rc) noexcept;

            /**
             * @fn ~Exception()
             * @brief Destroys an Exception.
             */
            virtual ~Exception() noexcept;

            /**
             * @fn what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
            virtual const char* what() noexcept;
        };

        /**
         * @class NullPointerException
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Exception for when a null pointer is passed to a function or is encountered
         *        during a function's runtime.
         */
            /**
             * @fn NullPointerException::NullPointerException()
             * @details Constructs a new NullPointerException.
             */
            /**
             * @fn NullPointerException::NullPointerException(const char*)
             * @details Constructs a new NullPointerException with a custom message.
             * 
             * @param[in] message A string clarifying the exceptional behavior.
             */
            /**
             * @fn NullPointerException::~NullPointerException()
             * @brief Destroys a NullPointerException.
             */
            /**
             * @fn const char* NullPointerException::what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
        /**
         * @class OutOfMemoryException
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Exception for when the runtime is unable to allocate any more memory, usually
         *        because memory has run out.
         */
            /**
             * @fn OutOfMemoryException::OutOfMemoryException()
             * @details Constructs a new OutOfMemoryException.
             */
            /**
             * @fn OutOfMemoryException::OutOfMemoryException(const char*)
             * @details Constructs a new OutOfMemoryException with a custom message.
             * 
             * @param[in] message A string clarifying the exceptional behavior.
             */
            /**
             * @fn OutOfMemoryException::~OutOfMemoryException()
             * @brief Destroys an OutOfMemoryException.
             */
            /**
             * @fn const char* OutOfMemoryException::what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
        /**
         * @class ZeroDivisionException
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Exception for when zero is passed to a division function or the runtime attempts
         *        to divide by zero.
         */
            /**
             * @fn ZeroDivisionException::ZeroDivisionException()
             * @details Constructs a new ZeroDivisionException.
             */
            /**
             * @fn ZeroDivisionException::ZeroDivisionException(const char*)
             * @details Constructs a new ZeroDivisionException with a custom message.
             * 
             * @param[in] message A string clarifying the exceptional behavior.
             */
            /**
             * @fn ZeroDivisionException::~ZeroDivisionException()
             * @brief Destroys a ZeroDivisionException.
             */
            /**
             * @fn const char* ZeroDivisionException::what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
        /**
         * @class InvalidArgumentException
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Exception for when an invalid argument is passed to a function.
         */
            /**
             * @fn InvalidArgumentException::InvalidArgumentException()
             * @details Constructs a new InvalidArgumentException.
             */
            /**
             * @fn InvalidArgumentException::InvalidArgumentException(const char*)
             * @details Constructs a new InvalidArgumentException with a custom message.
             * 
             * @param[in] message A string clarifying the exceptional behavior.
             */
            /**
             * @fn InvalidArgumentException::~InvalidArgumentException()
             * @brief Destroys an InvalidArgumentException.
             */
            /**
             * @fn const char* InvalidArgumentException::what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
        /**
         * @class OutOfBoundsException
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Exception for when an array index passed to a function is outside of the array's
         *        boundaries.
         */
            /**
             * @fn OutOfBoundsException::OutOfBoundsException()
             * @details Constructs a new OutOfBoundsException.
             */
            /**
             * @fn OutOfBoundsException::OutOfBoundsException(const char*)
             * @details Constructs a new OutOfBoundsException with a custom message.
             * 
             * @param[in] message A string clarifying the exceptional behavior.
             */
            /**
             * @fn OutOfBoundsException::~OutOfBoundsException()
             * @brief Destroys an OutOfBoundsException.
             */
            /**
             * @fn const char* OutOfBoundsException::what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
        /**
         * @class UnsupportedOperationException
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Exception for when a function is invoked and is not supported on the program's
         *        built platform.
         */
            /**
             * @fn UnsupportedOperationException::UnsupportedOperationException()
             * @details Constructs a new UnsupportedOperationException.
             */
            /**
             * @fn UnsupportedOperationException::UnsupportedOperationException(const char*)
             * @details Constructs a new UnsupportedOperationException with a custom message.
             * 
             * @param[in] message A string clarifying the exceptional behavior.
             */
            /**
             * @fn UnsupportedOperationException::~UnsupportedOperationException()
             * @brief Destroys an UnsupportedOperationException.
             */
            /**
             * @fn const char* UnsupportedOperationException::what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
        /**
         * @class UnimplementedOperationException
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Exception for when a function is invoked and has not been implemented.
         */
            /**
             * @fn UnimplementedOperationException::UnimplementedOperationException()
             * @details Constructs a new UnimplementedOperationException.
             */
            /**
             * @fn UnimplementedOperationException::UnimplementedOperationException(const char*)
             * @details Constructs a new UnimplementedOperationException with a custom message.
             * 
             * @param[in] message A string clarifying the exceptional behavior.
             */
            /**
             * @fn UnimplementedOperationException::~UnimplementedOperationException()
             * @brief Destroys an UnimplementedOperationException.
             */
            /**
             * @fn const char* UnimplementedOperationException::what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
        /**
         * @class BadFilePointerException
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief Exception for when a function is passed an invalid file pointer.
         */
            /**
             * @fn BadFilePointerException::BadFilePointerException()
             * @details Constructs a new BadFilePointerException.
             */
            /**
             * @fn BadFilePointerException::BadFilePointerException(const char*)
             * @details Constructs a new BadFilePointerException with a custom message.
             * 
             * @param[in] message A string clarifying the exceptional behavior.
             */
            /**
             * @fn BadFilePointerException::~BadFilePointerException()
             * @brief Destroys a BadFilePointerException.
             */
            /**
             * @fn const char* BadFilePointerException::what()
             * @brief Retrieves an error string from the exception.
             * 
             * @return Error string, or nullptr if unable to generate an error string.
             */
#       define __ENUMERATE_EXCEPTION(class_name, return_code)                \
            class EXPORT class_name ##Exception final : private Exception {  \
            private:                                                         \
                const char* _extended_message {nullptr};                     \
                char* _cached_message {nullptr};                             \
            public:                                                          \
                class_name ##Exception() noexcept;                           \
                class_name ##Exception(const char* message) noexcept;        \
                                                                             \
                ~class_name ##Exception() noexcept override;                 \
                                                                             \
                const char* what() noexcept override;                        \
            };

            ENUMERATE_EXCEPTIONS
#       undef __ENUMERATE_EXCEPTION

#   endif /* end C++ */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_EXCEPTION_HPP_ */
