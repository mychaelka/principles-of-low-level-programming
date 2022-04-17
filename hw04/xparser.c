#include "parser.h"
#include "vector.h"
#include "xparser.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*****************************************************************************
 *  DATA STRUCTURES
 *****************************************************************************/

struct node *node_create(mchar *name, mchar *key, mchar *value,
                         mchar *text, struct vector *children)
{
    assert(name != NULL);
    assert(text == NULL || children == NULL);

    struct node *node = malloc(sizeof(struct node));
    if (node != NULL) {
        *node = (struct node) {
            .name = name,
            .text = text,
            .key = key,
            .value = value,
            .children = children
        };
    }

    return node;
}

void node_destroy(struct node *node)
{
    if (node == NULL) {
        return;
    }

    str_destroy(node->name);
    str_destroy(node->text);
    str_destroy(node->key);
    str_destroy(node->value);

    vec_destroy(node->children, DESTRUCTOR(node_ptr_destroy));

    free(node);
}


/*****************************************************************************
 *  MISCELLANEOUS
 *****************************************************************************/

static bool add_while(struct parsing_state *state, mchar **str,
                      predicate predicate)
{
    assert(state != NULL);
    assert(predicate != NULL);
    assert(str != NULL);
    assert(*str != NULL);

    while(predicate(peek_char(state))) {
        if (!str_add_char(str, next_char(state))) {
            return false;
        }
    }

    return true;
}

mchar *parse_string(struct parsing_state *state, predicate valid_chars)
{
    assert(state != NULL);
    assert(valid_chars != NULL);

    mchar *string = str_create("");
    if (string != NULL && add_while(state, &string, valid_chars)) {
        return string;
    }

    alloc_error(state, "string");
    free(string);

    return NULL;
}

mchar *parse_word(struct parsing_state *state, predicate start,
                  predicate rest, const char *error_message)
{
    assert(state != NULL);
    assert(start != NULL);
    assert(rest != NULL);
    assert(error_message != NULL);

    if (!start(peek_char(state))) {
        parsing_error(state, error_message);
        return NULL;
    }

    mchar *word = str_create(CHAR_TO_STR(next_char(state)));
    if (word != NULL && add_while(state, &word, rest)) {
        return word;
    }

    alloc_error(state, "word");
    free(word);

    return NULL;
}


/*****************************************************************************
 *  NAME
 *****************************************************************************/

static int name_start(int c)
{
    return c == '_' || isalpha(c);
}

static int name_rest(int c)
{
    return strchr("_:.-", c) != NULL || isdigit(c) || isalpha(c);
}

// checks if the name is valid (starts with _ or letter and contains valid characters,
// if not, returns error. Returns the parsed name (mchar is typedef for char)
mchar *parse_name(struct parsing_state *state)
{
    assert(state != NULL);

    return parse_word(state, name_start, name_rest, "letters or _");
}


/*****************************************************************************
 *  TEXT
 *****************************************************************************/

static bool end_of_text(int c)
{
    return c == EOF || c == '<' || c == '"';
}

static bool add_to_text(struct parsing_state *state, mchar **text,
                        bool *space, bool normalize)
{
    assert(state != NULL);
    assert(text != NULL);

    int c = peek_char(state);

    if (normalize) {
        if (isspace(c)) {
            *space = true;
            next_char(state);
            return true;
        }

        if (*space && !str_add_char(text, ' ')) {
            return false;
        }

        *space = false;
    }

    return str_add_char(text, next_char(state));
}

mchar *parse_text(struct parsing_state *state, bool normalize)
{
    assert(state != NULL);

    if (normalize) {
        read_spaces(state, 0);
    }

    mchar *text = str_create("");
    if (text == NULL) {
        alloc_error(state, "text");
        return NULL;
    }

    bool space = false;
    while (!end_of_text(peek_char(state))) {
        if (!add_to_text(state, &text, &space, normalize)) {
            alloc_error(state, "text");
            str_destroy(text);
            return NULL;
        }
    }

    return text;
}


/*****************************************************************************
 *  VALUE
 *****************************************************************************/

bool parse_equals(struct parsing_state *state)
{
    assert(state != NULL);

    return read_spaces(state, 0)
        && pattern_char(state, '=')
        && read_spaces(state, 0);
}

mchar *parse_value(struct parsing_state *state)
{
    assert(state != NULL);

    if (!pattern_char(state, '"')) {
        return NULL;
    }

    mchar *value = parse_text(state, false);
    if (value == NULL) {
        return NULL;
    }

    if (!pattern_char(state, '"')) {
        str_destroy(value);
        return NULL;
    }

    return value;
}


/*****************************************************************************
 *  ATTRIBUTES
 *****************************************************************************/

static bool parse_attribute(struct parsing_state *state,
                            mchar **key, mchar **value)
{
    assert(state != NULL);

    *key = parse_name(state);
    if (*key == NULL) {
        return false;
    }

    bool success = parse_equals(state);
    success = success && (*value = parse_value(state)) != NULL;

    if (!success) {
        str_destroy(*key);
    }

    return success;
}

static bool end_of_attributes(struct parsing_state *state)
{
    assert(state != NULL);

    const int c = peek_char(state);
    return c == EOF || c == '/' || c == '>' || c == '<';
}

bool parse_attributes(struct parsing_state *state, mchar **key, mchar **value)
{
    assert(state != NULL);

    if (end_of_attributes(state)) {
        return true;
    }

    const bool whitespace = read_spaces(state, 1);
    if (end_of_attributes(state)) {
        return true;
    }

    return whitespace && parse_attribute(state, key, value);
}


/*****************************************************************************
 *  XNODE
 *****************************************************************************/

static bool check_end_tag(struct parsing_state *state, const mchar *name)
{
    return pattern_str(state, "</")
        && read_spaces(state, 0)
        && pattern_str(state, name)
        && read_spaces(state, 0)
        && pattern_char(state, '>');
}

static struct node *parse_content(struct parsing_state *state, mchar *name,
                                  mchar *key, mchar *value, bool is_empty)
{
    mchar *text = NULL;
    struct vector *children = NULL;

    if (!is_empty) {
        bool is_text = next_char(state) != '<' || peek_char(state) == '/';
        return_char(state);

        if (is_text) {
            text = parse_text(state, true);
        } else {
            children = parse_xnodes(state);
        }
    }

    const bool parsed_content = is_empty || text != NULL || children != NULL;
    if (parsed_content && (is_empty || check_end_tag(state, name))) {
        struct node *node = node_create(name, key, value, text, children);
        if (node != NULL) {
            return node;
        }
    }

    str_destroy(text);
    vec_destroy(children, DESTRUCTOR(node_ptr_destroy));

    return NULL;
}

static bool parse_tag(struct parsing_state *state, mchar **name,
                      mchar **key, mchar **value, bool *is_empty)
{
    if (!pattern_char(state, '<') || !read_spaces(state, 0)) {
        return false;
    }

    if ((*name = parse_name(state)) == NULL) {
        return false;
    }

    if (!parse_attributes(state, key, value)) {
        str_destroy(*name);
        return false;
    }

    read_spaces(state, 0);
    *is_empty = accept_char(state, '/');

    if (pattern_char(state, '>') && read_spaces(state, 0)) {
        return true;
    }

    str_destroy(*name);
    str_destroy(*key);
    str_destroy(*value);

    return false;
}

struct node *parse_xnode(struct parsing_state *state)
{
    read_spaces(state, 0);

    mchar *name = NULL;
    mchar *key = NULL;
    mchar *value = NULL;

    bool is_empty = true;
    if (!parse_tag(state, &name, &key, &value, &is_empty)) {
        return NULL;
    }

    struct node *node = parse_content(state, name, key, value, is_empty);
    if (node != NULL) {
        read_spaces(state, 0);
        return node;
    }

    str_destroy(name);
    str_destroy(key);
    str_destroy(value);

    return NULL;
}

void node_ptr_destroy(struct node **node)
{
    assert(node != NULL);

    node_destroy(*node);
}

struct vector *parse_xnodes(struct parsing_state *state)
{
    // creates empty vector to which children should be stored
    struct vector *children = vec_create(sizeof(struct node *));
    if (children == NULL) {
        return NULL;
    }

    // reads whitespace characters from buffer
    read_spaces(state, 0);

    while (true) {
        // children begin
        if (accept_char(state, '<')) {
            const int next = peek_char(state);
            return_char(state); // shifts back left in buffer so that we do not move forward in reading
            // If next character is '/', we know that this is an end tag,
            // so there are no children → return empty vector
            if (next == '/') {
                break;
                //return children; // next character after opening tag is slash, so it means end of node
            }

            struct node *child = parse_xnode(state);

            if (child != NULL) {
                if (vec_push_back(children, &child)) {
                    continue;
                    //return children;
                }

                alloc_error(state, "adding child node to vector of nodes");
            }

            vec_destroy(children, DESTRUCTOR(node_ptr_destroy));
            return NULL;
        }
    }
    read_spaces(state, 0);

    return children;
}


/*****************************************************************************
 *  XML
 *****************************************************************************/

struct node *parse_root(struct parsing_state *state)
{
    struct node *root = parse_xnode(state);
    if (root == NULL) {
        return NULL;
    }

    if (peek_char(state) == EOF) {
        return root;
    }

    parsing_error(state, "EOF");
    node_destroy(root);

    return NULL;
}

struct node *parse_xml(FILE *file)
{
    struct file_generator gen = { file };
    struct parsing_state state = parsing_state_init(&gen, file_fill);

    struct node *root = parse_root(&state);
    if (root == NULL) {
        print_error(&state, stderr);
    }

    return root;
}
