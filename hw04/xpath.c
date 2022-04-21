#include "xpath.h"
#include <ctype.h>

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

struct parsing_state read_xpath_from_file(FILE *file)
{
    struct file_generator gen = { file };
    struct parsing_state state = parsing_state_init(&gen, file_fill);
    return state;
}


bool check_beginning_xpath(struct parsing_state* state)
{
    return pattern_char(state, '/');
}

struct parsing_state read_xpath(const char *xpath)
{
    struct str_generator gen = { xpath, strlen(xpath) };
    struct parsing_state state = parsing_state_init(&gen, str_fill);

    return state;
}


mchar* get_xpath_part(struct parsing_state* state)
{
    if (peek_char(state) == EOF) {
        return NULL;
    }
    mchar* part = parse_name(state);

    int a = next_char(state);

    if (part == NULL && a != EOF) {
        fprintf(stderr, "%s\n", state->error.message);
        return NULL;
    }

    if (a != '/' && a != 16 && a != EOF && a != '\n') { // 16 = data link escape, what the f is that
        parsing_error(state, "/ or EOF");
        return NULL;
    }

    return part;
}


void print_subtree(struct node* node)
{
    if (node == NULL) {
        return;
    }

    if (node->children == NULL) {
        printf("%s\n", node->text);
    }
    else {
        for (size_t i = 0; i < vec_size(node->children); i++) {
            struct node **curr_child = vec_get(node->children, i);
            print_subtree(*curr_child);
        }
    }
}

void tree_descent(struct parsing_state state, struct node* node, mchar* xpath)
{
    xpath = get_xpath_part(&state);

    if (xpath == NULL) {
        if (state.error.code == PARSING_SUCCESS) {
            print_subtree(node);
            return;
        }
        else {
            return;
        }
    }

    if (node->children != NULL) {
        for (size_t i = 0; i < vec_size(node->children); i++) {
            struct node** child = vec_get(node->children, i);
            if (strcmp(xpath, (*child)->name) == 0) {
                tree_descent(state, *child, xpath);
            }
        }
        str_destroy(xpath);
    }
    else {
        str_destroy(xpath);
        return;
    }
}

void descending(struct parsing_state state, struct node* node)
{
    mchar* xpath = get_xpath_part(&state);
    if (xpath == NULL) {
        return;
    }
    if (strcmp(xpath, node->name) == 0) {
        tree_descent(state, node, xpath);
    }

    str_destroy(xpath);
}
