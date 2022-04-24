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
        return NULL;
    }

    //if (a == '[') {
        //TODO - read until closing bracket
        // add a help function that reads the text inside the brackets
        // if it is an index, add a for loop to the recursive descent function?
        // if it is an attribute, add a condition
   // }

    if (a != '/' && a != 16 && a != EOF && a != '\n') { // 16 = data link escape, what the f is that
        parsing_error(state, "/ or EOF");
        return NULL;
    }

    return part;
}


void print_subtree(struct node* node, FILE *file)
{
    if (node == NULL) {
        return;
    }

    if (node->children == NULL) {
        fprintf(file, "%s\n", node->text);
    }
    else {
        for (size_t i = 0; i < vec_size(node->children); i++) {
            struct node **curr_child = vec_get(node->children, i);
            print_subtree(*curr_child, file);
        }
    }
}

void print_attributes(struct node* node)
{
    for (int i = 0; i < 2; i++) {
        fprintf(stdout, "%s=\"%s\"\n", node->key, node->value);
        node->key --;
        node->value--;
    }
}

void print_subtree_xml(struct node* node, FILE *file, size_t depth)
{
    if (node == NULL) {
        return;
    }
    for (size_t i = 0; i < depth; i++) {
        fprintf(file, "    ");
    }

    if (node->children == NULL) {
        fprintf(file,"<%s %s=\"%s\">", node->name, node->key, node->value);
        fprintf(file," %s", node->text);
        fprintf(file,"</%s>\n", node->name);
    }
    else {
        fprintf(file,"<%s>\n", node->name);
        for (size_t i = 0; i < vec_size(node->children); i++) {
            struct node **curr_child = vec_get(node->children, i);
            print_subtree_xml(*curr_child, file, depth + 1);
        }
        for (size_t i = 0; i < depth; i++) {
            fprintf(file, "    ");
        }
        fprintf(file, "</%s>\n", node->name);
    }
}

int tree_descent(struct parsing_state state, struct node* node, mchar* xpath, bool xml, FILE *file)
{
    xpath = get_xpath_part(&state);

    if (xpath == NULL) {
        if (state.error.code == PARSING_SUCCESS) {

            if (xml) {
                print_subtree_xml(node, file, 0);
            }
            else {
                print_subtree(node, file);
            }
            return 0;
        }
        else {
            fprintf(stderr, "%s\n", state.error.message);
            return -1;
        }
    }

    if (node->children != NULL) {
        for (size_t i = 0; i < vec_size(node->children); i++) {
            struct node** child = vec_get(node->children, i);
            if (strcmp(xpath, (*child)->name) == 0) {
                if (tree_descent(state, *child, xpath, xml, file) != 0) {
                    fprintf(stderr, "%s\n", state.error.message);
                    return -1;
                }
            }
        }
        str_destroy(xpath);
    }
    else {
        str_destroy(xpath);
        return 0;
    }

    return 0;
}

int descending(struct parsing_state state, struct node* node, bool xml, FILE *file)
{
    mchar* xpath = get_xpath_part(&state);
    if (xpath == NULL) {
        fprintf(stderr, "%s\n", state.error.message);
        return -1;
    }
    if (strcmp(xpath, node->name) == 0) {
        int rv = tree_descent(state, node, xpath, xml, file);
        str_destroy(xpath);
        return rv;
    }

    str_destroy(xpath);
    return 0;
}
