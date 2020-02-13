/**
 * @file hashset_tests.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Hashset unit tests.
 * @version 0.1
 * @date 2020-02-05
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */
#ifndef SIMTEST_HASHSET_TESTS_C_
#define SIMTEST_HASHSET_TESTS_C_

#include "../test.h"
#include "simsoft/hashset.h"
#include "./hashset_tests.h"

static bool _int_eq(const int *const a, const int *const b) {
    return *a == *b;
}

static Sim_HashSet hashset;

Sim_ReturnCode hashset_test_construct(const char* *const out_err_str) {
    Sim_ReturnCode rc;
    srand(time(NULL));

    sim_hashset_construct(
        NULL,
        sizeof(void*),
        SIM_DATATYPE_OTHER,
        NULL,
        (Sim_PredicateFuncPtr)_int_eq,
        NULL,
        75
    );
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "construct: failed to check for NULLPTR hashset";
        return SIM_RC_ERR_NULLPTR;
    }

    sim_hashset_construct(
        &hashset,
        sizeof(void*),
        SIM_DATATYPE_OTHER,
        NULL,
        NULL,
        NULL,
        75
    );
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "construct: failed to check for NULLPTR predicate function";
        return SIM_RC_ERR_NULLPTR;
    }

    sim_hashset_construct(
        &hashset,
        sizeof(int),
        SIM_DATATYPE_INTEGRAL,
        NULL,
        (Sim_PredicateFuncPtr)_int_eq,
        NULL,
        75
    );
    if ((rc = sim_return_code())) {
        *out_err_str = "unexpected error out on construct";
        return rc;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode hashset_test_insert(const char* *const out_err_str) {
    Sim_ReturnCode rc;
    int i = 0;

    sim_hashset_insert(NULL, &i);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "insert: failed to check for NULLPTR hashset";
        return SIM_RC_FAILURE;
    }
    sim_hashset_insert(&hashset, NULL);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "insert: failed to check for NULLPTR item";
        return SIM_RC_FAILURE;
    }

    {
        bool r = sim_hashset_contains(NULL, &i);
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "contains: failed to check for NULLPTR hashset";
            return SIM_RC_FAILURE;
        }
        if (r) {
            *out_err_str = "contains: failed to return FALSE on error";
            return SIM_RC_FAILURE;
        }
        
        r = sim_hashset_contains(&hashset, NULL);
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "contains: failed to check for NULLPTR item";
            return SIM_RC_FAILURE;
        }
        if (r) {
            *out_err_str = "contains: failed to return FALSE on error";
            return SIM_RC_FAILURE;
        }
    }

    for (; i < 128; i++) {
        sim_hashset_insert(&hashset, &i);
        if ((rc = sim_return_code())) {
            sim_hashset_destroy(&hashset);
            *out_err_str = "unexpected error out on insert";
            return rc;
        }
        if (hashset.count != (size_t)(i + 1)) {
            *out_err_str = "insert: failed to increment count property";
            return SIM_RC_FAILURE;
        }

        sim_hashset_insert(&hashset, &i);
        if ((rc = sim_return_code())) {
            *out_err_str = "unexpected error out on insert";
            return rc;
        }
        if (hashset.count != (size_t)(i + 1)) {
            *out_err_str = "insert: incremented count for duplicate item";
            return SIM_RC_FAILURE;
        }

        if (!sim_hashset_contains(&hashset, &i)) {
            switch ((rc = sim_return_code())) {
            case SIM_RC_ERR_NOTFOUND:
                *out_err_str = "contains: raised ERR_NOTFOUND for item in hashset";
                return SIM_RC_FAILURE;
            case SIM_RC_SUCCESS:
                *out_err_str = "contains: returned FALSE for item in hashset";
                return SIM_RC_FAILURE;
            default:
                *out_err_str = "unexpected error out on contains";
                sim_hashset_destroy(&hashset);
                return rc;
            }
        }
    }

    i = 256;
    if (sim_hashset_contains(&hashset, &i)) {
        *out_err_str = "contains: returned TRUE for item not in hashset";
        return SIM_RC_FAILURE;
    }
    if (sim_return_code() != SIM_RC_ERR_NOTFOUND) {
        *out_err_str = "contains: failed to raise ERR_NOTFOUND for item not in hashset";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode hashset_test_remove(const char* *const out_err_str) {
    Sim_ReturnCode rc;
    int i = 0;

    sim_hashset_remove(NULL, &i);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "remove: failed to check for NULLPTR hashset";
        return SIM_RC_FAILURE;
    }
    sim_hashset_remove(&hashset, NULL);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "remove: failed to check for NULLPTR item";
        return SIM_RC_FAILURE;
    }

    i = 32;
    sim_hashset_remove(&hashset, &i);
    if ((rc = sim_return_code())) {
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

    if (sim_hashset_contains(&hashset, &i)) {
        *out_err_str = "contains: returned TRUE for item removed from hashset";
        return SIM_RC_FAILURE;
    }
    if (sim_return_code() != SIM_RC_ERR_NOTFOUND) {
        *out_err_str = "contains: failed to raise ERR_NOTFOUND for item removed from hashset";
        return SIM_RC_FAILURE;
    }

    i = 255;
    sim_hashset_remove(&hashset, &i);
    if (sim_return_code() != SIM_RC_ERR_NOTFOUND) {
        *out_err_str = "remove: failed to raise ERR_NOTFOUND for item not in hashset";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode hashset_test_destroy(const char* *const out_err_str) {
    sim_hashset_destroy(&hashset);

    if (simt_alloc_size() > 0) {
        *out_err_str = "destroy: failed to free dynamically allocated memory";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

#endif /* SIMTEST_HASHSET_TESTS_C_ */
