#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

struct node* find_element(struct parsing_state state, struct node* xml, char *xpath);

#endif /* XPATH_H */
