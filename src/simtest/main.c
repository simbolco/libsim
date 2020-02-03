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

#include "./tests/vector_tests.h"
#include "./tests/hashset_tests.h"
//#include "./tests/hashmap_tests.h"

SimTestStruct tests[] = {
    {
        .name = "vector",
        .description = "Unit tests for Sim_Vector.",
        .num_tests = sizeof(vector_tests) / sizeof(SimT_TestFuncStruct),
        .test_funcs = vector_tests
    },
    {
        .name = "hashset",
        .description = "Unit tests for Sim_HashSet.",
        .num_tests = sizeof(hashset_tests) / sizeof(SimT_TestFuncStruct),
        .test_funcs = hashset_tests
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
        "  \33[97mq, quit \33[0m- Quits this application\n\n"
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
        test_result  = (*tests[test_id].test_funcs[i].func_ptr)(&err_strs[i]);
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
            (tests[test_id].test_funcs[i].name ? " - " : " "),
            (tests[test_id].test_funcs[i].name ?
                tests[test_id].test_funcs[i].name :
                ""
            ),
            (tests[test_id].test_funcs[i].name ? " - " : ""),
            err_strs[i] ? err_strs[i] : "(none)"
        );
    }
    printf("\n");

    free(err_strs);
    
    if (test_result != SIM_RC_FAILURE && test_result != SIM_RC_SUCCESS) {
        printf(
            ERR_STR(" Test %d of suite returned error: \"%s\""),
            total - remaining,
            sim_get_return_code_string(test_result)
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

int main(int argc, char* argv[]) {
#   ifdef _WIN32
        _win32_ansi_setup();
#   endif
    setvbuf(stdout, NULL, _IONBF, 1024);
    setvbuf(stderr, NULL, _IONBF, 1024);
    signal(SIGINT, sig_catch);
    signal(SIGSEGV, sig_catch);

    sim_set_default_allocator(&simt_allocator);
    atexit(simt_atexit_free_all);

    bool exit_on_failure = false;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[1], "-f") || !strcmp(argv[1], "--exit-on-failure"))
            exit_on_failure = true;

        // perform all tests
        if (strlen(argv[i]) == 1 && argv[i][0] == '*') {
            for (int j = 0; j < (int)NUM_TESTS; j++)
                perform_test(j, exit_on_failure);
            break;
        }

        execute_test_id(argv[i], exit_on_failure);
    }
    
    char input_buffer[256];
    size_t input_len;

    list_tests();

    // main loop
    while (true) {
        printf("> ");
        fgets(input_buffer, sizeof(input_buffer), stdin);

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
        if (
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
