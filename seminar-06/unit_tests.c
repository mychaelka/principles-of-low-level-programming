#define CUT
#define CUT_MAIN
#include "cut.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_lib.h"

volatile sig_atomic_t caught_signal;
static void failed_successfully(int sig)
{
    caught_signal = sig;
    exit(0);
}

#define FAILS_PRECONDITION(expr)              \
    do {                                      \
        caught_signal = 0;                    \
        signal(SIGABRT, failed_successfully); \
                                              \
        expr;                                 \
                                              \
        CHECK(caught_signal == SIGABRT);      \
        signal(SIGABRT, SIG_DFL);             \
    } while (0)

TEST(dyn_strcpy)
{
    const char *orig = "Ahoj svet !";

    SUBTEST(standard_string)
    {
        char *copy = dyn_strcpy(orig);
        CHECK(copy != NULL);
        CHECK(strcmp(orig, copy) == 0);
        free(copy);
    }

    SUBTEST(provided_null)
    {
        FAILS_PRECONDITION(dyn_strcpy(NULL));
    }
}

TEST(dyn_strjoin)
{
    const char *ahoj = "Ahoj ";
    const char *svet = "svet!";

    SUBTEST(two_words)
    {
        char *result = dyn_strjoin(ahoj, svet);
        CHECK(result != NULL);
        CHECK(strcmp(result, "Ahoj svet!") == 0);
        free(result);
    }

    SUBTEST(both_empty)
    {
        char *result = dyn_strjoin("", "");
        CHECK(result != NULL);
        CHECK(strcmp(result, "") == 0);
        free(result);
    }

    SUBTEST(both_null)
    {
        FAILS_PRECONDITION(dyn_strjoin(NULL, NULL));
    }

    SUBTEST(first_null)
    {
        FAILS_PRECONDITION(dyn_strjoin(NULL, ahoj));
    }

    SUBTEST(second_null)
    {
        FAILS_PRECONDITION(dyn_strjoin(ahoj, NULL));
    }
}

TEST(dyn_alloc2d)
{
    SUBTEST(three_rows_cols_from_one_to_three)
    {
        size_t cols[] = { 1, 2, 3 };
        char **matrix = (char **) dyn_alloc2d(3, cols);
        CHECK(matrix != NULL);

        for (int i = 0; i < 3; i++) {
            CHECK(matrix[i] != NULL);
            free(matrix[i]);
        }
        free(matrix);
    }

    SUBTEST(two_rows_with_zero)
    {
        size_t cols[] = { 0, 1 };
        char **matrix = (char **) dyn_alloc2d(2, cols);
        CHECK(matrix != NULL);

        CHECK(matrix[0] == NULL);

        CHECK(matrix[1] != NULL);
        free(matrix[1]);

        free(matrix);
    }

    SUBTEST(zero_rows)
    {
        size_t cols[] = { 0, 1 };
        FAILS_PRECONDITION(dyn_alloc2d(0, cols));
    }

    SUBTEST(columns_are_null)
    {
        FAILS_PRECONDITION(dyn_alloc2d(5, NULL));
    }
}

TEST(dyn_free2d)
{
    SUBTEST(free_three_rows)
    {
        int **matrix = malloc(sizeof(int *) * 3);

        for (int i = 0; i < 3; i++) {
            matrix[i] = malloc(sizeof(int) * (i + 1));
        }

        CHECK(dyn_free2d((void **) matrix, 3) == 0);
    }

    SUBTEST(null_memory)
    {
        CHECK(dyn_free2d(NULL, 2) == 1);
    }
}

TEST(string_split)
{
    SUBTEST(func_test)
    {
        const char *orig = "carrot, bannana, orange, kebab, mouse";
        const char expected[10][20] = { "carrot", "bannana", "orange", "kebab", "mouse" };
        size_t size;
        char **split = string_split(orig, ", ", &size);
        CHECK(size == 5);

        for (int i = 0; i < 5; i++) {
            CHECK(strcmp(expected[i], split[i]) == 0);
        }

        for (size_t i = 0; i < size; i++) {
            free(split[i]);
        }

        free(split);
    }

    SUBTEST(null_as_size)
    {
        FAILS_PRECONDITION(string_split("ahoj, svet", ", ", NULL));
    }

    SUBTEST(null_as_input)
    {
        size_t size = 0;
        FAILS_PRECONDITION(string_split(NULL, ", ", &size));
    }

    SUBTEST(null_as_substring)
    {
        size_t size = 0;
        FAILS_PRECONDITION(string_split("ahoj, svet", NULL, &size));
    }

    SUBTEST(separator_at_start_end)
    {
        const char *original = "|some|data|from|markdown|table|";
        const char expected[][10] = { "", "some", "data", "from", "markdown", "table", "" };
        size_t size;
        char **split = string_split(original, "|", &size);
        CHECK(size == 7);

        for (size_t i = 0; i < 7; i++) {
            CHECK(strcmp(expected[i], split[i]) == 0);
        }

        dyn_free2d((void **) split, size);
    }
}

TEST(string_replace_all)
{
    SUBTEST(original_null)
    {
        FAILS_PRECONDITION(string_replace_all(NULL, "from", "to"));
    }

    SUBTEST(from_null)
    {
        FAILS_PRECONDITION(string_replace_all("original", NULL, "to"));
    }

    SUBTEST(to_null)
    {
        FAILS_PRECONDITION(string_replace_all("original", "from", NULL));
    }

    SUBTEST(basic_usage)
    {
        char *result = string_replace_all("this is old", "old", "new");
        CHECK(strcmp(result, "this is new") == 0);
        free(result);
    }

    SUBTEST(substring_not_matched)
    {
        char *result = string_replace_all("this is old", "cookie monster", "little lamb");
        CHECK(strcmp(result, "this is old") == 0);
        free(result);
    }

    SUBTEST(replace_tabs)
    {
        char *result = string_replace_all("1\t2\t3\t4\t5\t6", "\t", ",");
        CHECK(strcmp(result, "1,2,3,4,5,6") == 0);
        free(result);
    }

    SUBTEST(replaces_at_start_and_end)
    {
        char *result = string_replace_all("TAG some text in between TAG", "TAG", "|");
        CHECK(strcmp(result, "| some text in between |") == 0);
        free(result);
    }
}
