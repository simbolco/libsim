/**
 * @file test.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Test struct & other predefinitions
 * @version 0.1
 * @date 2020-01-29
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */
#ifndef SIMTEST_TEST_H_
#define SIMTEST_TEST_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "simsoft/allocator.h"
#include "simsoft/debug.h"

#ifdef _WIN32
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>

#   ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#       define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x004
#   endif
#   ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#       define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#   endif

#   ifdef DEBUG
        DWORD _win32_print_last_error(const char* err_format_str, ...) {
            DWORD err = GetLastError();

            CHAR err_msg_buffer[512];
            FormatMessageA(
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                err_msg_buffer,
                512,
                NULL
            );

            {
                va_list args;
                va_start(args, err_format_str);
                vfprintf(stderr, err_format_str, args);
                va_end(args);
            }

            fprintf(stderr, " returned error %ld: %s\n", err, err_msg_buffer);
            return err;
        }
#   else
#       define _win32_print_last_error(...) {}
#   endif

    void _win32_ansi_setup() {
        if (!SetConsoleOutputCP(65001))
            _win32_print_last_error("SetConsoleOutputCP(65001)");

        HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (stdout_handle == INVALID_HANDLE_VALUE) {
            _win32_print_last_error("GetStdHandle(STD_OUTPUT_HANDLE)");
            return;
        }

        DWORD console_mode = 0;
        if (!GetConsoleMode(stdout_handle, &console_mode)) {
            _win32_print_last_error(
                "GetConsoleMode(stdout_handle <%p>, &console_mode)",
                (void*)stdout_handle
            );
            return;
        }
        console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(stdout_handle, console_mode)) {
            _win32_print_last_error(
                "SetConsoleMode(stdout_handle <%p>, %lu)",
                (void*)stdout_handle,
                console_mode
            );
        }
    }
#endif

#define ERR_STR(str) "\33[1;91mError: \33[0;31m" str "\33[0m\n"

static void sig_catch(int signal_number) {
    switch (signal_number) {
    case SIGINT:
        printf("^C");
        exit(2);
        break;

    case SIGSEGV: {
        printf("SEGMENTATION FAULT!\n");

        Sim_BacktraceInfo backtrace_info[32];
        memset(backtrace_info, 0, sizeof(backtrace_info));
        size_t num_frames;
        
        if (!sim_get_backtrace_info(backtrace_info, 32, 0, &num_frames)) {
            printf("Backtrace:\n");
            for (size_t i = 0; i < num_frames; i++) {
                printf(" %p - %s%s%s%s\n",
                    backtrace_info[i].function_address,
                    backtrace_info[i].function_name ?
                        backtrace_info[i].function_name :
                        "????"
                    ,
                    backtrace_info[i].file_name ? " (" : "",
                    backtrace_info[i].file_name ?
                        backtrace_info[i].file_name :
                        ""
                    ,
                    backtrace_info[i].file_name ? ")" : ""
                );
            }
        }
        
        exit(-1);
        break;
    }

    default:
        break;
    }
}

#define SIMT_ALLOCED_PTRS_MAX_SIZE 1024
static void* simt_alloced_ptrs[SIMT_ALLOCED_PTRS_MAX_SIZE];
static size_t simt_alloced_ptrs_size = 0;

static void* simt_malloc(size_t size) {
    if (simt_alloced_ptrs_size == SIMT_ALLOCED_PTRS_MAX_SIZE - 1)
        return NULL;

    void* ptr = sim_allocator_default_malloc(size);
    if (!ptr)
        return NULL;

    simt_alloced_ptrs[simt_alloced_ptrs_size++] = ptr;
    return ptr;
}

static void* simt_falloc(size_t size, ubyte fill) {
    if (simt_alloced_ptrs_size == SIMT_ALLOCED_PTRS_MAX_SIZE - 1)
        return NULL;

    void* ptr = sim_allocator_default_falloc(size, fill);
    if (!ptr)
        return NULL;

    simt_alloced_ptrs[simt_alloced_ptrs_size++] = ptr;
    return ptr;
}

static void* simt_realloc(void* ptr, size_t size) {
    void* new_ptr = sim_allocator_default_realloc(ptr, size);
    if (!new_ptr)
        return NULL;

    for (size_t i = 0; i < simt_alloced_ptrs_size; i++) {
        if (ptr == simt_alloced_ptrs[i]) {
            simt_alloced_ptrs[i] = new_ptr;
            break;
        }
    }

    return new_ptr;
}

static void simt_free(void* ptr) {
    if (ptr) {
        for (size_t i = 0; i < simt_alloced_ptrs_size; i++) {
            if (ptr == simt_alloced_ptrs[i]) {
                memcpy(
                    &simt_alloced_ptrs[i],
                    &simt_alloced_ptrs[i + 1],
                    (simt_alloced_ptrs_size - i) * sizeof(void*)
                );
                break;
            }
        }
        sim_allocator_default_free(ptr);
        simt_alloced_ptrs_size--;
    }
}

static Sim_Allocator simt_allocator = {
    simt_malloc, simt_falloc, simt_realloc, simt_free
};

static void simt_atexit_free_all(void) {
    for (size_t i = 0; i < simt_alloced_ptrs_size; i++)
        sim_allocator_default_free(simt_alloced_ptrs[i]);
}


typedef Sim_ReturnCode (*SimT_TestFuncPtr)(const char* *const);

typedef struct SimT_TestFuncStruct {
    SimT_TestFuncPtr func_ptr;
    const char*      name;
} SimT_TestFuncStruct;

typedef struct SimTestStruct {
    const char*          name;
    const char*          description;
    int                  num_tests;
    SimT_TestFuncStruct* test_funcs;
} SimTestStruct;




static bool _int_eq(const int *const a, const int *const b) {
    return *a == *b;
}

#endif /* SIMTEST_TEST_H_ */
