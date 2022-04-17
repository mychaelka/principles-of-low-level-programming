#include "stack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct stack_item
{
    void *data;
    struct stack_item *next;
};

void stack_init(struct stack *stack)
{
    assert(stack != NULL);

    stack->top = NULL;
}

bool stack_push(struct stack *stack, void *data)
{
    assert(stack != NULL);

    struct stack_item *item = malloc(sizeof(struct stack_item));
    if (item == NULL) {
        perror("stack_push: malloc");
        return false;
    }

    item->data = data;
    item->next = stack->top;
    stack->top = item;

    return true;
}

void *stack_top(struct stack *stack)
{
    assert(stack != NULL);

    if (stack->top == NULL)
        return NULL;

    return stack->top->data;
}

void *stack_pop(struct stack *stack)
{
    assert(stack != NULL);

    if (stack->top == NULL)
        return NULL;

    struct stack_item *to_remove = stack->top;
    void *data = to_remove->data;
    stack->top = to_remove->next;
    free(to_remove);
    return data;
}

bool stack_empty(struct stack *stack)
{
    assert(stack != NULL);

    return stack->top == NULL;
}
