#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stack.h"
#include "tree.h"

#define UNUSED(x) (void) (x)

/**********
 * Task 1 *
 ******** */

int tree_sum(const struct node *node)
{
    if (node == NULL) {
        return 0;
    }

    return node->value + tree_sum(node->left) + tree_sum(node->right);

}

int tree_size(const struct node *node)
{
    if (node == NULL) {
        return 0;
    }

    return 1 + tree_size(node->left) + tree_size(node->right);
}

int tree_max_helper(const struct node *node, int val)
{
    if (node == NULL) {
        return val;
    }

    if (node->value > val) {
        val = node->value;
    }

    if (tree_max_helper(node->left, val) > tree_max_helper(node->right, val)) {
        return tree_max_helper(node->left, val);
    } else {
        return tree_max_helper(node->right, val);
    }
}

int tree_max_value(const struct node *node)
{
    return tree_max_helper(node, 0);
}

int projector_sum(int value)
{
    return value;
}

int projector_size(int value)
{
    UNUSED(value);
    return 1;
}

int projector_div3(int value)
{
    if (value % 3 == 0) {
        return 1;
    }

    return 0;
}

int tree_aggregate(const struct node *node, int (*projector)(int))
{
    if (node == NULL) {
        return 0;
    }

    return projector(node->value)
    + tree_aggregate(node->left, projector)
    + tree_aggregate(node->right, projector);
}

void tree_for_each(const struct node *node,
        void (*operation)(int, void *),
        void *data_pack)
{
    if (node == NULL) {
        return;
    }

    operation(node->value, data_pack);
    tree_for_each(node->left, operation, data_pack);
    tree_for_each(node->right, operation, data_pack);
}

void operation_sum(int value, void *data_pack)
{
    int *partial_result = (int *) data_pack;
    *partial_result += value;
}

void operation_max(int value, void *data_pack)
{
    int *partial_result = (int *) data_pack;
    if (value > *partial_result) {
        *partial_result = value;
    }
}

void operation_min(int value, void *data_pack)
{
    int *partial_result = (int *) data_pack;
    if (value < *partial_result) {
        *partial_result = value;
    }
}

void interval_lower(int value, void *data_pack)
{
    int *partial_result = (int *) data_pack;
    if (value <= *partial_result) {
        *partial_result += 1;
    }
}

void interval_upper(int value, void *data_pack)
{
    int *partial_result = (int *) data_pack;
    if (value <= *partial_result) {
        *partial_result -= 1;
    }
}

int get_tree_sum(const struct tree *tree)
{
    int result = 0;
    if (tree && tree->root) {
        tree_for_each(tree->root, &operation_sum, &result);
    }
    return result;
}

int get_tree_max(const struct tree *tree)
{
    int result = 0;
    if (tree && tree->root) {
        tree_for_each(tree->root, &operation_max, &result);
    }
    return result;
}

int get_tree_min(const struct tree *tree)
{
    int result = 0;
    if (tree && tree->root) {
        result = tree->root->value;
        tree_for_each(tree->root, &operation_min, &result);
    }
    return result;
}

int in_interval(const struct tree *tree, int lower, int upper)
{
    int result = 0;
    if (tree && tree->root) {
        result = tree->root->value;
        tree_for_each(tree->root, &interval_lower, &result);
        tree_for_each(tree->root, &interval_upper, &result);
    }
    return result;
}

void usage_task_1(const struct tree *tree)
{
    puts("Task 1");
    if (tree == NULL || tree->root == NULL) {
        fprintf(stderr, "\tthe tree is empty");
        return;
    }

    printf("\tsum: %d\n", tree_sum(tree->root));
    printf("\tsize: %d\n", tree_size(tree->root));
    printf("\tmax number: %d\n", tree_max_value(tree->root));

    puts("Task 2");
    printf("\tsum: %d\n", tree_aggregate(tree->root,
                                         projector_sum));
    printf("\tsize: %d\n", tree_aggregate(tree->root,
                                          projector_size));
    printf("\tDivisible by 3: %d\n", tree_aggregate(tree->root,
                                                    projector_div3));

    puts("Task 3");
    printf("\tsum: %d\n", get_tree_sum(tree));
    printf("\tmax number: %d\n", get_tree_max(tree));
    printf("\tmin number: %d\n", get_tree_min(tree));
    int a = 0;
    int b = 0;
    scanf("Lower bound: %d\n", &a);
    scanf("Upper bound: %d\n", &b);
    printf("\tIn interval: %d\n", in_interval(tree, a, b));
}

/* small main */

int main(void)
{
    srand(time(NULL)); // initialize pseudo-random generator
    struct tree *tree = tree_create(11);
    if (tree == NULL)
        return EXIT_FAILURE;

    if (!tree_print(tree, stdout)) {
        tree_destroy(tree);
        return EXIT_FAILURE;
    }

    int exit_status = EXIT_SUCCESS;
    // HERE YOU CAN USE THE GENERATED TREE
    usage_task_1(tree);

    tree_destroy(tree);

    return exit_status;
}
