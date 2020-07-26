/**
 * @file except.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Implementation of simsoft/except.h
 * @version 0.2
 * @date 2020-07-26
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_EXCEPT_C_
#define SIMSOFT_EXCEPT_C_

#include "simsoft/except.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _ERROR_BUF_SIZE 1024

// == TYPEDEFS =====================================================================================

typedef struct _OnThrowNode {
    Sim_ExceptOnThrowProc on_throw_proc;
    Sim_Variant callback_data;
    struct _OnThrowNode* previous;
} _OnThrowNode;

typedef struct _OnResetNode {
    Sim_ExceptOnResetProc on_reset_proc;
    Sim_Variant callback_data;
    struct _OnResetNode* previous;
} _OnResetNode;

typedef struct _JmpBufNode {
    jmp_buf jump_buffer;
    int return_code;
    _OnThrowNode* on_throw_head;
    struct _JmpBufNode* previous;
} _JmpBufNode;

// == THREAD-LOCAL STATICS =========================================================================

THREAD_LOCAL static _JmpBufNode*  _current_jmpbuf_node = NULL;
THREAD_LOCAL static _OnThrowNode* _last_on_no_catch_node = NULL;
THREAD_LOCAL static _OnResetNode* _last_on_reset_node = NULL;

THREAD_LOCAL static char _error_buf[_ERROR_BUF_SIZE];

// == CLEAN-UP FUNCTIONS (PRIVATE API) =============================================================

static void _atexit_free_function_nodes(void) {
    union {
        _OnResetNode* on_reset;
        _OnThrowNode* on_no_catch;
    } node_union;

    while (_last_on_no_catch_node) {
        node_union.on_no_catch = _last_on_no_catch_node;
        _last_on_no_catch_node = node_union.on_no_catch->previous;
        free(node_union.on_no_catch);
    }
    while (_last_on_reset_node) {
        node_union.on_reset = _last_on_reset_node;
        _last_on_reset_node = node_union.on_reset->previous;
        free(node_union.on_reset);
    }
}

// == PUBLIC API ===================================================================================

jmp_buf *const sim_except_push(void) {
    // allocate next jump buffer node
    _JmpBufNode* next = malloc(sizeof *_current_jmpbuf_node);
    if (!next)
        sim_except_throw(SIM_ERR_OUTOFMEM, "Couldn't allocate new jump buffer node");

    // initialize node contents
    memset(next->jump_buffer, 0, sizeof next->jump_buffer);
    next->return_code = SIM_NO_ERROR;
    next->on_throw_head = NULL;
    next->previous = _current_jmpbuf_node;

    // push current node to top of the exception stack
    _current_jmpbuf_node = next;

    // return jump buffer
    return &next->jump_buffer;
}

int sim_except_pop(void) {
    if (!_current_jmpbuf_node)
        sim_except_throw(SIM_ERR_JUMPBUF, "Attempted to pop when exception stack was empty");
    
    // pop current node off the exception stack
    _JmpBufNode* current = _current_jmpbuf_node;
    _current_jmpbuf_node = current->previous;

    // free on_throw stack on jmpbuf node
    _OnThrowNode* on_throw_node;
    while (current->on_throw_head) {
        on_throw_node = current->on_throw_head;
        current->on_throw_head = on_throw_node->previous;
        free(on_throw_node);
    }

    // get return code from popped node & free
    int rc = current->return_code;
    free(current);
    return rc;
}

int sim_except_peek(void) {
    return _current_jmpbuf_node ? _current_jmpbuf_node->return_code : SIM_NO_ERROR;
}

void sim_except_reset(void) {
    if (_current_jmpbuf_node)
        _current_jmpbuf_node->return_code = SIM_NO_ERROR;
    
    // call all registered on_reset functions
    _OnResetNode* node_ptr = _last_on_reset_node;
    while (node_ptr) {
        node_ptr->on_reset_proc(node_ptr->callback_data);
        node_ptr = node_ptr->previous;
    }
}

void sim_except_throw(int error_code, const char* error_msg, ...) {
    // only set _error_message if (error_msg) is set
    if (error_msg) {
        va_list args;
        va_start(args, error_msg);
        vsnprintf(_error_buf, ARR_SIZE(_error_buf), error_msg, args);
        va_end(args);
    }

    _OnThrowNode* node;

    // long jump to current jump buffer if available
    if (_current_jmpbuf_node) {
        _current_jmpbuf_node->return_code = error_code;

        // call all registered on_throw functions on uncaught exception
        node = _current_jmpbuf_node->on_throw_head;
        while (node) {
            node->on_throw_proc(error_code, _error_buf, node->callback_data);
            node = node->previous;
        }

        longjmp(_current_jmpbuf_node->jump_buffer, error_code);
    }

    // call all registered on_throw functions on uncaught exception
    node = _last_on_no_catch_node;
    while (node) {
        node->on_throw_proc(error_code, _error_buf, node->callback_data);
        node = node->previous;
    }

    exit(error_code);
}

void sim_except_on_reset(Sim_ExceptOnResetProc callback_proc, Sim_Variant callback_data) {
    if (!callback_proc)
        return;

    // allocate new node
    _OnResetNode* next = malloc(sizeof _last_on_reset_node);
    if (!next)
        sim_except_throw(SIM_ERR_OUTOFMEM, "Couldn't allocate new on_reset node");
    
    // initialize node contents
    next->on_reset_proc = callback_proc;
    next->callback_data = callback_data;

    // clean up function nodes at exit
    if (!_last_on_reset_node && !_last_on_no_catch_node)
        atexit(_atexit_free_function_nodes);

    // push to on_reset node stack
    next->previous = _last_on_reset_node;
    _last_on_reset_node = next;
}

void sim_except_on_throw(Sim_ExceptOnThrowProc callback_proc, Sim_Variant callback_data) {
    if (!callback_proc)
        return;

    // allocate new node
    _OnThrowNode* next = malloc(sizeof(_OnThrowNode));
    if (!next)
        sim_except_throw(SIM_ERR_OUTOFMEM, "Couldn't allocate new on_throw node");
    
    // initialize node contents
    next->on_throw_proc = callback_proc;
    next->callback_data = callback_data;

    // push to on_throw node stack for current jmpbuf node if it exists
    if (_current_jmpbuf_node) {
        next->previous = _current_jmpbuf_node->on_throw_head;
        _current_jmpbuf_node->on_throw_head = next;
    } else /* push to on_no_catch stack if no jmpbuf node exists */ {
        // clean up function nodes at exit
        if (!_last_on_reset_node && !_last_on_no_catch_node)
            atexit(_atexit_free_function_nodes);

        next->previous = _last_on_no_catch_node;
        _last_on_no_catch_node = next;
    }
}

void sim_except_on_no_catch(Sim_ExceptOnThrowProc callback_proc, Sim_Variant callback_data) {
    if (!callback_proc)
        return;

    // allocate new node
    _OnThrowNode* next = malloc(sizeof _last_on_no_catch_node);
    if (!next)
        sim_except_throw(SIM_ERR_OUTOFMEM, "Couldn't allocate new on_no_catch node");
    
    // initialize node contents
    next->on_throw_proc = callback_proc;
    next->callback_data = callback_data;

    // clean up function nodes at exit
    if (!_last_on_reset_node && !_last_on_no_catch_node)
        atexit(_atexit_free_function_nodes);

    // push to on_no_catch node stack
    next->previous = _last_on_no_catch_node;
    _last_on_no_catch_node = next;
}

#endif // SIMSOFT_EXCEPT_C_
