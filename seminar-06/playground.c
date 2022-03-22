#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_lib.h"

void play_dyn_strcpy(void)
{
    const char *orig = "Ahoj svet !";
    char *copy = dyn_strcpy(orig);

    printf("Original: %s\n", orig);
    printf("Copy: %s\n", copy);

    free(copy);
}

void play_dyn_strjoin(void)
{
    const char *ahoj = "Ahoj ";
    const char *svet = "svet!";
    char *result = dyn_strjoin(ahoj, svet);

    printf("Joined strings: \"%s\"\n", result);
    free(result);
}

void play_read_line(void)
{
    printf("Write something: ");
    char *line = read_line();
    printf("You have provided: \"%s\"\n", line);
    free(line);
}

static void str_printer(char **array, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        printf("%zu - %s\n", i + 1, array[i]);
    }
}

void play_split(void)
{
    const char *orig = "carrot, bannana, orange, kebab, mouse";
    size_t size;
    char **split = string_split(orig, ", ", &size);
    if (split == NULL)
        return;

    printf("Original string: %s\n", orig);
    printf("Splited:\n");
    str_printer(split, size);

    for (size_t i = 0; i < size + 1; i++) {
        free(split[i]);
    }

    free(split);
}

static void print_triangle(unsigned **matrix, size_t depth)
{
    if (matrix == NULL) {
        perror("No triangle provided!");
        return;
    }
    for (size_t row = 0; row < depth; row++) {
        for (size_t col = 0; col <= row; col++) {
            printf("%3u ", matrix[row][col]);
        }
        printf("\n");
    }
}

void play_triangle(void)
{
    unsigned **triangle = pascal_triangle(10);
    print_triangle(triangle, 10);
    dyn_free2d((void **) triangle, 10);
}

int main()
{
    play_dyn_strcpy();
    play_dyn_strjoin();
    play_read_line();

    // Bonuses:
    play_triangle();
    // play_split();

    return 0;
}
