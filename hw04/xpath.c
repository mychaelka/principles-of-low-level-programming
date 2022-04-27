#include <ctype.h>
#include "xpath.h"

struct attribute {
    mchar* key;
    mchar* value;
};

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
        return NULL;
    }

    if (a == '[') {

        // INDICES
        if (isdigit(peek_char(state))) {
            *index = str_to_digit(parse_digit(state)); //TODO: number overflow
            if (*index == 0) {
                return_char(state);
                parsing_error(state, "nonzero digit");
                return NULL;
            }

            if (next_char(state) != ']') {
                parsing_error(state, "]");
                return NULL;
            }
            return part;
        }

        // ATTRIBUTES
        if (peek_char(state) != '@') {
            parsing_error(state, "@");
            return NULL;
        }
        next_char(state);
        attribute->key = parse_name(state);
        if (peek_char(state) != ']') {
            // IN THIS CASE, EQUAL SIGN HAS TO FOLLOW
            if (!parse_equals(state)) {
                return NULL;
            }

            if (next_char(state) != '"') {
                parsing_error(state, "\"");
                return NULL;
            }
            attribute->value = parse_name(state);
            if (next_char(state) != '"') {
                parsing_error(state, "\"");
                return NULL;
            }
            if (next_char(state) != ']') {
                parsing_error(state, "]");
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

int tree_descent(struct parsing_state state, struct node* node, mchar* xpath, bool xml, FILE *file)
{
    struct attribute attribute = {NULL, NULL};
    size_t index = 0;
    xpath = get_xpath_part(&state, &attribute, &index);

    if (xpath == NULL) {
        if (state.error.code == PARSING_SUCCESS) {

            if (xml) {
                print_subtree_xml(node, file, 0);
            } else {
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

                if (attribute_equals(attribute.key, attribute.value,
                                     (*child)->keys, (*child)->values)
                                     && (index == 0 || index == i + 1)) {
                    if (tree_descent(state, *child, xpath, xml, file) != 0) {
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
    struct attribute attribute = {NULL, NULL};
    size_t index = 0;
    mchar* xpath = get_xpath_part(&state, &attribute, &index);

    if (xpath == NULL) {
        parsing_error(&state, "letters or _");
        fprintf(stderr, "%s\n", state.error.message);
        return -1;
    }

    if (strcmp(xpath, node->name) == 0) {
        if (attribute_equals(attribute.key, attribute.value, node->keys, node->values)) {
            int rv = tree_descent(state, node, xpath, xml, file);
            str_destroy(xpath);
            return rv;
        }
    }

    str_destroy(xpath);
    return 0;
}
