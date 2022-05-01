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

struct attribute;

int write_xpath_to_file(const char* filename, const char* xpath);

struct parsing_state read_xpath_from_file(FILE *file);

bool check_beginning_xpath(struct parsing_state* state);

struct parsing_state read_xpath(const char* xpath);

int tree_descent(struct parsing_state state, struct node* node, mchar* xpath, bool xml, FILE *file, struct vector* result);

int find_node(struct parsing_state state, struct node* node, bool xml, FILE *file);

void print_attributes(struct node* node, FILE *file);

#endif /* XPATH_H */
