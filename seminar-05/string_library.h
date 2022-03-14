#include <stdlib.h>
#include <stddef.h>

#ifndef STRING_LIBRARY_H
#define STRING_LIBRARY_H

/*
 * Get the string length
 * Fails if the string is NULL.
 * 
 * @param str - Input string
 * @return Length of string
 */
size_t string_length(const char *str);

/**
 * Creates string copy
 * 
 * @param[out] result - Where copy will be stored
 * @param original - From what string you will create copy
 * @return pointer to copied string; or NULL if any of inputs is NULL
 */
char *string_copy(char *result, const char *original);

/**
 * Counts chars in string
 * Fails if the string is NULL.
 * 
 * @param string - Input string where you will count chars
 * @param ch - Char to be count
 * @return Number of occurrences of character ch; if ch is 0, returns 0
 */
size_t string_count_char(const char *string, char ch);

/* COMPLEX */

/**
 * Counts substrings in string
 * Fails if either original or substring is NULL
 * 
 * @param original - Input string where you counting substrings
 * @param substring - Substring to be count
 * @return Number of substrings
 */
size_t string_count_substr(const char *original, const char *substring);

/**
 * Counts number of occurences of the string in the strings array
 * Fails if either array or string is NULL
 *
 * @param array - array of strings
 * @param string - string we are trying to find
 */

size_t string_count_string_in_array(const char *array[], const char *string);

/**
 * Splits given string and result puts into result matrix
 * Fails if input string is NULL
 * 
 * @param original - Input string
 * @param[out] result - Result matrix which has fixed 50 rows and 256 cols
 * @param[out] size - Number of rows of result matrix which will be used
 * @param delim - Character by which will be split original string. Result
 * substring will not contain this character.
 */
void string_split(const char *original, char result[50][256], int *size, char delim);

#endif
