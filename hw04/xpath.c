#include "xpath.h"
#include "xparser.h"
#include "vector.h"
#include "parser.h"

int parse_xpath(const char *xpath)
{
    if (xpath[0] != '\\') {
        fprintf(stderr, "Invalid path");
        return -1;
    }

    return 0;
}

int write_xpath_to_file(const char* filename, const char* xpath)
{
    FILE *out = fopen(filename, "w");
    if (xpath == NULL) {
        fprintf(stderr, "Could not write xpath\n");
        return -1;
    }
    fprintf(out, "%s\n", xpath);
    fclose(out);
    return 0;
}

struct parsing_state read_xpath(FILE *file)
{
    struct file_generator gen = { file };
    struct parsing_state state = parsing_state_init(&gen, file_fill);

    return state;
}

static int delim(int c)
{
    return c != '/';
}

static int start(int c)
{
    return c == '/';
}

struct node* find_element(struct parsing_state state, struct node* xml, char *xpath)
{
    struct node *result = node_create("result", NULL, NULL, NULL, NULL);

    struct vector *children = vec_create(sizeof(struct node *));
    if (children == NULL) {
        return NULL;
    }

    mchar *element = str_create("");
    size_t i = 1;
    while (xpath[i] != '/') {
        str_add_char(&element, xpath[i]);
        i++;
    }

    printf("THIS IS THE ELEMENT: %s\n", element);

    return result;
}