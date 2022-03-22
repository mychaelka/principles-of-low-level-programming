//
// Created by Wermington on 15.2.16.
//

#include "dynamic_lib.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"

#define UNUSED(a) ((void) (a))
#define NOT_IMPLEMENTED()                                      \
    do {                                                       \
        fprintf(stderr, "%s is not implemented!\n", __func__); \
        raise(SIGTERM);                                        \
    } while (0)

char *dyn_strcpy(const char *str)
{
    char * str_array = malloc(50 * sizeof(char));
    memset(str_array, 0, 50 * sizeof(char));
    int i = 0;

    while (str[i] != '\0') {
        str_array[i] = str[i];
        i++;
    }
    str_array[i] = '\0';

    return str_array;
}

char *dyn_strjoin(const char *pre, const char *post)
{
    if (pre == NULL || post == NULL) {
        return NULL;
    }

    char * joined = malloc(50 * sizeof(char));
    memset(joined, 0, 50 * sizeof(char));

    int i = 0;
    int j = 0;

    while (pre[i] != '\0') {
        joined[j] = pre[i];
        i++;
        j++;
    }

    i = 0;
    while (post[i] != '\0') {
        joined[j] = post[i];
        i++;
        j++;
    }

    joined[j] = '\0';

    return joined;
}

char *read_line(void)
{
    int size = 10 * sizeof(char);
    const int NUM_CHARACTERS = 10;

    char * line = malloc(size);
    memset(line, 0, size);

    int ch = getchar();
    int count = 0;
    int i = 0;

    while (ch != '\n' && ch != '\0' && ch != EOF) {
        if (count >= NUM_CHARACTERS) {
            line = realloc(line, 2 * size);
            memset(line + size, 0, size);
            size *= 2;
            count = 0;
        }
        line[i] = (char) ch;
        count++;
        i++;
        ch = getchar();
    }

    line[i] = '\0';

    return line;
}

void **dyn_alloc2d(size_t rows, const size_t row_sizes[rows])
{
    if (rows == 0 || row_sizes == NULL) {
        return NULL;
    }

    void ** dynamic_2D = malloc(rows * sizeof(void *));
    for (size_t i = 0; i < rows; i++) {
        dynamic_2D[i] = realloc(dynamic_2D[i], row_sizes[i]);

        if (dynamic_2D[i] == NULL) {
            for (size_t j = 0; j < i; j++) {
                free(dynamic_2D[j]);
            }
            free(dynamic_2D);
            return NULL;
        }
    }

    return dynamic_2D;
}

int dyn_free2d(void **memory, size_t rows)
{
    if (rows == 0 || memory == NULL) {
        return 1;
    }

    for (size_t i = 0; i < rows; i++) {
        free(memory[i]);
    }

    free(memory);

    return 0;
}

/**
 * BONUSES
 */

unsigned **pascal_triangle(size_t depth)
{
    UNUSED(depth);
    NOT_IMPLEMENTED();
    return NULL;
}

char **string_split(const char *orig, const char *splitter, size_t *size)
{
    // TODO
    UNUSED(orig);
    UNUSED(splitter);
    UNUSED(size);
    NOT_IMPLEMENTED();
    return NULL;
}

char *string_replace_all(const char *original, const char *from, const char *to)
{
    // TODO
    UNUSED(original);
    UNUSED(from);
    UNUSED(to);
    NOT_IMPLEMENTED();
    return NULL;
}
