/**
 * @file common.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Source file/implementation of simsoft/common.h
 * @version 0.1
 * @date 2020-04-05
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_COMMON_C_
#define SIMSOFT_COMMON_C_

#include "./_internal.h"

// thread-local return code value
static THREAD_LOCAL Sim_ReturnCode _sim_return_code = SIM_RC_SUCCESS;

// sim_get_return_code(0): Retrieves the return code from SimSoft library functions.
Sim_ReturnCode sim_get_return_code() {
    return _sim_return_code;
}

// sim_set_return_code(1): Sets the return code to a given value.
void sim_set_return_code(Sim_ReturnCode return_code) {
    _sim_return_code = return_code;
}

// return code strings
static const char* _SIM_EXPECTED_CODE_STRINGS[] = {
    "Success",
    "Failure",
    "Item not found",
    "Jump buffer stack empty"
};
static const char* _SIM_ERROR_CODE_STRINGS[] = {
    "Null pointer error",
    "Out of memory",
    "Zero division error",
    "Invalid argument error",
    "Out of bounds error",
    "Unsupported operation",
    "Unimplemented operation",
    "Bad file pointer"
};

// Verify size of return code string arrays
STATIC_ASSERT(
    sizeof(_SIM_EXPECTED_CODE_STRINGS) / sizeof(_SIM_EXPECTED_CODE_STRINGS[0]) == SIM_RC___COUNT,
    "Expected return code strings array count should be equal to"
    "the number of expected return codes"
);
STATIC_ASSERT(
    sizeof(_SIM_ERROR_CODE_STRINGS) / sizeof(_SIM_ERROR_CODE_STRINGS[0]) == SIM_RC_ERR___COUNT,
    "Error return code strings array count should be equal to the number of error return codes"
);

// thread-local error message
static THREAD_LOCAL const char* _sim_error_message = NULL;

// sim_get_return_code_string(1): Converts a return code to a human-readable string.
const char* sim_get_return_code_string(
    Sim_ReturnCode return_code
) {
    if (return_code < SIM_RC_ERR___LAST && return_code > SIM_RC_ERR___FIRST) // error code
        return _SIM_ERROR_CODE_STRINGS[return_code - SIM_RC_ERR___FIRST - 1];
    else if (return_code >= 0 && return_code < SIM_RC___COUNT) // expected code
        return _SIM_EXPECTED_CODE_STRINGS[return_code];
    
    // otherwise the given return code is invalid
    return "Invalid return code";
}

// sim_cexcept_get_error_message(0): 
const char* sim_cexcept_get_error_message(void) {
    return _sim_error_message;
}

// == C exception-related code =====================================================================

// jump buffer structure
typedef struct _Sim_JumpBufferNode {
    jmp_buf c_jump_buffer;      // standard C jmp_buf
    Sim_ReturnCode return_code; // SimSoft return code
    struct _Sim_JumpBufferNode* previous_node_ptr;
} _Sim_JumpBufferNode;

// the current jump buffer node in the linked-list stack
static THREAD_LOCAL _Sim_JumpBufferNode* _sim_current_jump_buffer_node = NULL;

// on throw linked list
typedef struct _Sim_CExceptOnThrowNode {
    Sim_CExceptOnThrowProc on_throw_proc;
    struct _Sim_CExceptOnThrowNode* previous_node_ptr;
} _Sim_CExceptOnThrowNode;

static THREAD_LOCAL _Sim_CExceptOnThrowNode* _sim_last_on_throw_node = NULL;

// on reset linked list
typedef struct _Sim_CExceptOnResetNode {
    Sim_CExceptOnResetProc on_reset_proc;
    struct _Sim_CExceptOnResetNode* previous_node_ptr;
} _Sim_CExceptOnResetNode;

static THREAD_LOCAL _Sim_CExceptOnResetNode* _sim_last_on_reset_node = NULL;

// free on_reset and on_throw linked lists
static void _sim_cexcept_atexit_free_function_nodes(void) {
    {
        _Sim_CExceptOnThrowNode* on_throw_node;
        while (_sim_last_on_throw_node) {
            on_throw_node = _sim_last_on_throw_node;
            _sim_last_on_throw_node = on_throw_node->previous_node_ptr;
            free(on_throw_node);
        }
    }

    {
        _Sim_CExceptOnResetNode* on_reset_node;
        while (_sim_last_on_throw_node) {
            on_reset_node = _sim_last_on_reset_node;
            _sim_last_on_reset_node = on_reset_node->previous_node_ptr;
            free(on_reset_node);
        }
    }
}

// -- public API -----------------------------------------------------------------------------------

// sim_cexcept_throw(2): Throws an error code as an exception.
void sim_cexcept_throw(Sim_ReturnCode error_code, const char* error_message) {
    // set return code + error message
    _sim_return_code = error_code;
    if (error_message)
        _sim_error_message = error_message;

    // call all registered on_throw functions
    _Sim_CExceptOnThrowNode* node_ptr = _sim_last_on_throw_node;
    while (node_ptr) {
        node_ptr->on_throw_proc(error_code);
        node_ptr = node_ptr->previous_node_ptr;
    }

    // long jump to the current jump buffer if available
    if (_sim_current_jump_buffer_node) {
        _sim_current_jump_buffer_node->return_code = error_code;
        longjmp(_sim_current_jump_buffer_node->c_jump_buffer, error_code);
    }

    // otherwise crash the program & print error
    printf(
        "The program crashed with error code %d - \"%s%s%s\"\n",
        error_code,
        _SIM_ERROR_CODE_STRINGS[error_code - SIM_RC_ERR___FIRST - 1],
        _sim_error_message ? ": " : "",
        _sim_error_message ? _sim_error_message : ""
    );
    exit(error_code);
}

// sim_cexcept_push(0): Retrieves a new jump buffer for exception handling.
jmp_buf *const sim_cexcept_push(void) {
    // allocate next list item
    _Sim_JumpBufferNode* next = malloc(sizeof *_sim_current_jump_buffer_node);

    // failed to allocate new node -- throw ERR_OUTOFMEM
    if (!next)
        THROW(SIM_RC_ERR_OUTOFMEM);

    // initialize node contents
    memset(next->c_jump_buffer, 0, sizeof next->c_jump_buffer);
    next->return_code = SIM_RC_SUCCESS;
    next->previous_node_ptr = _sim_current_jump_buffer_node;

    // set current jump buffer node & return
    _sim_current_jump_buffer_node = next;
    RETURN(SIM_RC_SUCCESS, &next->c_jump_buffer);
}

// sim_cexcept_pop(0): Deletes the current jump buffer.
Sim_ReturnCode sim_cexcept_pop(void) {
    if (_sim_current_jump_buffer_node)
        RETURN(SIM_RC_NO_JMPBUF, SIM_RC_NO_JMPBUF);

    // set current node to the previous one
    _Sim_JumpBufferNode* current = _sim_current_jump_buffer_node;
    _sim_current_jump_buffer_node = current->previous_node_ptr;

    Sim_ReturnCode rc = current->return_code;
    free(current);
    RETURN(SIM_RC_SUCCESS, rc);
}

// sim_cexcept_reset(0): Resets the error status of the current jump buffer.
void sim_cexcept_reset(void) {
    if (_sim_current_jump_buffer_node)
        _sim_current_jump_buffer_node->return_code = SIM_RC_SUCCESS;
    _sim_return_code = SIM_RC_SUCCESS;

    // call all registered on_reset functions
    _Sim_CExceptOnResetNode* node_ptr = _sim_last_on_reset_node;
    while (node_ptr) {
        node_ptr->on_reset_proc();
        node_ptr = node_ptr->previous_node_ptr;
    }
}

// sim_cexcept_on_reset(1): Registers a function to be called when sim_cexcept_reset() is called.
void sim_cexcept_on_reset(Sim_CExceptOnResetProc on_reset_proc) {
    if (!on_reset_proc)
        THROW(SIM_RC_ERR_NULLPTR);

    _Sim_CExceptOnResetNode* node_ptr = malloc(sizeof _sim_last_on_reset_node);
    if (!node_ptr)
        THROW(SIM_RC_ERR_OUTOFMEM);

    node_ptr->on_reset_proc = on_reset_proc;
    node_ptr->previous_node_ptr = _sim_last_on_reset_node;

    if (!node_ptr->previous_node_ptr) {
        atexit(_sim_cexcept_atexit_free_function_nodes);
    }

    _sim_last_on_reset_node = node_ptr;
}

// sim_cexcept_on_throw(1): Registers a function to be called when sim_cexcept_throw() is called.
void sim_cexcept_on_throw(Sim_CExceptOnThrowProc on_throw_proc) {
    if (!on_throw_proc)
        THROW(SIM_RC_ERR_NULLPTR);

    _Sim_CExceptOnThrowNode* node_ptr = malloc(sizeof _sim_last_on_throw_node);
    if (!node_ptr)
        THROW(SIM_RC_ERR_OUTOFMEM);

    node_ptr->on_throw_proc = on_throw_proc;
    node_ptr->previous_node_ptr = _sim_last_on_throw_node;

    if (!node_ptr->previous_node_ptr) {
        atexit(_sim_cexcept_atexit_free_function_nodes);
    }

    _sim_last_on_throw_node = node_ptr;
}

#endif // SIMSOFT_COMMON_C_
