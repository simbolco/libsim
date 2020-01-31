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
#include "./vector_test.h"

SimTestStruct tests[1] = {
    {
        .func_ptr = vector_test,
        .name = "vector",
        .description = "Unit tests for the Sim_Vector type and related functions."
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

    SimT_TestInfo test_info;
    memset(&test_info, 0, sizeof(test_info));
    Sim_ReturnCode test_result = (*tests[test_id].func_ptr)(&test_info);

    printf(
        "%d tests performed:\n"
        " \33[92m✓\33[0m [%d/%d] passed\n"
        " \33[91mX\33[0m [%d/%d] failed\n"
        " \33[93m*\33[0m [%d/%d] remaining\n",
        test_info.total,
        test_info.passed, test_info.total,
        test_info.failed, test_info.total,
        test_info.remaining, test_info.total
    );
    
    if (test_result) {
        printf(
            ERR_STR("Test %d of suite returned error: \"%s\""),
            test_info.total - test_info.remaining,
            sim_get_return_code_string(test_result)
        );

        if (exit_on_failure)
            exit(-test_result);
    }
}

int main(int argc, char* argv[]) {
#   ifdef _WIN32
        _win32_ansi_setup();
#   endif
    setbuf(stdout, NULL);
    signal(SIGINT, sig_catch);

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

        int test_id = atoi(argv[i]) + 1;
        if (test_id > 0 && test_id < (int)NUM_TESTS)
            perform_test(test_id, exit_on_failure);
        else
            printf(ERR_STR("Invalid test ID '%d'"), test_id);
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

        if (!strcmp(input_buffer, "q") || !strcmp(input_buffer, "quit"))
            break;
        if (
            !strcmp(input_buffer, "?") ||
            !strcmp(input_buffer, "h") ||
            !strcmp(input_buffer, "help")
        ) {
            list_tests();
            continue;
        }

        int test_id = atoi(input_buffer);
        bool is_test = false;
        for (int i = 0; i < (int)NUM_TESTS; i++) {
            if (test_id == i + 1 || !strcmp(tests[i].name, input_buffer)) {
                perform_test(i, exit_on_failure);
                is_test = true;
                break;
            }
        }
        if (!is_test)
            printf(ERR_STR("\"%s\" isn't a valid command or test suite"), input_buffer);
    }
    return 0;
}
