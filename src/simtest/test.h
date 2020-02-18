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
#include "simsoft/dynlib.h"
#include "simsoft/hashmap.h"

#define ERR_STR(str) "\33[1;91mError: \33[0;31m" str "\33[0m\n"

extern void*  simt_malloc(size_t size);
extern void*  simt_falloc(size_t size, ubyte fill);
extern void*  simt_realloc(void* ptr, size_t size);
extern void   simt_free(void* ptr);
extern size_t simt_alloc_size(void);
extern void   simt_alloc_set_lock(bool lock);

// =================================================================================================

typedef Sim_ReturnCode (*SimT_TestProc)(const char* *const);

typedef struct SimT_TestProcStruct {
    SimT_TestProc proc;
    const char*   name;
} SimT_TestProcStruct;

typedef struct SimTestStruct {
    const char*          name;
    const char*          description;
    int                  num_tests;
    SimT_TestProcStruct* test_procs;
} SimTestStruct;

#endif /* SIMTEST_TEST_H_ */
