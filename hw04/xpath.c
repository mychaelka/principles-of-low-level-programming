#include <ctype.h>
#include "xpath.h"

struct attribute {
    mchar* key;
    mchar* value;
};

bool check_beginning_xpath(struct parsing_state* state)
{
    return pattern_char(state, '/');
}

bool parse_xpath_index(struct parsing_state* state, size_t* index)
{
    mchar* digit = parse_digit(state);
    *index = str_to_digit(digit);
    if (*index == 0) {
        return_char(state);
        parsing_error(state, "nonzero digit");
        str_destroy(digit);
        return false;
    }

    if (next_char(state) != ']') {
        parsing_error(state, "]");
        str_destroy(digit);
        return false;
    }
    str_destroy(digit);
    return true;
}

bool parse_xpath_attr_value(struct parsing_state* state, struct attribute* attribute)
{
    if (!parse_equals(state)) {
        return false;
    }
    if (next_char(state) != '"') {
        parsing_error(state, "\"");
        return false;
    }

    attribute->value = parse_text(state, true);
    if (attribute->value == NULL) {
        return false;
    }
    if (next_char(state) != '"') {
        parsing_error(state, "\"");
        str_destroy(attribute->value);
        return false;
    }
    if (next_char(state) != ']') {
        parsing_error(state, "]");
        str_destroy(attribute->value);
        return false;
    }

    return true;
}

bool legal_end_of_attribute(struct parsing_state* state, int current)
{
    if (current == '/' && peek_char(state) == EOF) {  // string ended by slash - illegal
        parsing_error(state, "letters or _");
        return false;
    }
    if (current == EOF || current == '/') {  // after ']', there are only 2 legal options
        return true;
    }
    return false;
}

bool legal_end_of_index(struct parsing_state* state, int current)
{
    return legal_end_of_attribute(state, current);
}


bool parse_brackets(struct parsing_state* state, struct attribute* attribute, size_t* index)
{
    // INDICES
    if (isdigit(peek_char(state))) {
        if (!parse_xpath_index(state, index)) {
            return false;
        }
        int next = next_char(state);
        return legal_end_of_index(state, next);
    }

    // ATTRIBUTES
    if (next_char(state) != '@') {  // no other option
        parsing_error(state, "@");
        return false;
    }
    if (peek_char(state) == '*') {
        attribute->key = str_create("*");
        next_char(state);
    } else {
        attribute->key = parse_name(state);
    }
    if (attribute->key == NULL) {
        return false;
    }
    if (peek_char(state) != ']') {
        // in this case, equal sign has to follow
        if (!parse_xpath_attr_value(state, attribute)) {
            str_destroy(attribute->key);
            return false;
        }
        int next = next_char(state);
        if (legal_end_of_attribute(state, next)) {
            return true;
        }
        str_destroy(attribute->key);
        str_destroy(attribute->value);
        return false;
    }

    next_char(state);   // this must be ']'
    int next = next_char(state);
    if (legal_end_of_attribute(state, next)) {
        return true;
    }
    str_destroy(attribute->key);
    return false;
}


mchar* get_xpath_part(struct parsing_state* state, struct attribute* attribute, size_t* index)
{
    if (peek_char(state) == EOF) {
        return NULL;
    }

    mchar* part = NULL;
    if (peek_char(state) == '*') {
        part = str_create("*");
        next_char(state);
    } else {
        part = parse_name(state);
    }

    int next = next_char(state);
    if (part == NULL && next != EOF) {
        return NULL;
    }

    if (next == '/' && peek_char(state) == EOF) {  // string ended by slash - illegal
        parsing_error(state, "letters or _");
        str_destroy(part);
        return NULL;
    }

    if (next == '[') {
        if (!parse_brackets(state, attribute, index)) {
            str_destroy(part);
            return NULL;
        }
        return part;
    }

    if (next != '/' && next != EOF) {
        parsing_error(state, "/ or EOF");
        str_destroy(part);
        return NULL;
    }

    return part;
}

void print_attributes(struct node* node, FILE *file)
{
    if (node->keys != NULL && node->values != NULL) {
        for (size_t i = 0; i < vec_size(node->keys); i++) {
            mchar **key = vec_get(node->keys, i);
            mchar **value = vec_get(node->values, i);
            fprintf(file," %s=\"%s\"", *key, *value);
        }
    }
}

void print_final_node(struct node* node, FILE *file)
{
    fprintf(file,"<%s", node->name);
    print_attributes(node, file);
    fprintf(file, ">");
    fprintf(file," %s ", node->text);
    fprintf(file,"</%s>\n", node->name);
}

void print_subtree(struct node* node, FILE *file)
{
    if (node == NULL) {
        return;
    }

    if (node->children == NULL) {
        fprintf(file, "%s\n", node->text);
    } else {
        for (size_t i = 0; i < vec_size(node->children); i++) {
            struct node **curr_child = vec_get(node->children, i);
            print_subtree(*curr_child, file);
        }
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
        print_final_node(node, file);
    }
    else {
        fprintf(file,"<%s", node->name);
        print_attributes(node, file);
        fprintf(file, ">\n");
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

bool key_and_value_equal(const mchar* key, const mchar* value,
                           struct vector* keys, struct vector* values, size_t index)
{
    mchar** curr_key = vec_get(keys, index);
    mchar** curr_value = vec_get(values, index);
    return ((strcmp(key, *curr_key) == 0 || strcmp(key, "*") == 0)
             && strcmp(value, *curr_value) == 0);
}

bool attribute_equals(const mchar* key, const mchar* value,
                      struct vector* keys, struct vector* values) {
    assert((keys == NULL && values == NULL)
           || (vec_size(keys) == vec_size(values)));

    if (key == NULL && value == NULL) {
        return true;
    }

    if (key != NULL && value == NULL) {
        if (vec_empty(keys)) {
            return false;
        }
        if (strcmp(key, "*") == 0) {
            return true;
        }
        for (size_t i = 0; i < vec_size(keys); i++) {
            mchar** curr_key = vec_get(keys, i);
            if (strcmp(key, *curr_key) == 0) {
                return true;
            }
        }
        return false;
    }

    if (key != NULL) {
        if (keys == NULL || values == NULL) {
            return false;
        }
        for (size_t i = 0; i < vec_size(keys); i++) {
            if (key_and_value_equal(key, value, keys, values, i)) {
                return true;
            }
        }
        return false;
    }
    return false;
}

bool attribute_satisfies(mchar* xpath, mchar* name, struct attribute attribute,
        struct vector* keys, struct vector* values)
{
    return (strcmp(xpath, name) == 0 || strcmp(xpath, "*") == 0)
           && (attribute_equals(attribute.key, attribute.value, keys, values));
}

int tree_descent(struct parsing_state state, struct node* node, mchar* xpath, bool xml, FILE *file, struct vector* result)
{
    assert(result != NULL);
    struct attribute attribute = {NULL, NULL};
    size_t index = 0;
    xpath = get_xpath_part(&state, &attribute, &index);

    if (xpath == NULL) {
        if (state.error.code == PARSING_SUCCESS) {
            if (!vec_push_back(result, &node)) {
                str_destroy(attribute.key);
                str_destroy(attribute.value);
                return -1;
            }
            str_destroy(attribute.key);
            str_destroy(attribute.value);
            return 0;
        }
        else {
            fprintf(stderr, "%s\n", state.error.message);
            return -1;
        }
    }

    if (node->children != NULL) {
        size_t child_idx = 0;
        for (size_t i = 0; i < vec_size(node->children); i++) {
            struct node** child = vec_get(node->children, i);

            if (attribute_satisfies(xpath, (*child)->name, attribute,
                                     (*child)->keys, (*child)->values)) {
                child_idx += 1;
                if (index == 0 || index == child_idx) {
                    if (tree_descent(state, *child, xpath, xml, file, result) != 0) {
                        fprintf(stderr, "%s\n", state.error.message);
                        return -1;
                    }
                }
            }
        }
    }
    str_destroy(xpath);
    str_destroy(attribute.key);
    str_destroy(attribute.value);
    return 0;
}

int find_node(struct parsing_state state, struct node* node, bool xml, FILE *file)
{
    struct vector* result = vec_create(sizeof(struct node*));
    if (result == NULL) {
        return -1;
    }
    struct attribute attribute = {NULL, NULL};
    size_t index = 0;
    mchar* xpath = get_xpath_part(&state, &attribute, &index);

    if (xpath == NULL) {
        parsing_error(&state, "letters or _");
        fprintf(stderr, "%s\n", state.error.message);
        vec_destroy(result, DESTRUCTOR(node_ptr_destroy));
        return -1;
    }

    if (conditions_satisfied(xpath, node->name, attribute,
                             node->keys, node->values,
                             index, 1)) {
        if (tree_descent(state, node, xpath, xml, file, result) != 0) {
            free_vector(result);
            str_destroy(xpath);
            return -1;
        }

        if (vec_size(result) == 0) {
            free_vector(result);
            str_destroy(xpath);
            return 0;
        }
        if (xml && vec_size(result) > 1) {
            mchar *name = str_create("result");
            struct node* res = node_create(name, NULL, NULL, NULL, result);
            print_subtree_xml(res, file, 0);
            res->children = NULL;  // otherwise node_destroy would result in double free() error
            node_destroy(res);
        }

        for (size_t i = 0; i < vec_size(result); i++) {
            struct node **res = vec_get(result, i);
            if (xml) {
                print_subtree_xml(*res, file, 0);
            } else {
                print_subtree(*res, file);
            }
            *res = NULL;
        }
    }
    free_vector(result);
    str_destroy(xpath);
    return 0;
}
