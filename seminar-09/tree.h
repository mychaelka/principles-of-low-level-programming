#ifndef TREE_H
#define TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct node
{
    int value;
    struct node *left;
    struct node *right;
};

struct tree
{
    struct node *root;
};

/**
 * Creates a semi-balanced tree.
 *
 * \param size total count of nodes in the tree
 *
 * \return pointer to generated tree or NULL if the function fails
 */
struct tree *tree_create(size_t size);

/**
 * Deallocates the tree.
 *
 * \param tree non NULL pointer to the tree structure
 */
void tree_destroy(struct tree *tree);

/**
 * Prints the tree with root on top and layers of children on rows.
 *
 * Example:
 *     4
 *   /   \
 *  2     6
 *   \   /
 *    3 5
 *
 * \param tree non NULL pointer to the tree structure to be printed
 * \param out handle to the file(stream) to print to
 *
 * \return true if successful, false otherwise
 */
bool tree_print(const struct tree *tree, FILE *out);

#endif
