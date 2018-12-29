#pragma once

int tests_run, tests_failed = 0;

#define run_test(message, test)                      \
    do {                                             \
        if (!tests_run) {                            \
            fprintf(stdout, "Running tests...\n");   \
        }                                            \
        if (!test()) {                               \
            fprintf(stdout, "%s: Error\n", message); \
            tests_failed++;                          \
        } else {                                     \
            fprintf(stdout, "%s: Ok\n", message);    \
        }                                            \
        tests_run++;                                 \
    } while (0)

#define show_tests_result                                        \
    do {                                                         \
        fprintf(stdout, "Tests run: %d\n", tests_run);           \
        if (!tests_failed) {                                     \
            fprintf(stdout, "All tests passed!\n");              \
        } else {                                                 \
            fprintf(stdout, "Tests failed: %d\n", tests_failed); \
        }                                                        \
    } while (0)
