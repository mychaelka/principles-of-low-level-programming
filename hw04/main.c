#include <getopt.h>
#include <string.h>

#include "xpath.h"
#include "xparser.h"
#include "vector.h"

struct arguments
{
    char *input_file;
    char *output_file;
    char *xpath;
    bool input_specified;
    bool output_specified;
    bool xpath_specified;
    bool plain_text;
    bool xml;
};

int load_arguments(int argc, char **argv, struct arguments *args)
{
    for (int opt = 1; opt < argc; opt++) {
        if (args->xpath_specified) {
            fprintf(stderr, "Invalid arguments after XPATH\n");
            return -1;
        }
        if (argv[opt][0] != '-') {  // this must be XPATH
            args->xpath = argv[opt];
            args->xpath_specified = true;
            continue;
        }

        if (strcmp(argv[opt], "-i") == 0 || strcmp(argv[opt], "--input") == 0) {
            if (args->input_specified) {
                fprintf(stderr, "Only one input file can be specified\n");
                return -1;
            }
            if (opt + 1 == argc) {
                fprintf(stderr, "Option '--input' requires an argument\n");
                return -1;
            }
            args->input_file = argv[opt + 1];
            args->input_specified = true;
            opt++;
            continue;
        }

        if (strcmp(argv[opt], "-o") == 0 || strcmp(argv[opt], "--output") == 0) {
            if (args->output_specified) {
                fprintf(stderr, "Only one output file can be specified\n");
                return -1;
            }
            if (opt + 1 == argc) {
                fprintf(stderr, "Option '--output' requires an argument\n");
                return -1;
            }
            args->output_file = argv[opt + 1];
            args->output_specified = true;
            opt++;
            continue;
        }

        if (strcmp(argv[opt], "-t") == 0 || strcmp(argv[opt], "--text") == 0) { //x and t options can be used multiple times
            if (args->xml) {
                fprintf(stderr, "Options '--text' and '--xml' are incompatible\n");
                return -1;
            }
            args->plain_text = true;
            continue;
        }
        if (strcmp(argv[opt], "-x") == 0 || strcmp(argv[opt], "--xml") == 0) {
            if (args->plain_text) {
                fprintf(stderr, "Options '--text' and '--xml' are incompatible\n");
                return -1;
            }
            args->xml = true;
            continue;
        }

        fprintf(stderr, "Invalid option '%s'\n", argv[opt]);
        return -1;
    }

    if (!args->xpath_specified) {
        fprintf(stderr, "Positional argument XPATH required\n");
        return -1;
    }
    return 0;
}


int main(int argc, char **argv)
{
    struct arguments args = {"", "", "",
            false, false, false, false, false};

    if (load_arguments(argc, argv, &args) != 0) {
        return EXIT_FAILURE;
    }

    if (write_xpath_to_file("xpath.txt", args.xpath) != 0) {
        fprintf(stderr, "Could not write xpath to file\n");
        return EXIT_FAILURE;
    }

    //FILE *path = fopen("xpath.txt", "r");
    //struct file_generator gen = { path };
    //struct parsing_state state = parsing_state_init(&gen, file_fill);

    struct str_generator gen_str = { args.xpath, strlen(args.xpath) };
    struct parsing_state state = parsing_state_init(&gen_str, str_fill);

    if (!check_beginning_xpath(&state)) {
        fprintf(stderr, "%s\n", state.error.message);
        return EXIT_FAILURE;
    }

    FILE *in = stdin;

    if (args.input_specified) {
        in = fopen(args.input_file, "r");
        if (in == NULL) {
            fprintf(stderr, "Could not open file '%s'\n", args.input_file);
            return EXIT_FAILURE;
        }
    }

    struct node *node = parse_xml(in);
    if (node == NULL) {
        fclose(in);
        return EXIT_FAILURE;
    }
    fclose(in);

    if (args.output_specified) {
        FILE *out = fopen(args.output_file, "w");
        if (out == NULL) {
            fprintf(stderr, "Could not open file '%s'\n", args.output_file);
            node_destroy(node);
            return EXIT_FAILURE;
        }

        if (descending(state, node, args.xml, out) != 0) {
            node_destroy(node);
            fclose(out);
            return EXIT_FAILURE;
        }
        fclose(out);

    } else {
        if (descending(state, node, args.xml, stdout) != 0) {
            node_destroy(node);
            return EXIT_FAILURE;
        }
    }

    node_destroy(node);
    return EXIT_SUCCESS;
}
