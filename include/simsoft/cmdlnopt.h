/**
 * @file cmdlnopt.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for command line option handling.
 * @version 0.2
 * @date 2020-07-26
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_CMDLNOPT_H_
#define SIMSOFT_CMDLNOPT_H_

#include "macro.h"
#include "typedefs.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START // start C API

        /**
         * @brief C enumeration of command line option argument specifications.
         */
        typedef enum Sim_CmdLnOptHasArgument {
            SIM_OPT_NO_ARGUMENT = 0,       ///< This option explicitly takes no arguments.
            SIM_OPT_REQUIRED_ARGUMENT = 1, ///< This optoin explicitly requires at least 1 argument.
            SIM_OPT_OPTIONAL_ARGUMENT = 2  ///< This option may or may not take arguments.
        } Sim_CmdLnOptHasArgument;

        /**
         * @brief C enumeration of errors that can occur whilst handling command line options.
         */
        typedef enum Sim_CmdLnOptError {
            SIM_OPT_ERR_UNKNOWN_OPTION = 0,   ///< An unknown option was provided.
            SIM_OPT_ERR_MISSING_ARGUMENT = 1, ///< The given option was not provided an argument.
            SIM_OPT_ERR_NONFLAG_OPTION = 2    ///< The given option that requires arguments was
                                              ///< included in a string of options that explicitly
                                              ///< take no arguments.
        } Sim_CmdLnOptError;

        /**
         * @brief Opaque structure used by option handler callbacks.
         */
        typedef struct _Sim_CmdLnOptArgs Sim_CmdLnOptArgs;

        /**
         * @brief Function pointer for a command line option handler callback.
         *  
         * @param[in]     args_state_ptr    Opaque pointer used to retrieve extra argstrings.
         * @param[in]     option_string     The UTF codepoint of the option being handled.
         * @param[in]     argument_string   The first argstring passed to the option.
         * @param[out]    exit_code_out_ptr Pointer to the exit code.
         * @param[in,out] data              User-provided callback data.
         * 
         * @returns @c false to continue handling command line options;
         *          @c true  to break and stop handling any further command line options.
         * 
         * @sa Sim_CmdLnOptStruct
         */
        typedef bool (*Sim_CmdLnOptHandlerProc)(
            Sim_CmdLnOptArgs *const args_state_ptr,
            const char*             option_string,
            const char*             argument_string,
            int *const              exit_code_out_ptr,
            Sim_Variant             userdata
        );

        #define SIM_CMDLNOPT_ARGS                       \
            Sim_CmdLnOptArgs *const args_state_ptr,     \
            const char*             option_string,      \
            const char*             argument_string,    \
            int *const              exit_code_out_ptr,  \
            Sim_Variant             userdata

        /**
         * @brief Structure defining a command line option handler.
         * 
         * @sa Sim_CmdLnOptHasArgument
         * @sa Sim_CmdLnOptHandlerProc
         */
        typedef struct Sim_CmdLnOptHandler {
            const char*             long_name;       ///< The long name of this option.
            uint32                  short_codepoint; ///< The short name of this option.
            Sim_CmdLnOptHasArgument has_argument;    ///< Whether this option takes arguments.
            Sim_CmdLnOptHandlerProc handler_proc;    ///< Option handler callback function.
        } Sim_CmdLnOptHandler;

        /**
         * @brief Retrieves the next argstring from the given args state.
         * @details A command line option handler callback function is provided with an
         * @e args_state_ptr specifically for use in this function.
         * 
         * @param[in] args_state_ptr Opaque pointer provided to option handler callbacks.
         * 
         * @returns The next argstring in the @e argv provided to @e sim_cmdlnopt_handle_options;
         *          @c NULL if no more argstrings are available or on error (see remarks).
         * 
         * @throw SIM_ERR_NULLPTR if @e args_state_ptr is @c NULL.
         * 
         * @sa Sim_CmdLnOptArgs
         */
        DYNAPI_PROC(const char*, sim_cmdlnopt_next_argstring,,
            Sim_CmdLnOptArgs *const args_state_ptr
        );

        /**
         * @brief Retrieves the program name as entered from the command line.
         * @details A command line option handler callback function is provided with an
         *          @e args_state_ptr specifically for use in this function.
         * 
         * @param[in] args_state_ptr Opaque pointer provided to option handler callbacks.
         * 
         * @returns The program name as entered in the command line; @c NULL on error (see remarks).
         * 
         * @throw SIM_ERR_NULLPTR if @e args_state_ptr is @c NULL.
         * 
         * @sa Sim_CmdLnOptArgs
         */
        DYNAPI_PROC(const char*, sim_cmdlnopt_get_program_name,,
            Sim_CmdLnOptArgs *const args_state_ptr
        );

        /**
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
         * @brief Handles POSIX-esque command line options.
         * 
         * @param[in]     arguments_count_ptr  Pointer to @e argc.
         * @param[in]     arguments_array_ptr  Pointer to @e argv.
         * @param[in]     option_handler_count The number of options in @e option_handler_array.
         * @param[in]     option_handler_array Array of option handlers.
         * @param[in]     error_proc           Callback for if an error occurs.
         * @param[in,out] userdata             Variant to provide to the option handlers.
         * 
         * @returns @c EXIT_SUCCESS if none of the option handlers set it to a different value.
         * 
         * @throw SIM_ERR_NULLPTR if @e arguments_count_ptr, @e arguments_array_ptr, or
         *                        @e option_handler_array are @c NULL.
         * 
         * @remarks @e error_proc can be set to @c NULL to invoke a default error handler. This
         *          error handler prints what went wrong to standard output, stops handling any
         *          further arguments, and makes this function return @c EXIT_FAILURE.
         * 
         * @sa Sim_CmdLnOptHandler
         * @sa Sim_CmdLnOptErrorProc
         */
        DYNAPI_PROC(int, sim_cmdlnopt_handle_options,,
            int *const                arguments_count_ptr,
            char** *const             arguments_array_ptr,
            size_t                    option_handler_count,
            const Sim_CmdLnOptHandler option_handler_array[],
            Sim_CmdLnOptErrorProc     error_proc,
            Sim_Variant               userdata
        );

    CPP_NAMESPACE_C_API_END // end C API

#   ifdef __cplusplus       // start C++ API
        
        //

#   endif // end C++ API
CPP_NAMESPACE_END(SimSoft)

#endif // SIMSOFT_CMDLNOPT_H_