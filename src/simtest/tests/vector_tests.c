/**
 * @file vector_tests.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source for vector unit tests.
 * @version 0.1
 * @date 2020-02-05
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */
#ifndef SIMTEST_VECTOR_TESTS_C_
#define SIMTEST_VECTOR_TESTS_C_

#include "./vector_tests.h"
#include "../test.h"
#include "simsoft/vector.h"

static bool _int_eq(const int *const a, const int *const b) {
    return *a == *b;
}

static Sim_Vector vec;

Sim_ReturnCode vector_test_construct(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    srand(time(NULL));

    sim_vector_construct(
        NULL,
        sizeof(void*),
        SIM_DATATYPE_OTHER,
        NULL,
        256
    );
    printf("%s\n", sim_debug_get_return_code_string(sim_return_code()));
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "construct: failed to check for NULLPTR vector";
        return SIM_RC_ERR_NULLPTR;
    }

    sim_vector_construct(
        &vec,
        sizeof(int),
        SIM_DATATYPE_INTEGRAL,
        NULL,
        256
    );
    if ((rc = sim_return_code())) {
        *out_err_str = "unexpected error out on construct";
        return rc;
    }

    if (vec._allocated != 256) {
        *out_err_str = "construct: failed to set _allocated property to correct value";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_push(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    sim_vector_push(NULL, &rc);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "push: failed to check for NULLPTR vector";
        return SIM_RC_FAILURE;
    }

    sim_vector_push(&vec, NULL);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "push: failed to check for NULLPTR out_data_ptr";
        return SIM_RC_FAILURE;
    }

    for (int i = 0; i < 256; i++) {
        sim_vector_push(&vec, &i);
        if ((rc = sim_return_code())) {
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
        sim_vector_get(NULL, 0, NULL);
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "get: failed to check for NULLPTR vector";
            return SIM_RC_FAILURE;
        }

        {
            int* i =sim_vector_get_ptr(NULL, 0);
            if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
                *out_err_str = "get_ptr: failed to check for NULLPTR vector";
                return SIM_RC_FAILURE;
            }
            if (i) {
                *out_err_str = "get_ptr: failed to return NULLPTR on error";
                return SIM_RC_FAILURE;
            }
        }

        int j;
        for (size_t i = 0; i < 256; i++) {
            sim_vector_get(&vec, i, &j);
            if ((rc = sim_return_code())) {
                sim_vector_destroy(&vec);
                *out_err_str = "unexpected error out on get";
                return rc;
            }

            if (j != (int)i) {
                *out_err_str = "get: failed to retrieve correct value from vector";
                return SIM_RC_FAILURE;
            }
        }

        sim_vector_get(&vec, 256, &j);
        if (sim_return_code() != SIM_RC_ERR_OUTOFBND) {
            *out_err_str = "get: failed to throw ERR_OUTOFBND for out-of-bounds index";
            sim_vector_destroy(&vec);
            return SIM_RC_ERR_OUTOFBND;
        }
    }

    {
        int old_val, new_val, retrieved_val;
        int* k = sim_vector_get_ptr(&vec, 32);
        if ((rc = sim_return_code())) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on get_ptr";
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
        
        sim_vector_get(&vec, 32, &retrieved_val);
        if ((rc = sim_return_code())) {
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
    {
        int item;
        if (sim_vector_contains(NULL, &item, (Sim_PredicateProc)_int_eq)) {
            *out_err_str = "contains: returned TRUE for NULLPTR vector";
            return SIM_RC_FAILURE;
        }
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "contains: failed to check for NULLPTR vector";
            return SIM_RC_FAILURE;
        }

        if (sim_vector_contains(&vec, NULL, (Sim_PredicateProc)_int_eq)) {
            *out_err_str = "contains: returned true for NULLPTR item";
            return SIM_RC_FAILURE;
        }
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "contains: failed to check for NULLPTR item";
            return SIM_RC_FAILURE;
        }

        if (sim_vector_contains(&vec, &item, NULL)) {
            *out_err_str = "contains: returned true for NULLPTR predicate function";
            return SIM_RC_FAILURE;
        }
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "contains: failed to check for NULLPTR predicate function";
            return SIM_RC_FAILURE;
        }

        size_t ind = sim_vector_index_of(NULL, &item, (Sim_PredicateProc)_int_eq, 0);
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "index_of: failed to check for NULLPTR vector";
            return SIM_RC_FAILURE;
        }
        if (ind != (size_t)-1) {
            *out_err_str = "index_of: failed to return -1 on error";
        }

        ind = sim_vector_index_of(&vec, NULL, (Sim_PredicateProc)_int_eq, 0);
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "index_of: failed to check for NULLPTR item";
            return SIM_RC_FAILURE;
        }
        if (ind != (size_t)-1) {
            *out_err_str = "index_of: failed to return -1 on error";
        }
        
        ind = sim_vector_index_of(&vec, &item, NULL, 0);
        if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
            *out_err_str = "index_of: failed to check for NULLPTR predicate function";
            return SIM_RC_FAILURE;
        }
        if (ind != (size_t)-1) {
            *out_err_str = "index_of: failed to return -1 on error";
        }
    }

    {
        int i = 16;
        if (!sim_vector_contains(&vec, &i, (Sim_PredicateProc)_int_eq)) {
            *out_err_str = "contains: returned false for item in the vector";
            return SIM_RC_FAILURE;
        }
        i = -30;
        if (sim_vector_contains(&vec, &i, (Sim_PredicateProc)_int_eq)) {
            *out_err_str = "contains: returned true for item not in the vector";
            return SIM_RC_FAILURE;
        }
    }

    Sim_ReturnCode rc;

    {
        int i = rand() % vec.count, j;

        size_t ind = sim_vector_index_of(&vec, &i, (Sim_PredicateProc)_int_eq, 0);
        if ((rc = sim_return_code())) {
            *out_err_str = "unexpected error out on index_of";
            sim_vector_destroy(&vec);
            return rc;
        }
        sim_vector_get(&vec, ind, &j);
        if ((rc = sim_return_code())) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on get";
            return rc;
        }
        if (i != j) {
            *out_err_str = "index_of: wrong index returned";
            return SIM_RC_FAILURE;
        }

        j = -1;
        ind = sim_vector_index_of(&vec, &j, (Sim_PredicateProc)_int_eq, 0);
        if (sim_return_code() != SIM_RC_ERR_NOTFOUND) {
            sim_vector_destroy(&vec);
            *out_err_str = "index_of: failed to throw ERR_NOTFOUND for item not in vector";
            return SIM_RC_ERR_NOTFOUND;
        }
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_remove(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    sim_vector_remove(NULL, NULL, 0);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "remove: failed to check for NULLPTR vector";
        return SIM_RC_FAILURE;
    }
    sim_vector_pop(NULL, NULL);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "pop: failed to check for NULLPTR vector";
        return SIM_RC_FAILURE;
    }
    
    size_t i = 0;
    int arr[256];

    while (!sim_vector_is_empty(&vec)) {
        size_t ind = (size_t)(rand() % vec.count);
        sim_vector_remove(&vec, &(arr[i++]), ind);
        if ((rc = sim_return_code())) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on remove";
            return rc;
        }

        if (vec.count != 256 - i) {
            *out_err_str = "remove: failed to decrement count property";
            return SIM_RC_FAILURE;
        }
    }

    for (i = 0; i < 256; i++) {
        sim_vector_push(&vec, &arr[i]);
        if ((rc = sim_return_code())) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on push";
            return rc;
        }
    }

    for (i = 0; i < 256; i++) {
        sim_vector_pop(&vec, &arr[i]);
        if ((rc = sim_return_code())) {
            sim_vector_destroy(&vec);
            *out_err_str = "unexpected error out on pop";
            return rc;
        }

        if (vec.count != 255 - i) {
            *out_err_str = "pop: failed to decrement count property";
            return SIM_RC_FAILURE;
        }
    }
    
    for (i = 0; i < 256; i++) {
        sim_vector_push(&vec, &arr[i]);
        if ((rc = sim_return_code())) {
            *out_err_str = "unexpected error out on push";
            return rc;
        }
    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_clear(const char* *const out_err_str) {
    Sim_ReturnCode rc;

    sim_vector_clear(NULL);
    if (sim_return_code() != SIM_RC_ERR_NULLPTR) {
        *out_err_str = "clear: failed to check for NULLPTR vector";
        return SIM_RC_FAILURE;
    }

    sim_vector_clear(&vec);
    if ((rc = sim_return_code())) {
        sim_vector_destroy(&vec);
        *out_err_str = "unexpected error out on clear";
        return rc;
    }

    if (vec.count != 0) {
        *out_err_str = "clear: failed to set count to 0 after clear";
        return SIM_RC_FAILURE;
    }

    sim_vector_pop(&vec, NULL);
    if (sim_return_code() != SIM_RC_ERR_OUTOFBND) {
        *out_err_str = "pop: failed to raise ERR_OUTOFBND given cleared vector";
        return SIM_RC_FAILURE;
    }

    {
        int i = 5;
        sim_vector_push(&vec, &i);
        if ((rc = sim_return_code())) {
            sim_vector_destroy(&vec);
            *out_err_str = "error out on push given cleared vector";
            return rc;
        }

    }

    return SIM_RC_SUCCESS;
}

Sim_ReturnCode vector_test_destroy(const char* *const out_err_str) {
    sim_vector_destroy(&vec);

    if (simt_alloc_size() > 0) {
        *out_err_str = "destroy: failed to free dynamically allocated memory";
        return SIM_RC_FAILURE;
    }

    return SIM_RC_SUCCESS;
}

#endif /* SIMTEST_VECTOR_TEST_C_ */
