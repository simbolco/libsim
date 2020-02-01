/**
 * @file exception.hpp
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief C++ header for exception type
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
         * @class SimSoft::Exception
         * @headerfile exception.hpp "simsoft/exception.hpp"
         * @brief C++ exception class
         */
        class SIM_API Exception {
        private:
            static constexpr size_t _BACKTRACE_SIZE = 16;

            const char* _message { "" };
            BacktraceInfo _backtrace_array[_BACKTRACE_SIZE] { 0,0,0,0 };
            bool _has_backtrace { false };
        public:
            /**
             * @fn SimSoft::Exception::Exception(const char*)
             * @brief Constructs a new Exception with a custom message.
             * 
             * @param[in] message User-specified error message.
             */
            Exception(const char* message) noexcept;
            /**
             * @fn SimSoft::Exception::Exception(const SimSoft::Exception&)
             * @brief Constructs a new Exception by copying the contents of another.
             * 
             * @param[in] other Exception to copy.
             */
            Exception(const Exception& other) noexcept;
            /**
             * @fn SimSoft::Exception::Exception(SimSoft::Exception&&)
             * @brief Constructs a new Exception by moving the contents of another.
             * 
             * @param[in] other Exception to move.
             */
            Exception(Exception&& other) noexcept;
            /**
             * @fn SimSoft::Exception::Exception(SimSoft::ReturnCode)
             * @brief Constructs a new Exception from a ReturnCode.
             * 
             * @param[in] rc Return code.
             */
            Exception(ReturnCode rc) noexcept;

            /**
             * @fn SimSoft::Exception::~Exception()
             * @brief Virtualized Exception destructor.
             */
            virtual ~Exception() noexcept;

            /**
             * @fn const char* SimSoft::Exception::what()
             * @brief Retrieve an error string from the exception.
             * 
             * @return Error string.
             */
            virtual const char* what() noexcept;
        };

#   endif /* end C++ */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_EXCEPTION_HPP_ */
