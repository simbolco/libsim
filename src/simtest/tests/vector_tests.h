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

#include "../test.h"
#include "simsoft/vector.h"

static Sim_Vector vec;

Sim_ReturnCode vector_test_init(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    srand(time(NULL));

    if ((rc = sim_vector_initialize(
        NULL,
        sizeof(void*),
        SIM_DATATYPE_OTHER,
        NULL,
        256
    )) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "initialize: failed to check for NULLPTR vector";
        return SIM_RC_ERR_NULLPTR;
    }

    if ((rc = sim_vector_initialize(
        &vec,
        sizeof(int),
        SIM_DATATYPE_INTEGRAL,
        NULL,
        256
    ))) {
        *out_err_str = "unexpected error out on initialize";
        return rc;
    }

    if (vec._allocated != 256) {
        *out_err_str = "initialize: failed to set _allocated property to correct value";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_push(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    if ((rc = sim_vector_push(NULL, &rc)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "push: failed to check for NULLPTR vector";
        return SIM_RC_FAILURE;
    }

    if ((rc = sim_vector_push(&vec, NULL)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "push: failed to check for NULLPTR out_data_ptr";
        return SIM_RC_FAILURE;
    }

    for (int i = 0; i < 256; i++) {
        if ((rc = sim_vector_push(&vec, &i))) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on push";
            return rc;
        }
        if (vec.count != (size_t)i+1) {
            *out_err_str = "push: failed to increment count property";
            return SIM_RC_FAILURE;
        }
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_get(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    {
        if ((rc = sim_vector_get(NULL, 0, NULL)) != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "get: failed to check for NULLPTR vector";
            return SIM_RC_FAILURE;
        }
        if ((rc = sim_vector_get_ptr(NULL, 0, NULL)) != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "get_ptr: failed to check for NULLPTR vector";
            return SIM_RC_FAILURE;
        }

        int j;
        for (size_t i = 0; i < 256; i++) {
            if ((rc = sim_vector_get(&vec, i, &j))) {
                sim_vector_destroy(&vec);
                *out_err_str = "unexpected error out on get";
                return rc;
            }

            if (j != (int)i) {
                *out_err_str = "get: failed to retrieve correct value from vector";
                return SIM_RC_FAILURE;
            }
        }

        if ((rc = sim_vector_get(&vec, 256, &j)) != SIM_RC_ERR_OUTOFBND) {
            *out_err_str = "get: failed to throw ERR_OUTOFBND for out-of-bounds index";
            sim_vector_destroy(&vec);
            return SIM_RC_ERR_OUTOFBND;
        }
    }

    {
        int* k, old_val, new_val, retrieved_val;
        if ((rc = sim_vector_get_ptr(&vec, 32, (void**)&k))) {
            sim_vector_destroy(&vec);
            return rc;
        }

        if (!k) {
            *out_err_str = "get_ptr: returned NULL pointer for in-bounds index";
            sim_vector_destroy(&vec);
            return SIM_RC_ERR_NULLPTR;
        } else if (*k != 32) {
            *out_err_str = "get_ptr: retrieved pointer to incorrect value";
            return SIM_RC_FAILURE;
        }

        old_val = *k;

        new_val = rand() % 1024;
        *k = new_val;
        if ((rc = sim_vector_get(&vec, 32, &retrieved_val))) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on get";
            return rc;
        } else if (retrieved_val != new_val) {
            *out_err_str = "get: retrieved vector doesn't point to vector entry";
            return SIM_RC_FAILURE;
        }

        *k = old_val;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_contains(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    {
        int item;
        if (sim_vector_contains(NULL, &item, (Sim_PredicateFuncPtr)_int_eq)) {
            *out_err_str = "contains: returned true for NULLPTR vector";
            return SIM_RC_FAILURE;
        }
        if (sim_vector_contains(&vec, NULL, (Sim_PredicateFuncPtr)_int_eq)) {
            *out_err_str = "contains: returned true for NULLPTR item";
            return SIM_RC_FAILURE;
        }
        if (sim_vector_contains(&vec, &item, NULL)) {
            sim_vector_destroy(&vec);
            *out_err_str = "contains: returned true for NULLPTR predicate function";
            return SIM_RC_FAILURE;
        }

        if (
            (rc = sim_vector_index_of(NULL, &item, (Sim_PredicateFuncPtr)_int_eq, 0, NULL))
            != SIM_RC_ERR_NULLPTR
        ) {
            *out_err_str = "index_of: failed to check for NULLPTR vector";
            return SIM_RC_FAILURE;
        }
        if (
            (rc = sim_vector_index_of(&vec, NULL, (Sim_PredicateFuncPtr)_int_eq, 0, NULL))
            != SIM_RC_ERR_NULLPTR
        ) {
            *out_err_str = "index_of: failed to check for NULLPTR item";
            return SIM_RC_FAILURE;
        }
        if (
            (rc = sim_vector_index_of(&vec, &item, NULL, 0, NULL))
            != SIM_RC_ERR_NULLPTR
        ) {
            *out_err_str = "index_of: failed to check for NULLPTR predicate function";
            return SIM_RC_FAILURE;
        }
    }

    {
        int i = 16;
        if (!sim_vector_contains(&vec, &i, (Sim_PredicateFuncPtr)_int_eq)) {
            *out_err_str = "contains: returned false for item in the vector";
            return SIM_RC_FAILURE;
        }
        i = -30;
        if (sim_vector_contains(&vec, &i, (Sim_PredicateFuncPtr)_int_eq)) {
            *out_err_str = "contains: returned true for item not in the vector";
            return SIM_RC_FAILURE;
        }
    }

    {
        size_t ind;
        int i = rand() % vec.count, j;
        if ((rc = sim_vector_index_of(&vec, &i, (Sim_PredicateFuncPtr)_int_eq, 0, &ind))) {
            *out_err_str = "unexpected error out on index_of";
            sim_vector_destroy(&vec);
            return rc;
        }
        if ((rc = sim_vector_get(&vec, ind, &j))) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on get";
            return rc;
        }
        if (i != j) {
            *out_err_str = "index_of: wrong index returned";
            return SIM_RC_FAILURE;
        }

        j = -1;
        if (
            (rc = sim_vector_index_of(&vec, &j, (Sim_PredicateFuncPtr)_int_eq, 0, &ind))
            != SIM_RC_ERR_NOTFOUND
        ) {
            sim_vector_destroy(&vec);
            *out_err_str = "index_of: failed to throw ERR_NOTFOUND for item not in vector";
            return SIM_RC_ERR_NOTFOUND;
        }
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_remove(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    if ((rc = sim_vector_remove(NULL, NULL, 0)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "remove: failed to check for NULLPTR vector";
        return SIM_RC_FAILURE;
    }
    if ((rc = sim_vector_pop(NULL, NULL)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "pop: failed to check for NULLPTR vector";
        return SIM_RC_FAILURE;
    }
    
    size_t i = 0;
    int arr[256];

    while (!sim_vector_is_empty(&vec)) {
        size_t ind = (size_t)(rand() % vec.count);
        if ((rc = sim_vector_remove(&vec, &(arr[i++]), ind))) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on remove";
            return rc;
        }

        if (vec.count != 256 - i) {
            *out_err_str = "remove: failed to decrement count property";
            return SIM_RC_FAILURE;
        }
    }

    for (i = 0; i < 256; i++)
        if ((rc = sim_vector_push(&vec, &arr[i]))) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on push";
            return rc;
        }

    for (i = 0; i < 256; i++) {
        if ((rc = sim_vector_pop(&vec, &arr[i]))) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on pop";
            return rc;
        }

        if (vec.count != 255 - i) {
            *out_err_str = "pop: failed to decrement count property";
            return SIM_RC_FAILURE;
        }
    }
    
    for (i = 0; i < 256; i++)
        if ((rc = sim_vector_push(&vec, &arr[i]))) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on push";
            return rc;
        }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_clear(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    if ((rc = sim_vector_clear(NULL)) != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "clear: failed to check for NULLPTR vector";
        return SIM_RC_FAILURE;
    }

    if ((rc = sim_vector_clear(&vec))) {
        sim_vector_destroy(&vec);
        *out_err_str = "unexpected error out on clear";
        return rc;
    }

    if (vec.count != 0) {
        *out_err_str = "clear: failed to set count to 0 after clear";
        return SIM_RC_FAILURE;
    }

    if ((rc = sim_vector_pop(&vec, NULL)) != SIM_RC_ERR_OUTOFBND) {
        *out_err_str = "pop: failed to raise ERR_OUTOFBND given cleared vector";
        return SIM_RC_FAILURE;
    }

    {
        int i = 5;
        if ((rc = sim_vector_push(&vec, &i))) {
            sim_vector_destroy(&vec);
            *out_err_str = "error out on push given cleared vector";
            return rc;
        }

    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_destroy(const char* *const out_err_str) {
    sim_vector_destroy(&vec);

    if (simt_alloced_ptrs_size > 0) {
        *out_err_str = "destroy: failed to free dynamically allocated memory";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

SimT_TestFuncStruct vector_tests[] = {
    { vector_test_init, "initialization" },
    { vector_test_push, "push" },
    { vector_test_get,  "get & get_ptr" },
    { vector_test_contains, "contains & index_of" },
    { vector_test_remove, "remove & pop" },
    { vector_test_clear, "clear" },
    { vector_test_destroy, "destructor" }
};

#endif /* SIMTEST_VECTOR_TEST_H_ */
