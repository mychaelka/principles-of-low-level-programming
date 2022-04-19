#include "xpath.h"

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


void take_while_delim(char * xpath)
{
    char * delim = "/";
    char *ptr = strtok(xpath, delim);

    while(ptr != NULL) { // puts \0 instead of delimiter everywhere
        ptr = strtok(NULL, delim);
    }
}

void print_subtree(struct node* node)
{
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


void tree_descent(struct node* node, char * xpath, size_t xpath_len, size_t curr_idx)
{
    curr_idx += strlen(xpath) + 1;

    if (curr_idx >= xpath_len) {
        print_subtree(node);
    }

    size_t elem_size = strlen(xpath);
    xpath += (elem_size + 1);


    if (node->children != NULL) {
        for (size_t i = 0; i < vec_size(node->children); i++) {
            struct node** child = vec_get(node->children, i);

            if (strcmp(xpath, (*child)->name) == 0) {
                tree_descent(*child, xpath, xpath_len, curr_idx);
            }
        }
    }
}
