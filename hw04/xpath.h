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

bool check_beginning_xpath(struct parsing_state* state);

int find_node(struct parsing_state state, struct node* node, bool xml, FILE *file);

void print_attributes(struct node* node, FILE *file);

#endif /* XPATH_H */
