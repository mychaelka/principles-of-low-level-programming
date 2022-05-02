#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "string_library.h"

void play_split()
{
    const char *original = "Ahoj svet 000 Query";
    char result[50][256];
    int size;
    char delim = ' ';

    string_split(original, result, &size, delim);
    printf("SPLIT: \nSize: %d\nParts: \n", size);
    for (int i = 0; i < size; i++) {
        printf("   %d. - %s\n", i + 1, result[i]);
    }
}

size_t string_length(const char *str)
{
    size_t length = 0;
    while (*str != '\0') {
        length += 1;
        str++;
    }
    return length;
}

char *string_copy(char *result, const char *original)
{
    int length = 0;

    if (original == NULL) {
        return NULL;
    }
    while (*original != '\0') {
        *result = *original;
        original++;
        result++;
        length += 1;
    }
    result++;
    *result = '\0';
    return result - length;
}

size_t string_count_char(const char *string, char ch)
{
    int counter = 0;

    if (ch == '\0') {
        return 0;
    }

    while (*string != '\0') {
        if (*string == ch) {
            counter += 1;
        }
        string++;
    }

    return counter;
}

size_t string_count_substr(const char *original, const char *substring)
{

    if (*original == '\0' || *substring == '\0') {
        return 0;
    }

    size_t counter = 0;
    const char *current = original;

    while ((current = strstr(current, substring)) != NULL) {
        counter++;
        current++;
    }

    return counter;
}

size_t string_count_string_in_array(const char *array[], const char *string)
{
    size_t counter = 0;
    while (*array != NULL) {
        if (strcmp(*array, string) == 0) {
            counter += 1;
        }
        array++;
    }

    return counter;
}

void string_split(const char *string, char result[50][256], int *size, char delim)
{
    int max_chars = 255;
    int max_substrings = 50;
    int substring = 0;
    int chars = 0;
    while (*string != '\0' && (substring < max_substrings)) {
        while (*string != delim && chars < max_chars) {
            result[substring][chars] = *string;
            chars += 1;
            string++;
        }
        result[substring][chars + 1] = '\0';
        *size += 1;
        string++;
        substring += 1;
        chars = 0;

    }
}

int main(void)
{
    puts("PlayGround!");
    play_split();

    size_t length = string_length("Hello world!");
    printf("Length: %zu\n", length);
    assert(length == 12);

    char buffer[7];
    string_copy(buffer, "Hello!");
    assert(strcmp(buffer, "Hello!") == 0);

    size_t count_a = string_count_char("abba123 a", 'a'); // will return 3
    assert(count_a == 3);

    size_t substrings = string_count_substr("abba123 abba abba", "abba");
    //assert(substrings == 3);

    char *const strings1[] = { "hello", "hi", 0 };
    size_t str1 = string_count_string_in_array(strings1, "hello");
    assert(str1 == 1);
    return 0;




}
