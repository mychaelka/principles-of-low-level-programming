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

void print_tree_text(struct node* node, char * xpath, size_t xpath_len, size_t curr_idx)
{
    curr_idx += strlen(xpath) + 1;

    //printf("xpath_len: %lu\n", xpath_len);
    //printf("curr_idx: %lu\n", curr_idx);
    //printf("xpath: %s\n", xpath);

    if (curr_idx >= xpath_len) {
        if (node->children != NULL) {
            for (size_t i = 0; i < vec_size(node->children); i++) {
                struct node **curr_child = vec_get(node->children, i);
                printf("%s\n", (*curr_child)->text);
            }
        } else {
            printf("%s\n", node->text);
        }
        return;
    }

    while (*xpath != '\0' && *xpath != EOF) {
        xpath++;
    }
    if (*xpath == '\0') {
        xpath++;
    }

    struct node** child = NULL;
    if (node->children != NULL) {
        for (size_t i = 0; i < vec_size(node->children); i++) {
            child = vec_get(node->children, i);

            if (strcmp(xpath, (*child)->name) == 0) {
                print_tree_text(*child, xpath, xpath_len, curr_idx);
            }
        }
    }
}
