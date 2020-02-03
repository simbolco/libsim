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

#include "../test.h"
#include "simsoft/hashset.h"

Sim_HashSet hashset;

Sim_ReturnCode hashset_test_init(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    srand(time(NULL));

    if ((rc = sim_hashset_initialize(
        NULL,
        sizeof(void*),
        SIM_DATATYPE_OTHER,
        NULL,
        (Sim_PredicateFuncPtr)_int_eq,
        NULL,
        75
    )) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "initialize: failed to check for NULLPTR hashset";
        return SIM_RC_ERR_NULLPTR;
    }
    if ((rc = sim_hashset_initialize(
        &hashset,
        sizeof(void*),
        SIM_DATATYPE_OTHER,
        NULL,
        NULL,
        NULL,
        75
    )) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "initialize: failed to check for NULLPTR predicate function";
        return SIM_RC_ERR_NULLPTR;
    }

    if ((rc = sim_hashset_initialize(
        &hashset,
        sizeof(int),
        SIM_DATATYPE_INTEGRAL,
        NULL,
        (Sim_PredicateFuncPtr)_int_eq,
        NULL,
        75
    ))) {
        *out_err_str = "unexpected error out on initialize";
        return rc;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode hashset_test_insert(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    int i = 0;

    if ((rc = sim_hashset_insert(NULL, &i)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "insert: failed to check for NULLPTR hashset";
        return SIM_RC_FAILURE;
    }
    if ((rc = sim_hashset_insert(&hashset, NULL)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "insert: failed to check for NULLPTR item";
        return SIM_RC_FAILURE;
    }
    if ((rc = sim_hashset_contains(NULL, &i)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "contains: failed to check for NULLPTR hashset";
        return SIM_RC_FAILURE;
    }
    if ((rc = sim_hashset_contains(&hashset, NULL)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "contains: failed to check for NULLPTR item";
        return SIM_RC_FAILURE;
    }

    for (; i < 128; i++) {
        if ((rc = sim_hashset_insert(&hashset, &i))) {
            sim_hashset_destroy(&hashset);
            *out_err_str = "unexpected error out on insert";
            return rc;
        }
        if (hashset.count != (size_t)(i + 1)) {
            *out_err_str = "insert: failed to increment count property";
            return SIM_RC_FAILURE;
        }

        if ((rc = sim_hashset_insert(&hashset, &i)) != SIM_RC_FAILURE) {
            *out_err_str = "insert: failed to return FAILURE for item already in hashset";
            return SIM_RC_FAILURE;
        }

        if ((rc = sim_hashset_contains(&hashset, &i))) {
            switch (rc) {
            case SIM_RC_ERR_NOTFOUND:
                *out_err_str = "contains: raised ERR_NOTFOUND for item in hashset";
                return SIM_RC_FAILURE;
            default:
                *out_err_str = "unexpected error out on contains";
                sim_hashset_destroy(&hashset);
                return rc;
            }
        }
    }

    i = 256;
    if ((rc = sim_hashset_contains(&hashset, &i)) != SIM_RC_ERR_NOTFOUND) {
        *out_err_str = "contains: failed to raise ERR_NOTFOUND for item not in hashset";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode hashset_test_remove(const char* *const out_err_str) {
    Sim_ReturnCode rc;
    int i = 0;

    if ((rc = sim_hashset_remove(NULL, &i)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "remove: failed to check for NULLPTR hashset";
        return SIM_RC_FAILURE;
    }
    if ((rc = sim_hashset_remove(&hashset, NULL)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "remove: failed to check for NULLPTR item";
        return SIM_RC_FAILURE;
    }

    i = 32;
    if ((rc = sim_hashset_remove(&hashset, &i))) {
        switch (rc) {
        case SIM_RC_ERR_NOTFOUND:
            *out_err_str = "remove: raised ERR_NOTFOUND for item in hashset";
                return SIM_RC_FAILURE;
        default:
            *out_err_str = "unexpected error out on remove";
            sim_hashset_destroy(&hashset);
            return rc;
        }
    }

    if ((rc = sim_hashset_contains(&hashset, &i)) != SIM_RC_ERR_NOTFOUND) {
        *out_err_str = "contains: failed to raise ERR_NOTFOUND for item removed from hashset";
        return SIM_RC_FAILURE;
    }

    i = 255;
    if ((rc = sim_hashset_remove(&hashset, &i)) != SIM_RC_ERR_NOTFOUND) {
        *out_err_str = "remove: failed to raise ERR_NOTFOUND for item not in hashset";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode hashset_test_destroy(const char* *const out_err_str) {
    sim_hashset_destroy(&hashset);

    if (simt_alloced_ptrs_size > 0) {
        *out_err_str = "destroy: failed to free dynamically allocated memory";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

SimT_TestFuncStruct hashset_tests[] = {
    { hashset_test_init, "initialization" },
    { hashset_test_insert, "insert & contains" },
    { hashset_test_remove, "remove & contains" },
    { hashset_test_destroy, "destructor" }
};

#endif /* SIMTEST_HASHSET_TESTS_H_ */
