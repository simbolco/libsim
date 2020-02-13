/**
 * @file vector_tests.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Vector unit tests.
 * @version 0.1
 * @date 2020-01-29
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */
#ifndef SIMTEST_VECTOR_TESTS_H_
#define SIMTEST_VECTOR_TESTS_H_

#include "simsoft/common.h"

extern Sim_ReturnCode vector_test_construct(const char* *const out_err_str);
extern Sim_ReturnCode vector_test_push(const char* *const out_err_str);

extern Sim_ReturnCode vector_test_get(const char* *const out_err_str);

extern Sim_ReturnCode vector_test_contains(const char* *const out_err_str);
extern Sim_ReturnCode vector_test_remove(const char* *const out_err_str);
extern Sim_ReturnCode vector_test_clear(const char* *const out_err_str);
extern Sim_ReturnCode vector_test_destroy(const char* *const out_err_str);

#endif /* SIMTEST_VECTOR_TEST_H_ */
