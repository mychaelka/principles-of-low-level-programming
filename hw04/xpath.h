#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "xparser.h"
#include "vector.h"
#include "parser.h"
#include "managed_string.h"

#ifndef XPATH_H
#define XPATH_H

typedef char* attribute;

typedef attribute* attributes;

struct element {
    char * name;
    int index;
    attribute attribute;
    char * attribute_value;
};

int write_xpath_to_file(const char* filename, const char* xpath);

struct parsing_state read_xpath(FILE *file);

void take_while_delim(char * xpath);

void print_tree_text(struct node* node, char * xpath, size_t xpath_len, size_t curr_idx);

#endif /* XPATH_H */
