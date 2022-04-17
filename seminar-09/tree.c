#include "tree.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t maximum(size_t a, size_t b)
{
    return a > b ? a : b;
}

static void node_destroy(struct node *node)
{
    if (node == NULL)
        return;

    node_destroy(node->left);
    node_destroy(node->right);
    free(node);
}

static struct node *node_create(size_t size)
{
    assert(size > 0);

    struct node *node = (struct node *) malloc(sizeof(struct node));
    if (node == NULL) {
        perror("node_create: malloc");
        return NULL;
    }
    node->value = rand() % 20;
    node->left = NULL;
    node->right = NULL;

    size_t right_part = (size - 1) / 2;       // this won't underflow as size > 0
    size_t left_part = size - 1 - right_part; // this shouldn't underflow too

    if (left_part > 0 && (node->left = node_create(left_part)) == NULL) {
        node_destroy(node);
        return NULL;
    }
    if (right_part > 0 && (node->right = node_create(right_part)) == NULL) {
        node_destroy(node);
        return NULL;
    }

    return node;
}

struct tree *tree_create(size_t size)
{
    struct tree *tree = malloc(sizeof(struct tree));
    if (tree == NULL) {
        perror("tree_create: malloc");
        return NULL;
    }

    if (size == 0) {
        tree->root = NULL;
    } else if ((tree->root = node_create(size)) == NULL) {
        free(tree);
        tree = NULL;
    }

    return tree;
}

void tree_destroy(struct tree *tree)
{
    assert(tree != NULL);

    node_destroy(tree->root);
    free(tree);
}

static size_t node_depth(const struct node *node)
{
    if (node == NULL)
        return 0;

    return 1 + maximum(node_depth(node->left), node_depth(node->right));
}

struct position
{
    int validity;
    int value;
};

static void recursive_tree_print(const struct node *node, struct position **lines, size_t line, size_t column)
{
    if (node == NULL)
        return;

    lines[line][column].validity = 1;
    lines[line][column].value = node->value;

    recursive_tree_print(node->left, lines, line + 1, column * 2);
    recursive_tree_print(node->right, lines, line + 1, column * 2 + 1);
}

bool tree_print(const struct tree *tree, FILE *out)
{
    assert(tree != NULL);

    static const char *spaces = "  ";
    static const char *format = "%2i";
    static const char *hline = "--";

    if (tree->root == NULL)
        return true;

    size_t depth = node_depth(tree->root);
    struct position **lines = malloc(sizeof(struct position *) * depth);
    if (lines == NULL) {
        perror("tree_print: malloc");
        return false;
    }

    size_t width = 1;
    for (size_t i = 0; i < depth; ++i) {
        lines[i] = malloc(sizeof(struct position) * width);
        if (lines[i] == NULL) {
            for (size_t j = 0; j < i; ++j)
                free(lines[j]);
            free(lines);
            return false;
        }
        memset(lines[i], 0, sizeof(struct position) * width);
        width *= 2;
    }

    recursive_tree_print(tree->root, lines, 0, 0);

    size_t columns = 1;
    for (size_t line = 0; line < depth; ++line, columns *= 2, width /= 2) {
        for (size_t column = 0; column < columns; ++column) {
            size_t s_count = width - 1;
            if (column == 0)
                s_count = width / 2 - 1;

            for (size_t s = 0; s < s_count; ++s)
                fputs(spaces, out);

            if (lines[line][column].validity)
                fprintf(out, format, lines[line][column].value);
            else
                fputs(spaces, out);
        }
        fputc('\n', out);
    }

    for (size_t i = 0; i < columns; ++i)
        fputs(hline, out);
    fputc('\n', out);

    for (size_t i = 0; i < depth; ++i)
        free(lines[i]);
    free(lines);

    return true;
}
