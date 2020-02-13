/**
 * @file hashset_tests.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Hashset unit tests.
 * @version 0.1
 * @date 2020-02-01
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */
#ifndef SIMTEST_HASHSET_TESTS_H_
#define SIMTEST_HASHSET_TESTS_H_

#include "simsoft/common.h"

extern Sim_ReturnCode hashset_test_construct(const char* *const out_err_str);
extern Sim_ReturnCode hashset_test_insert(const char* *const out_err_str);
extern Sim_ReturnCode hashset_test_remove(const char* *const out_err_str);
extern Sim_ReturnCode hashset_test_destroy(const char* *const out_err_str);

#endif /* SIMTEST_HASHSET_TESTS_H_ */
