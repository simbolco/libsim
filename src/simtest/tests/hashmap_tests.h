/**
 * @file hashmap_tests.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Hashmap unit tests.
 * @version 0.1
 * @date 2020-02-01
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */
#ifndef SIMTEST_HASHMAP_TESTS_H_
#define SIMTEST_HASHMAP_TESTS_H_

#include "./test.h"
#include "simsoft/hashmap.h"

Sim_HashMap hashmap;



SimT_TestFuncStruct hashmap_tests[] = {
    { hashmap_test_init, "initialization" },
    { hashmap_test_destroy, "destructor" }
};

#endif /* SIMTEST_HASHMAP_TESTS_H_ */
