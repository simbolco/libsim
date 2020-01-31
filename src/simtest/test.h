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
#include <signal.h>
#include "simsoft/debug.h"
#ifdef _WIN32
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>

#   ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#       define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x004
#   endif

    void _win32_ansi_setup() {
        HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD console_mode = 0;
        GetConsoleMode(stdout_handle, &console_mode);
        console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(stdout_handle, console_mode);
    }
#endif

#define ERR_STR(str) "\33[1;91mError: \33[0;31m" str "\33[0m\n"

static void sig_catch(int signal_number) {
    if (signal_number == SIGINT) {
        printf("^C");
        exit(2);
    }
}

typedef struct SimT_TestInfo {
    int total;
    int passed;
    int failed;
    int remaining;
} SimT_TestInfo;

typedef Sim_ReturnCode (*SimT_TestFuncPtr)(SimT_TestInfo *const);

typedef struct SimTestStruct {
    SimT_TestFuncPtr func_ptr;
    const char*      name;
    const char*      description;
    size_t           num_args;
    size_t           num_opt_args;
} SimTestStruct;

#endif /* SIMTEST_TEST_H_ */
