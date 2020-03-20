/**
 * @file cmdlnopt.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for command line option handling.
 * @version 0.1
 * @date 2020-03-16
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_CMDLNOPT_H_
#define SIMSOFT_CMDLNOPT_H_

#include "./common.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

        /**
         * @enum Sim_CmdLnOptHasArgument
         * @headerfile cmdlnopt.h "simsoft/cmdlnopt.h"
         * @brief C enumeration of command line option argument specifications.
         * 
         * @property SIM_CMDLNOPT_NO_ARGUMENT       This option explicitly takes no arguments.
         * @property SIM_CMDLNOPT_REQUIRED_ARGUMENT This option explicitly requires at least 1 
         *                                          argument.
         * @property SIM_CMDLNOPT_OPTIONAL_ARGUMENT This option may or may not take arguments.
         */
        typedef enum Sim_CmdLnOptHasArgument {
            SIM_CMDLNOPT_NO_ARGUMENT = 0,
            SIM_CMDLNOPT_REQUIRED_ARGUMENT = 1,
            SIM_CMDLNOPT_OPTIONAL_ARGUMENT = 2
        } Sim_CmdLnOptHasArgument;

        /**
         * @enum Sim_CmdLnOptError
         * @headerfile cmdlnopt.h "simsoft/cmdlnopt.h"
         * @brief C enumeration of errors that can occur whilst handling command line options.
         * 
         * @property SIM_CMDLNOPT_ERR_UNKNOWN_OPTION   An unknown option was provided.
         * @property SIM_CMDLNOPT_ERR_MISSING_ARGUMENT The given option was not provided an
         *                                             argument.
         * @property SIM_CMDLNOPT_ERR_NONFLAG_OPTION   The given option that requires arguments was
         *                                             included in a string of options that
         *                                             explicitly take no arguments.
         */
        typedef enum Sim_CmdLnOptError {
            SIM_CMDLNOPT_ERR_UNKNOWN_OPTION = 0,
            SIM_CMDLNOPT_ERR_MISSING_ARGUMENT = 1,
            SIM_CMDLNOPT_ERR_NONFLAG_OPTION = 2
        } Sim_CmdLnOptError;

        /**
         * @struct Sim_CmdLnOptArgs
         * @headerfile cmdlnopt.h "simsoft/cmdlnopt.h"
         * @brief Opaque structure used by option handler callbacks.
         */
        typedef struct _Sim_CmdLnOptArgs Sim_CmdLnOptArgs;

        /**
         * @typedef Sim_CmdLnOptHandlerProc
         * @headerfile cmdlnopt.h "simsoft/cmdlnopt.h"
         * @brief Function pointer for a command line option handler callback.
         *  
         * @param[in]     args_state_ptr         Opaque pointer used to retrieve extra argstrings.
         * @param[in]     option_long_name       The long name of the option being handled.
         * @param[in]     option_short_codepoint The UTF codepoint of the option being handled.
         * @param[in]     argument_string        The first argstring passed to the option.
         * @param[out]    exit_code_out_ptr      Pointer to the exit code.
         * @param[in,out] userdata               User-provided variant.
         * 
         * @returns @c false to continue handling command line options;
         *          @c true  to break and stop handling any further command line options.
         * 
         * @sa Sim_CmdLnOptStruct
         */
        typedef bool (*Sim_CmdLnOptHandlerProc)(
            Sim_CmdLnOptArgs *const args_state_ptr,
            const char*             option_long_name,
            uint32                  option_short_codepoint,
            const char*             argument_string,
            int *const              exit_code_out_ptr,
            Sim_Variant             userdata
        );

        /**
         * @struct Sim_CmdLnOptHandler
         * @headerfile cmdlnopt.h "simsoft/cmdlnopt.h"
         * @brief Structure defining a command line option handler.
         * 
         * @property long_name       The long name of this option.
         * @property short_codepoint The UTF codepoint of the short name of this option.
         * @property has_argument    Determines how this option takes arguments.
         * @property handler_proc    Callback function pointer for handling this option.
         * 
         * @sa Sim_CmdLnOptHasArgument
         * @sa Sim_CmdLnOptHandlerProc
         */
        typedef struct Sim_CmdLnOptHandler {
            const char*             long_name;
            uint32                  short_codepoint;
            Sim_CmdLnOptHasArgument has_argument;
            Sim_CmdLnOptHandlerProc handler_proc;
        } Sim_CmdLnOptHandler;

        /**
         * @fn const char* sim_cmdlnopt_next_argstring(1)
         * @headerfile cmdlnopt.h "simsoft/cmdlnopt.h"
         * @relates Sim_CmdLnOptArgs
         * @brief Retrieves the next argstring from the given args state.
         * A command line option handler callback function is provided with an @e args_state_ptr
         * specifically for use in this function.
         * 
         * @param[in] args_state_ptr Opaque pointer provided to option handler callbacks.
         * 
         * @returns The next argstring in the @e argv provided to @e sim_cmdlnopt_handle_options;
         *          @c NULL if no more argstrings are available or on error (see remarks);
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e args_state_ptr is @c NULL;
         *     @b SIM_RC_SUCCESS     otherwise.
         */
        extern EXPORT const char* C_CALL sim_cmdlnopt_next_argstring(
            Sim_CmdLnOptArgs *const args_state_ptr
        );

        /**
         * @typedef Sim_CmdLnOptErrorProc
         * @headerfile cmdlnopt.h "simsoft/cmdlnopt.h"
         * @brief Function pointer for a command line option error handler.
         * 
         * @param[in]     program_string    The program name as invoked by the command line.
         * @param[in]     option_string     The option that caused the error.
         * @param[in]     error_type        The type of error to be handled.
         * @param[out]    exit_code_out_ptr Pointer to the exit code.
         * @param[in,out] userdata          User-provided variant.
         * 
         * @returns @c false to continue handling command line options;
         *          @c true  to break and stop handling any further command line options.
         * 
         * @sa Sim_CmdLnOptError
         */
        typedef bool (*Sim_CmdLnOptErrorProc)(
            const char*       program_string,
            const char*       option_string,
            Sim_CmdLnOptError error_type,
            int *const        exit_code_out_ptr,
            Sim_Variant       userdata
        );

        /**
         * @fn int sim_cmdlnopt_handle_options(5)
         * @headerfile cmdlnopt.h "simsoft/cmdlnopt.h"
         * @brief Handles POSIX-esque command line options.
         * 
         * @param[in]     arguments_count_ptr  Pointer to @e argc.
         * @param[in]     arguments_array_ptr  Pointer to @e argv.
         * @param[in]     option_handler_count The number of options in @e option_handler_array.
         * @param[in]     option_handler_array Array of option handlers.
         * @param[in]     error_proc           Callback for if an error occurs.
         * @param[in,out] userdata             Variant to provide to the option handlers.
         * 
         * @returns @c EXIT_SUCCESS if none of the option handlers set it to a different value;
         *          see remarks.
         * 
         * @remarks @e error_proc can be set to @c NULL to invoke a default error handler. This
         *          error handler prints what went wrong to standard output, stops handling any
         *          further arguments, and makes this function return @c EXIT_FAILURE.
         * 
         * @remarks sim_return_code() is set to one of the following:
         *     @b SIM_RC_ERR_NULLPTR if @e arguments_count_ptr, @e arguments_array_ptr, or
         *                           @e option_handler_array are @c NULL;
         *     @b SIM_RC_FAILURE     if the function doesn't return @c EXIT_SUCCESS.
         *     @b SIM_RC_SUCCESS     otherwise.
         * 
         * @sa Sim_CmdLnOptHandler
         * @sa Sim_CmdLnOptErrorProc
         */
        extern EXPORT int C_CALL sim_cmdlnopt_handle_options(
            int *const                arguments_count_ptr,
            char** *const             arguments_array_ptr,
            size_t                    option_handler_count,
            const Sim_CmdLnOptHandler option_handler_array[],
            Sim_CmdLnOptErrorProc     error_proc,
            Sim_Variant               userdata
        );

    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
        
        //

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_CMDLNOPT_H_ */