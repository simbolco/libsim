/**
 * @file main.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Main source file for testing suite.
 * @version 0.1
 * @date 2020-01-29
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#include "./test.h"
#include "simsoft/dynlib.h"
#include "simsoft/util.h"

#include "./tests/vector_tests.h"
#include "./tests/hashset_tests.h"

#ifdef _WIN32
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>

#   ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#       define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x004
#   endif
#   ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#       define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#   endif

#   ifdef DEBUG
        static DWORD _win32_print_last_error(const char* err_format_str, ...) {
            DWORD err = GetLastError();

            CHAR err_msg_buffer[512];
            FormatMessageA(
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                err_msg_buffer,
                512,
                NULL
            );

            {
                va_list args;
                va_start(args, err_format_str);
                vfprintf(stderr, err_format_str, args);
                va_end(args);
            }

            fprintf(stderr, " returned error %ld: %s\n", err, err_msg_buffer);
            return err;
        }
#   else
#       define _win32_print_last_error(...) {}
#   endif

    static void _win32_ansi_setup() {
        if (!SetConsoleOutputCP(65001))
            _win32_print_last_error("SetConsoleOutputCP(65001)");

        HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (stdout_handle == INVALID_HANDLE_VALUE) {
            _win32_print_last_error("GetStdHandle(STD_OUTPUT_HANDLE)");
            return;
        }

        DWORD console_mode = 0;
        if (!GetConsoleMode(stdout_handle, &console_mode)) {
            _win32_print_last_error(
                "GetConsoleMode(stdout_handle <%p>, &console_mode)",
                (void*)stdout_handle
            );
            return;
        }
        console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(stdout_handle, console_mode)) {
            _win32_print_last_error(
                "SetConsoleMode(stdout_handle <%p>, %lu)",
                (void*)stdout_handle,
                console_mode
            );
        }
    }
#endif

// ================================================================================================

static void sigint_catch(int signal_number) {
    (void)signal_number;
    printf("^C");
    exit(2);
}

static void sigsegv_catch(int signal_number) {
    (void)signal_number;
    printf("SEGMENTATION FAULT!\n");

    Sim_BacktraceInfo backtrace_info[32];
    memset(backtrace_info, 0, sizeof(backtrace_info));
    size_t num_frames;
    
    if ((num_frames = sim_debug_get_backtrace_info(backtrace_info, 32, 0))) {
        printf("Backtrace:\n");
        for (size_t i = 0; i < num_frames; i++) {
            printf(" %p - %s%s%s%s\n",
                backtrace_info[i].function_address,
                backtrace_info[i].function_name ?
                    backtrace_info[i].function_name :
                    "????"
                ,
                backtrace_info[i].file_name ? " (" : "",
                backtrace_info[i].file_name ?
                    backtrace_info[i].file_name :
                    ""
                ,
                backtrace_info[i].file_name ? ")" : ""
            );
        }
    }
    
    exit(-1);
}

// ================================================================================================

#define SIMT_ALLOCED_PTRS_MAX_SIZE 1024

typedef struct SimT_Allocator {
    Sim_IAllocator allocator;
    size_t ptrs_size;
    void* ptrs[SIMT_ALLOCED_PTRS_MAX_SIZE];
    bool locked;
} SimT_Allocator;

static SimT_Allocator SIMT_ALLOCATOR = {
    .allocator = {
        simt_malloc,
        simt_falloc,
        simt_realloc,
        simt_free
    },
    .ptrs_size = 0,
    .locked = false
};

void* simt_malloc(size_t size) {
    if (SIMT_ALLOCATOR.locked)
        return NULL;
    
    if (SIMT_ALLOCATOR.ptrs_size == SIMT_ALLOCED_PTRS_MAX_SIZE - 1)
        return NULL;

    void* ptr = sim_allocator_default_malloc(size);
    if (!ptr)
        return NULL;

    SIMT_ALLOCATOR.ptrs[SIMT_ALLOCATOR.ptrs_size++] = ptr;
    return ptr;
}

void* simt_falloc(size_t size, ubyte fill) {
    if (SIMT_ALLOCATOR.locked)
        return NULL;

    if (SIMT_ALLOCATOR.ptrs_size == SIMT_ALLOCED_PTRS_MAX_SIZE - 1)
        return NULL;

    void* ptr = sim_allocator_default_falloc(size, fill);
    if (!ptr)
        return NULL;

    SIMT_ALLOCATOR.ptrs[SIMT_ALLOCATOR.ptrs_size++] = ptr;
    return ptr;
}

void* simt_realloc(void* ptr, size_t size) {
    if (SIMT_ALLOCATOR.locked)
        return NULL;

    void* new_ptr = sim_allocator_default_realloc(ptr, size);
    if (!new_ptr)
        return NULL;

    for (size_t i = 0; i < SIMT_ALLOCATOR.ptrs_size; i++) {
        if (ptr == SIMT_ALLOCATOR.ptrs[i]) {
            SIMT_ALLOCATOR.ptrs[i] = new_ptr;
            break;
        }
    }

    return new_ptr;
}

void simt_free(void* ptr) {
    if (!ptr)
        return;
    
    for (size_t i = 0; i < SIMT_ALLOCATOR.ptrs_size; i++) {
        if (ptr == SIMT_ALLOCATOR.ptrs[i]) {
            memcpy(
                &SIMT_ALLOCATOR.ptrs[i],
                &SIMT_ALLOCATOR.ptrs[i + 1],
                (SIMT_ALLOCATOR.ptrs_size - i) * sizeof(void*)
            );
            break;
        }
    }
    sim_allocator_default_free(ptr);
    SIMT_ALLOCATOR.ptrs_size--;
}

size_t simt_alloc_size() {
    return SIMT_ALLOCATOR.ptrs_size;
}

void simt_alloc_set_lock(bool lock) {
    SIMT_ALLOCATOR.locked = lock;
}

// ================================================================================================

static Sim_HashMap simt_dynlibs;

typedef struct SimT_String {
    char* str;
    size_t size;
    size_t hash1;
    size_t hash2;
    bool has_hash;
} SimT_String;

static size_t simt_strhash(SimT_String *const str_ptr, const size_t attempt) {
    // Hash keys for hash fallback + double hashing
#   define SIPHASH_KEY1 0x90d6346e7b77f546ULL
#   define SIPHASH_KEY2 0x1e0a6097372b5de5ULL
#   define SIPHASH_KEY3 0x62d76395429756a9ULL
#   define SIPHASH_KEY4 0xe26534637479058cULL
    
    if (!str_ptr->has_hash) {
        str_ptr->hash1 = sim_siphash(
            (const uint8*)str_ptr->str,
            str_ptr->size,
            (Sim_HashKey){ SIPHASH_KEY1, SIPHASH_KEY2 }
        );
        str_ptr->hash2 = sim_siphash(
            (const uint8*)str_ptr->str,
            str_ptr->size,
            (Sim_HashKey){ SIPHASH_KEY3, SIPHASH_KEY4 }
        );
    }

    return str_ptr->hash1 + (str_ptr->hash2 * attempt) + attempt;

#   undef SIPHASH_KEY1
#   undef SIPHASH_KEY2
#   undef SIPHASH_KEY3
#   undef SIPHASH_KEY4
}

static bool simt_streq(const SimT_String *const str1_ptr, const SimT_String *const str2_ptr) {
    return !!strcmp(str1_ptr->str, str2_ptr->str);
}

typedef struct SimT_DynlibsMapPair {
    const SimT_String*       key;
    Sim_LibraryHandle *const value;
} SimT_DynlibsMapPair;

static bool simt_dynlibs_foreach_clean(
    SimT_DynlibsMapPair *const key_value_pair_ptr,
    const size_t index,
    Sim_Variant userdata
) {
    (void)userdata; (void)index;
    printf("This should be empty!\n");
    free(key_value_pair_ptr->key->str);
    sim_dynlib_unload(*key_value_pair_ptr->value);
    return true; 
}

static void simt_atexit_free_all(void) {
    while (SIMT_ALLOCATOR.ptrs_size > 0)
        simt_free(SIMT_ALLOCATOR.ptrs[0]);

    sim_hashmap_foreach(
        &simt_dynlibs,
        (Sim_MapForEachProc)simt_dynlibs_foreach_clean,
        (Sim_Variant)0
    );
    sim_hashmap_destroy(&simt_dynlibs);
}

// ================================================================================================

static SimTestStruct tests[] = {
    {
        .name = "vector",
        .description = "Unit tests for Sim_Vector.",
        .num_tests = 7,
        .test_procs = (SimT_TestProcStruct []){
            { vector_test_construct, "constructor" },
            { vector_test_push,      "push" },
            { vector_test_get,       "get & get_ptr" },
            { vector_test_contains,  "contains & index_of" },
            { vector_test_remove,    "remove & pop" },
            { vector_test_clear,     "clear" },
            { vector_test_destroy,   "destructor" }
        }
    },
    {
        .name = "hashset",
        .description = "Unit tests for Sim_HashSet.",
        .num_tests = 4,
        .test_procs = (SimT_TestProcStruct []){
            { hashset_test_construct, "constructor" },
            { hashset_test_insert,    "insert & contains" },
            { hashset_test_remove,    "remove & contains" },
            { hashset_test_destroy,   "destructor" }
        }
    }
};

#define NUM_TESTS (sizeof(tests) / sizeof(SimTestStruct))

void list_tests() {
    printf("\33[1;97mSimSoft library test suites:\33[0m\n");
    for (int i = 0; i < (int)NUM_TESTS; i++) {
        printf(" [\33[97m%d\33[0m]", i + 1);
        if (tests[i].name)
            printf(" - \33[33m%s\33[0m", tests[i].name);
        if (tests[i].description)
            printf(": %s", tests[i].description);
        printf("\n");
    }

    printf(
        "\33[1;97mCommands:\33[0m\n"
        "  \33[97mh, help, ? \33[0m- Prints this help screen\n"
        "  \33[97mq, quit \33[0m- Quits this application\n"
        "\n"
    );
}

void perform_test(int test_id, bool exit_on_failure) {
    printf("Performing test suite [%d]", test_id + 1);
    if (tests[test_id].name)
        printf(" - \"%s\"...", tests[test_id].name);
    printf("\n");

    int
        total  = tests[test_id].num_tests,
        passed = 0,
        failed = 0,
        remaining = total
    ;
    Sim_ReturnCode test_result = SIM_RC_SUCCESS;

    const char** err_strs = calloc((size_t)tests[test_id].num_tests, sizeof(const char*));

    for (int i = 0; i < total; i++) {
        remaining--;
        test_result  = (*tests[test_id].test_procs[i].proc)(&err_strs[i]);
        if (test_result == SIM_RC_SUCCESS)
            passed++;
        else {
            failed++;
            if (test_result != SIM_RC_FAILURE)
                break;
        }
    }

    printf(
        " %d %s performed:\n"
        "  \33[92m✓\33[0m [%d/%d] passed\n"
        "  \33[91mX\33[0m [%d/%d] failed\n"
        "  \33[93m*\33[0m [%d/%d] remaining\n\n",
        total - remaining,
        (total - remaining) == 1 ? "test" : "tests",
        passed, total,
        failed, total,
        remaining, total
    );

    printf(" Test status strings:\n");
    for (int i = 0; i < total - remaining; i++) {
        printf(
            "  [%d]%s%s%s%s\n",
            i + 1,
            (tests[test_id].test_procs[i].name ? " - " : " "),
            (tests[test_id].test_procs[i].name ?
                tests[test_id].test_procs[i].name :
                ""
            ),
            (tests[test_id].test_procs[i].name ? " - " : ""),
            err_strs[i] ? err_strs[i] : "(none)"
        );
    }
    printf("\n");

    free(err_strs);
    
    if (test_result != SIM_RC_FAILURE && test_result != SIM_RC_SUCCESS) {
        printf(
            ERR_STR(" Test %d of suite returned error: \"%s\""),
            total - remaining,
            sim_debug_get_return_code_string(test_result)
        );

        if (exit_on_failure)
            exit(-test_result);
    }
}

void execute_test_id(const char* id_str, bool exit_on_failure) {
    int test_id = atoi(id_str);
    if (test_id > 0 && test_id <= (int)NUM_TESTS) {
        perform_test(test_id-1, exit_on_failure);
        return;
    }

    for (int i = 1; i <= (int)NUM_TESTS; i++) {
        if (!strcmp(tests[i-1].name, id_str)) {
            perform_test(i-1, exit_on_failure);
            return;
        }
    }

    printf(ERR_STR("Invalid test ID '%s'"), id_str);
}

// ================================================================================================

int main(int argc, char* argv[]) {
#   ifdef _WIN32
        _win32_ansi_setup();
#   endif
    setvbuf(stdout, NULL, _IONBF, 1024);
    setvbuf(stderr, NULL, _IONBF, 1024);
    signal(SIGINT,  sigint_catch);
    signal(SIGSEGV, sigsegv_catch);

    sim_hashmap_construct(
        &simt_dynlibs,
        sizeof(const char*),
        SIM_DATATYPE_OTHER,
        (Sim_HashProc)simt_strhash,
        (Sim_PredicateProc)simt_streq,
        sizeof(Sim_LibraryHandle),
        NULL,
        53
    );

    sim_allocator_set_default(&SIMT_ALLOCATOR.allocator);
    atexit(simt_atexit_free_all);

    bool exit_on_failure = false;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--exit-on-failure")) {
            exit_on_failure = true;
            continue;
        }

        // perform all tests
        if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--perform-all-tests")) {
            for (int j = 0; j < (int)NUM_TESTS; j++)
                perform_test(j, exit_on_failure);
            continue;
        }

        execute_test_id(argv[i], exit_on_failure);
    }
    
    char input_buffer[256];
    size_t input_len;

    list_tests();

    // main loop
    while (true) {
        printf("> ");
        (void)fgets(input_buffer, sizeof(input_buffer), stdin);

        char* pos;
        if (!(pos = memchr(input_buffer, '\n', sizeof(input_buffer)))) {
            printf(ERR_STR("Input too long"));
            continue;
        }

        *pos = '\0';
        input_len = (size_t)((ptrdiff_t)pos - (ptrdiff_t)input_buffer);
        if (input_len == 0)
            continue;

        // Test for special commands
        if (!strcmp(input_buffer, "q") || !strcmp(input_buffer, "quit"))
            // break on q or quit
            break;
        else if (
            !strcmp(input_buffer, "?") ||
            !strcmp(input_buffer, "h") ||
            !strcmp(input_buffer, "help")
        ) {
            // list tests on ?, h, or help
            list_tests();
            continue;
        }

        // else execute test id
        execute_test_id(input_buffer, exit_on_failure);
    }
    return 0;
}
