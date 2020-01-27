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
         * @brief C++ exception class
         */
        class SIM_API Exception {
        private:
            static constexpr size_t _BACKTRACE_SIZE = 16;

            const char* _message { "" };
            BacktraceInfo _backtrace_array[_BACKTRACE_SIZE] { 0,0,0,0 };
            bool _has_backtrace { false };
        public:
            Exception(const char* message) noexcept;
            Exception(const Exception& other) noexcept;
            Exception(Exception&& other) noexcept;
            Exception(ReturnCode rc) noexcept;

            virtual ~Exception() noexcept;

            Exception& operator=(const Exception& other) noexcept;

            virtual const char* what() noexcept;
        };

#   endif /* end C++ */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_EXCEPTION_HPP_ */
