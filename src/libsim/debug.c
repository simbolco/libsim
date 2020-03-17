/**
 * @file debug.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation for simsoft/debug.h
 * @version 0.1
 * @date 2020-01-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SIMSOFT_DEBUG_C_
#define SIMSOFT_DEBUG_C_

#include "simsoft/debug.h"
#include "./_internal.h"

#ifdef __WIN32__
#   include <dbghelp.h>
#elif defined(__GLIBC__)
#   include <execinfo.h>
#endif

// sim_get_return_code_string(1): Convert a return code to a human-readable string.
const char* sim_get_return_code_string(
    Sim_ReturnCode return_code
) {
    static const char* RETURN_CODES[] = {
        "Success",
        "Failure",
        "Null pointer error",
        "Out of memory",
        "Zero division error",
        "Invalid argument",
        "Out of bounds error",
        "Unsupported operation",
        "Item not found in container",
        "Unimplemented operation"
    };

    return (
        return_code < 0 ||
        return_code >= (sizeof(RETURN_CODES)/sizeof(const char*))
    ) ?
        NULL :
        RETURN_CODES[return_code]
    ;
}

// sim_get_backtrace_info(3): Retrieve stack backtrace information.
size_t sim_get_backtrace_info(
    Sim_BacktraceInfo backtrace_array[],
    size_t            backtrace_size,
    size_t            skip_frames
) {
    // check for nullptr
    RETURN_IF(!backtrace_array, SIM_RC_ERR_NULLPTR, 0);

#   ifdef _WIN32
        // get process/thread handles
        HANDLE
            process_handle = GetCurrentProcess(),
            thread_handle  = GetCurrentThread()
        ;

        // get thread context
        CONTEXT context;
        ZeroMemory(&context, sizeof(CONTEXT));
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        // set up & initialize symbol handler
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
        if (!SymInitialize(process_handle, NULL, true))
        {
            _sim_win32_print_last_error("SymInitialize(%p, %p, true)", process_handle, NULL);
            RETURN(SIM_RC_FAILURE, 0);
        }

        // prepare stack frame struct
        DWORD machine_type;
        STACKFRAME64 stack_frame;
        ZeroMemory(&stack_frame, sizeof(STACKFRAME64));

#       ifdef _M_IX86
            machine_type = IMAGE_FILE_MACHINE_I386;
            stack_frame.AddrPC.Offset = context.Eip;
            stack_frame.AddrPC.Mode = AddrModeFlat;
            stack_frame.AddrFrame.Offset = context.Ebp;
            stack_frame.AddrFrame.Mode = AddrModeFlat;
            stack_frame.AddrStack.Offset = context.Esp;
            stack_frame.AddrStack.Mode = AddrModeFlat;
#       elif  _M_X64
            machine_type = IMAGE_FILE_MACHINE_AMD64;
            stack_frame.AddrPC.Offset = context.Rip;
            stack_frame.AddrPC.Mode = AddrModeFlat;
            stack_frame.AddrFrame.Offset = context.Rsp;
            stack_frame.AddrFrame.Mode = AddrModeFlat;
            stack_frame.AddrStack.Offset = context.Rsp;
            stack_frame.AddrStack.Mode = AddrModeFlat;
#       elif  _M_IA64
            machine_type = IMAGE_FILE_MACHINE_IA64;
            stack_frame.AddrPC.Offset = context.StIIP;
            stack_frame.AddrPC.Mode = AddrModeFlat;
            stack_frame.AddrFrame.Offset = context.IntSp;
            stack_frame.AddrFrame.Mode = AddrModeFlat;
            stack_frame.AddrBStore.Offset = context.RsBSP;
            stack_frame.AddrBStore.Mode = AddrModeFlat;
            stack_frame.AddrStack.Offset = context.IntSp;
            stack_frame.AddrStack.Mode = AddrModeFlat;
#       endif

        // prepare symbol info struct
        char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol_info_ptr = (PSYMBOL_INFO)symbol_buffer;
        symbol_info_ptr->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol_info_ptr->MaxNameLen = MAX_SYM_NAME;

        // prepare line info struct
        IMAGEHLP_LINE64 line_info;
        line_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        // iterate through stack trace
        size_t index = 0;
        for (size_t i = 0; i < backtrace_size; i++) {
            // walk stack until end
            if (!StackWalk64(
                machine_type,
                process_handle,
                thread_handle,
                &stack_frame,
                &context,
                NULL,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                NULL
            ))
                break;
            
            // start filling backtrace_array after (skip_frames) stack frames...
            if (i >= skip_frames) {
                backtrace_array[index].function_address =
                    (void*)(uintptr_t)stack_frame.AddrPC.Offset
                ;

                // get symbol from address
                if (SymFromAddr(
                    process_handle,
                    (DWORD64)backtrace_array[index].function_address,
                    NULL,
                    symbol_info_ptr
                )) {
                    backtrace_array[index].function_name = strdup(symbol_info_ptr->Name);
                    if (!backtrace_array[index].function_name) {
                        for (ssize_t j = (ssize_t)index; j >= 0; j--) {
                            free(backtrace_array[index].function_name);
                            backtrace_array[index].function_address = NULL;
                        }
                        RETURN(SIM_RC_ERR_OUTOFMEM, index - 1);
                    }
                    
                    // get line/file info from address
                    if (SymGetLineFromAddr64(
                        process_handle,
                        (DWORD64)backtrace_array[index].function_address,
                        NULL,
                        &line_info
                    )) {
                        backtrace_array[index].line_number = line_info.LineNumber;

                        backtrace_array[index].file_name = strdup(line_info.FileName);
                        if (!backtrace_array[index].file_name) {
                            for (ssize_t j = (ssize_t)index; j >= 0; j--) {
                                free(backtrace_array[index].file_name);
                                backtrace_array[index].file_name = NULL;
                                free(backtrace_array[index].function_name);
                                backtrace_array[index].function_address = NULL;
                            }
                            RETURN(SIM_RC_ERR_OUTOFMEM, index - 1);
                        }
                    } else {
                        _sim_win32_print_last_error(
                            "SymGetLineFromAddr64(%p, %p, %p, %p)",
                            process_handle,
                            backtrace_array[index].function_address,
                            NULL,
                            &line_info
                        );
                        backtrace_array[index].file_name = NULL;
                        backtrace_array[index].line_number = 0;
                    }
                } else {
                    _sim_win32_print_last_error(
                        "SymFromAddr(%p, %p, %p, %p)",
                        process_handle,
                        backtrace_array[index].function_address,
                        NULL,
                        symbol_info_ptr
                    );
                    backtrace_array[index].function_name = NULL;
                    backtrace_array[index].file_name = NULL;
                    backtrace_array[index].line_number = 0;
                }

                index++;
            }
        }

        // cleanup symbol handler
        if (!SymCleanup(process_handle)) {
            _sim_win32_print_last_error("SymCleanup(%p)", process_handle);
        }
        
        RETURN(SIM_RC_SUCCESS, index);

#   elif defined(__GLIBC__)
        // get backtrace addresses
        void* backtrace_ptr_array[backtrace_size];
        int num_entries = backtrace(backtrace_ptr_array, (int)(backtrace_size + skip_frames));
        
        // get backtrace symbols
        char* backtrace_symbol_array = backtrace_symbols(
            backtrace_ptr_array,
            num_entries
        );

        int i;
        for (i = skip_frames; i < num_entries; i++) {
            backtrace_array[i - skip_frames].function_address = backtrace_ptr_array[i];
            backtrace_array[i - skip_frames].line_number = 0;
            backtrace_array[i - skip_frames].file_name = NULL;

            // get function name
            if (backtrace_symbol_array && backtrace_symbol_array[i]) {
                backtrace_array[i - skip_frames].function_name = strdup(backtrace_symbol_array[i]);
                if (!backtrace_array[i - skip_frames].function_name) {
                    for (int j = i; j >= 0; j--) {
                        free(backtrace_array[j].function_name);
                        backtrace_array[j].function_address = NULL;
                    }
                    
                    RETURN(SIM_RC_ERR_OUTOFMEM, 0);
                }
            } else
                backtrace_array[i - skip_frames].function_name = NULL;
        }

        free(backtrace_symbol_array);
        RETURN(SIM_RC_SUCCESS, i);

#   else
#       warning("sim_get_backtrace_info(3) is unsupported")
        (void)backtrace_array; (void)backtrace_size; (void)skip_frames;
        RETURN(SIM_RC_ERR_UNSUPRTD, 0);
#   endif
}

#endif /* SIMSOFT_DEBUG_C_ */
