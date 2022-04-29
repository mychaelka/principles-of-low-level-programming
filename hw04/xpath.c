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

mchar* get_xpath_part(struct parsing_state* state, struct attribute* attribute, size_t* index)
{
    if (peek_char(state) == EOF) {
        return NULL;
    }

    mchar* part = parse_name(state);
    int a = next_char(state);

    if (part == NULL && a != EOF) {
        return NULL;
    }

    if (a == '/' && peek_char(state) == EOF) {  // string ended by slash - illegal
        parsing_error(state, "letters or _");
        str_destroy(part);
        return NULL;
    }

    if (a == '[') {

        // INDICES
        if (isdigit(peek_char(state))) {
            mchar* digit = parse_digit(state);
            *index = str_to_digit(digit); //TODO: number overflow
            if (*index == 0) {
                return_char(state);
                parsing_error(state, "nonzero digit");
                str_destroy(digit);
                str_destroy(part);
                return NULL;
            }

            if (next_char(state) != ']') {
                parsing_error(state, "]");
                str_destroy(digit);
                str_destroy(part);
                return NULL;
            }
            str_destroy(digit);
            return part;
        }

        // ATTRIBUTES
        if (peek_char(state) != '@') {
            parsing_error(state, "@");
            str_destroy(part);
            return NULL;
        }
        next_char(state);
        attribute->key = parse_name(state);
        if (peek_char(state) != ']') {
            // IN THIS CASE, EQUAL SIGN HAS TO FOLLOW
            if (!parse_equals(state)) {
                str_destroy(part);
                return NULL;
            }

            if (next_char(state) != '"') {
                parsing_error(state, "\"");
                str_destroy(part);
                return NULL;
            }
            attribute->value = parse_name(state);
            if (next_char(state) != '"') {
                parsing_error(state, "\"");
                str_destroy(part);
                return NULL;
            }
            if (next_char(state) != ']') {
                parsing_error(state, "]");
                str_destroy(part);
                return NULL;
            }
            return part;
        }
        else {
            next_char(state);
            if (peek_char(state) == EOF) {
                return part;
            }
        }
    }

    if (a != '/' && a != EOF && a != '\n') {
        parsing_error(state, "/ or EOF");
        str_destroy(part);
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

void print_subtree_xml(struct node* node, FILE *file, size_t depth)
{
    if (node == NULL) {
        return;
    }
    for (size_t i = 0; i < depth; i++) {
        fprintf(file, "    ");
    }

    if (node->children == NULL) {
        fprintf(file,"<%s", node->name);
        print_attributes(node, file);
        fprintf(file, ">");
        fprintf(file," %s ", node->text);
        fprintf(file,"</%s>\n", node->name);
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

bool attribute_equals(const mchar* key, const mchar* value, struct vector* keys, struct vector* values) {
    assert((keys == NULL && values == NULL) || (vec_size(keys) == vec_size(values)));

    if (key == NULL && value == NULL) {
        return true;
    }

    if (key != NULL && value == NULL) {
        if (keys == NULL) {
            return false;
        }
        for (size_t i = 0; i < vec_size(keys); i++) {
            mchar** curr_key = vec_get(keys, i);
            if (strcmp(key, *curr_key) == 0) {
                return true;
            }
        }
        return false;
    }

    if (key != NULL && value != NULL) {
        if (keys == NULL || values == NULL) {
            return false;
        }
        for (size_t i = 0; i < vec_size(keys); i++) {
            mchar** curr_key = vec_get(keys, i);
            mchar** curr_value = vec_get(values, i);
            if (strcmp(key, *curr_key) == 0 && strcmp(value, *curr_value) == 0) {
                return true;
            }
        }
        return false;
    }

    return false;
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
                return -1;
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

                if (attribute_equals(attribute.key, attribute.value,
                                     (*child)->keys, (*child)->values)
                                     && (index == 0 || index == i + 1)) {
                    if (tree_descent(state, *child, xpath, xml, file, result) != 0) {
                        fprintf(stderr, "%s\n", state.error.message);
                        return -1;
                    }
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

    if (strcmp(xpath, node->name) == 0) {
        if (attribute_equals(attribute.key, attribute.value, node->keys, node->values)) {
            if (tree_descent(state, node, xpath, xml, file, result) == 0) {
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
                else if (xml){
                    struct node **res = vec_get(result, 0);
                    print_subtree_xml(*res, file, 0);
                    *res = NULL;
                }
                else {
                    for (size_t i = 0; i < vec_size(result); i++) {
                        struct node **res = vec_get(result, i);
                        print_subtree(*res, file);
                        *res = NULL;
                    }
                }
                free_vector(result);
                str_destroy(xpath);
                return 0;
            }

            free_vector(result);
            str_destroy(xpath);
            return -1;
        }
    }

    free_vector(result);
    str_destroy(xpath);
    return 0;
}
