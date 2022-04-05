#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"

#define UNUSED(VAR) \
    ((void) (VAR))

const int INVALID_SEARCH = -1;

/**
 * Searches for \p search in each line of \p in and replaces it with
 * stars. The result is written into \p out.
 *
 * \param   search      string to censor
 * \param   in          input file
 * \param   out         output file
 * \return  0 on success, an arbitrary error code on failure
 */
int censor(const char *search, FILE *in, FILE *out)
{
    char *line;
    size_t idx = 0;
    size_t len_search = strlen(search);

    if (len_search == 0) {
        return INVALID_SEARCH;
    }

    while ((line = readline(in)) != NULL) {
        while (line[idx] != '\0') {
            if (strstr(&line[idx], search) == &line[idx]) {
                memset(&line[idx], '*', len_search);
                idx += len_search - 1;
            } else {
                idx++;
            }
        }
        fputs(line, out);
        free(line);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s STRING SOURCE DESTINATION\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *in = fopen(argv[2], "r");

    if (in == NULL) {
        fputs("Cannot open file", stderr);
        return EXIT_FAILURE;
    }

    FILE *out = fopen(argv[3], "w");
    if (out == NULL) {
        fclose(in);
        fputs("Cannot open file", stderr);
        return EXIT_FAILURE;
    }

    if (censor(argv[1], in, out) != 0) {
        fputs("An error occured", stderr);
        fclose(in);
        fclose(out);
        return EXIT_FAILURE;
    }

    fclose(in);
    fclose(out);
    return EXIT_SUCCESS;
}
