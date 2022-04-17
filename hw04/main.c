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

    FILE *path = fopen("xpath.txt", "r");
    struct parsing_state state = read_xpath(path);
    fclose(path);

    if (args.input_specified) {
        FILE *in = fopen(args.input_file, "r");
        if (in == NULL) {
            fprintf(stderr, "Could not open file '%s'\n", args.input_file);
            return EXIT_FAILURE;
        }

        struct node *bookstore = parse_xml(in);
        fclose(in);

        struct node *try_node = find_element(state, bookstore, args.xpath);
        printf("%s\n", try_node->name);




        printf("%s\n", bookstore->name);

        struct node **book = vec_get(bookstore->children, 2);
        printf("Books: %lu\n", vec_size(bookstore->children));
        printf("%s\n", (*book)->key);
        printf("%s\n", (*book)->value);
        struct node **title = vec_get((*book)->children, 3);
        printf("%s\n", (*title)->text);
        printf("XPATH: %s\n", args.xpath);

        struct node *result = node_create("result", NULL, NULL, NULL, NULL);
        if (result == NULL) {
            fprintf(stderr, "Could not create result node\n");
            return EXIT_FAILURE;
        }
    }

    if (args.output_specified) {
        FILE *out = fopen(args.output_file, "w");
        if (out == NULL) {
            fprintf(stderr, "Could not open file '%s'\n", args.output_file);
            return EXIT_FAILURE;
        }
        fclose(out);
    }

    return EXIT_SUCCESS;
}
