#include <getopt.h>
#include <string.h>

#include "xpath.h"

struct arguments
{
    char *input_file;
    char *output_file;
    bool plain_text;
    bool xml;
};

int load_arguments(int argc, char **argv, struct arguments *args)
{
    for (int opt = 1; opt < argc; opt++) {
        if (strcmp(argv[opt], "-i") == 0 || strcmp(argv[opt], "--input") == 0) {
            args->input_file = argv[opt + 1];
            opt++;
        }

        if (strcmp(argv[opt], "-o") == 0 || strcmp(argv[opt], "--output") == 0) {
            args->output_file = argv[opt + 1];
            opt++;
        }

        if (strcmp(argv[opt], "-t") == 0 || strcmp(argv[opt], "--text") == 0) { //x and t options can be used multiple times
            if (args->xml) {
                fprintf(stderr, "Options '--text' and '--xml' are incompatible\n");
                return -1;
            }
            args->plain_text = true;
        }
        if (strcmp(argv[opt], "-x") == 0 || strcmp(argv[opt], "--xml") == 0) {
            if (args->plain_text) {
                fprintf(stderr, "Options '--text' and '--xml' are incompatible\n");
                return -1;
            }
            args->xml = true;
        }

    }
    return 0;
}

int main(int argc, char **argv)
{
    struct arguments args = {"", "", false, false};

    if (load_arguments(argc, argv, &args) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
