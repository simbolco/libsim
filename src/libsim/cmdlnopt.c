/**
 * @file cmdlnopt.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/cmdlnopt.h
 * @version 0.1
 * @date 2020-03-16
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_CMDLNOPT_C_
#define SIMSOFT_CMDLNOPT_C_

#include "simsoft/cmdlnopt.h"
#include "simsoft/utf8.h"
#include "./_internal.h"

struct _Sim_CmdLnOptArgs {
    int *const argc_ptr;
    char** *const argv_ptr;
    const char* program_name;
};

// deals with handling a given command line option + gets next argstring if necessary
static bool _sim_cmdlnopt_handle_option(
    int *const                       arguments_count_ptr,
    char** *const                    arguments_array_ptr,
    const Sim_CmdLnOptHandler *const option_handler_ptr,
    char*                            option_argument_string,
    int *const                       exit_code_out_ptr,
    Sim_CmdLnOptErrorProc            error_proc,
    Sim_CmdLnOptArgs *const          args_state_ptr,
    Sim_Variant                      userdata
) {
    // get argument from next argstring if an argument is required/optional and no argument was
    // provided in the same argstring as the option
    if (
        option_handler_ptr->has_argument &&
        !option_argument_string &&
        *arguments_count_ptr > 1 &&
        (
            (*arguments_array_ptr)[1][0] != '-' ||
            !strcmp((*arguments_array_ptr)[1], "-")
        )
    ) {
        option_argument_string = (*arguments_array_ptr)[1];
        (*arguments_count_ptr)--;
        (*arguments_array_ptr)++;
    }
    
    // error out if no argument was provided to an option that requires one.
    if (
        option_handler_ptr->has_argument == SIM_CMDLNOPT_REQUIRED_ARGUMENT &&
        !option_argument_string
    ) {
        char short_option_buf[5] = { 0 };
        sim_utf8_from_codepoint(option_handler_ptr->short_codepoint, short_option_buf);
        return error_proc(
            args_state_ptr->program_name,
            option_handler_ptr->long_name ? option_handler_ptr->long_name : short_option_buf,
            SIM_CMDLNOPT_ERR_MISSING_ARGUMENT,
            exit_code_out_ptr,
            userdata
        );
    }
    
    // set option_argument to NULL if option explicitly says no argument is needed.
    if (option_handler_ptr->has_argument == SIM_CMDLNOPT_NO_ARGUMENT)
        option_argument_string = NULL;
    
    return option_handler_ptr->handler_proc(
        args_state_ptr,
        option_handler_ptr->long_name,
        option_handler_ptr->short_codepoint,
        option_argument_string,
        exit_code_out_ptr,
        userdata
    );
}

// the default error handler
static bool _sim_cmdlnopt_handle_error(
    const char*       program_string,
    const char*       option_string,
    Sim_CmdLnOptError error_type,
    int *const        exit_code_out_ptr,
    Sim_Variant       userdata
) {
    (void)userdata;

    // unknown option provided
    switch (error_type) {
    case SIM_CMDLNOPT_ERR_UNKNOWN_OPTION:
        printf("%s: unknown option `%s'\n", program_string, option_string);
        break;
        
    case SIM_CMDLNOPT_ERR_NONFLAG_OPTION:
        printf(
            "%s: option `%s' requires arguments; cannot be combined with other flag options\n",
            program_string, option_string
        );
        break;
        
    case SIM_CMDLNOPT_ERR_MISSING_ARGUMENT:
        printf("%s: option `%s' missing required argument\n", program_string, option_string);
        break;
    }
    
    *exit_code_out_ptr = EXIT_FAILURE;
    return true;
}

// sim_cmdlnopt_next_argstring(1): Retrieves the next argstring from the given args state.
const char* sim_cmdlnopt_next_argstring(Sim_CmdLnOptArgs *const args_state_ptr) {
    RETURN_IF(!args_state_ptr, SIM_RC_ERR_NULLPTR, NULL);
    
    if (
        (*args_state_ptr->argc_ptr) > 1 &&
        (
            (*args_state_ptr->argv_ptr)[1][0] != '-' ||
            !strcmp((*args_state_ptr->argv_ptr)[1], "-")
        )
    ) {
        (*args_state_ptr->argc_ptr)--;
        (*args_state_ptr->argv_ptr)++;
        RETURN(SIM_RC_SUCCESS, (*args_state_ptr->argv_ptr)[0]);
    }
    
    RETURN(SIM_RC_SUCCESS, NULL);
}

// sim_cmdlnopt_get_program_name(1): Retrieves the program name as entered from the command line.
const char* sim_cmdlnopt_get_program_name(Sim_CmdLnOptArgs *const args_state_ptr) {
    RETURN_IF(!args_state_ptr, SIM_RC_ERR_NULLPTR, NULL);
    RETURN(SIM_RC_SUCCESS, args_state_ptr->program_name);
}

// sim_cmdlnopt_handle_options(5): Handles POSIX-esque command line options.
int sim_cmdlnopt_handle_options(
    int *const                arguments_count_ptr,
    char** *const             arguments_array_ptr,
    size_t                    option_handler_count,
    const Sim_CmdLnOptHandler option_handler_array[],
    Sim_CmdLnOptErrorProc     error_proc,
    Sim_Variant               userdata
) {
    RETURN_IF(!arguments_array_ptr || !arguments_count_ptr || !option_handler_array,
        SIM_RC_ERR_NULLPTR, 0
    );
    
    // use default error handler if none is provided by the user
    if (!error_proc)
        error_proc = _sim_cmdlnopt_handle_error;
    
    int exit_code = EXIT_SUCCESS;
    bool break_flag = false;
    
    char* option_argument = NULL;
    
    size_t codepoint_size = 0;
    uint32 codepoint = 0;
    
    const char* program_name = (*arguments_array_ptr)[0];
    Sim_CmdLnOptArgs args_state = {
        .argc_ptr = arguments_count_ptr,
        .argv_ptr = arguments_array_ptr,
        .program_name = program_name
    };
    
    do {
        (*arguments_count_ptr)--;
        (*arguments_array_ptr)++;
        
        if (*arguments_count_ptr == 0)
            break;
        
        // check if the given argument is an option
        switch ((*arguments_array_ptr)[0][0]) {
        case '-':
            switch ((*arguments_array_ptr)[0][1]) {
            // handle long option (i.e. "--<LONG_OPTION_NAME>=<ARGUMENT>")
            case '-':
                // "--" signals end of command line options; skip and break
                if (!(*arguments_array_ptr)[0][2]) {
                    break_flag = true;
                    (*arguments_count_ptr)--;
                    (*arguments_array_ptr)++;
                    break;
                }
                
                // split based on '='
                option_argument = strchr(&((*arguments_array_ptr)[0][2]), '=');
                
                // split the option & argument with a null-terminator
                if (option_argument) {
                    *option_argument = '\0';
                    option_argument++;
                }
                
                // serach for long option in option_handler_array
                {
                    size_t i;
                    for (i = 0; i < option_handler_count; i++) {
                        // found the option in the list; now handle it!
                        if (
                            option_handler_array[i].long_name && 
                            !strcmp(
                                &((*arguments_array_ptr)[0][2]),
                                option_handler_array[i].long_name
                            )
                        ) {
                            break_flag = _sim_cmdlnopt_handle_option(
                                arguments_count_ptr,
                                arguments_array_ptr, 
                                &option_handler_array[i],
                                option_argument,
                                &exit_code,
                                error_proc,
                                &args_state,
                                userdata
                            );
                            break;
                        }
                    }
                    
                    // error out: unknown option
                    if (i == option_handler_count)
                        break_flag = error_proc(
                            program_name,
                            &((*arguments_array_ptr)[0][2]),
                            SIM_CMDLNOPT_ERR_UNKNOWN_OPTION,
                            &exit_code,
                            userdata
                        );
                }
                break;
            
            // "-" is a valid argument, not an option -> finished parsing options; break
            case '\0':
                break_flag = true;
                break;
            
            // handle short option (i.e. "-<SHORT_OPTION_CODEPOINT><ARGUMENT>")
            default:
                codepoint_size = sim_utf8_get_char_size(&((*arguments_array_ptr)[0][1]));
                if ((*arguments_array_ptr)[0][1])
                    option_argument = &((*arguments_array_ptr)[0][1 + codepoint_size]);
                else
                    option_argument = NULL;
                
                // set option_argument to NULL if end of argstring
                if (*option_argument == '\0')
                    option_argument = NULL;
                
                // convert option to UTF codepoint
                codepoint = sim_utf8_to_codepoint(&((*arguments_array_ptr)[0][1]));
                
                // search for short option in option_handler_array
                {
                    size_t i;
                    for (i = 0; i < option_handler_count; i++) {
                        // found the option in the list; now handle it!
                        if (codepoint == option_handler_array[i].short_codepoint) {
                            break_flag = _sim_cmdlnopt_handle_option(
                                arguments_count_ptr,
                                arguments_array_ptr, 
                                &option_handler_array[i],
                                option_argument,
                                &exit_code,
                                error_proc,
                                &args_state,
                                userdata
                            );
                            break;
                        }
                    }
                    
                    // error out: unknown option
                    if (i == option_handler_count)
                        break_flag = error_proc(
                            program_name,
                            &((*arguments_array_ptr)[0][1]),
                            SIM_CMDLNOPT_ERR_UNKNOWN_OPTION,
                            &exit_code,
                            userdata
                        );
                    // continue iterating through the UTF-8 chars in argstring as long as they all
                    // explicitly require no args
                    else if (
                        !break_flag &&
                        option_handler_array[i].has_argument == SIM_CMDLNOPT_NO_ARGUMENT
                        && option_argument
                    )
                        do {
                            codepoint = sim_utf8_to_codepoint(option_argument);
                            
                            // search for short option in option_handler_array
                            for (i = 0; i < option_handler_count; i++) {
                                // found the option in the list; now handle it!
                                if (codepoint == option_handler_array[i].short_codepoint) {
                                    // error out if option doesn't explicitly take no arguments
                                    if (
                                        option_handler_array[i].has_argument !=
                                            SIM_CMDLNOPT_NO_ARGUMENT
                                    ) {
                                        break_flag = error_proc(
                                            program_name,
                                            option_argument,
                                            SIM_CMDLNOPT_ERR_NONFLAG_OPTION,
                                            &exit_code,
                                            userdata
                                        );
                                        break;
                                    }
                                    
                                    break_flag = _sim_cmdlnopt_handle_option(
                                        arguments_count_ptr,
                                        arguments_array_ptr, 
                                        &option_handler_array[i],
                                        NULL,
                                        &exit_code,
                                        error_proc,
                                        &args_state,
                                        userdata
                                    );
                                    break;
                                }
                            }
                            
                            // error out: unknown option
                            if (i == option_handler_count)
                                break_flag = error_proc(
                                    program_name,
                                    option_argument,
                                    SIM_CMDLNOPT_ERR_UNKNOWN_OPTION,
                                    &exit_code,
                                    userdata
                                );
                            
                            option_argument = sim_utf8_next_char(option_argument);
                        } while (*option_argument && !break_flag);
                }
                
                break;
            }
            break;
        
        // not an option -> finished parsing options; break
        default:
            break_flag = true;
            break;
        }
    } while ((*arguments_count_ptr) > 0 && !break_flag);
    
    RETURN((exit_code == EXIT_SUCCESS) ? SIM_RC_SUCCESS : SIM_RC_FAILURE, exit_code);
}

#endif /* SIMSOFT_CMDLNOPT_C_ */
