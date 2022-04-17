#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

struct stack_item;

struct stack
{
    struct stack_item *top;
};

/**
 * Initializes the stack
 *
 * \param stack non NULL pointer to the stack structure
 */
void stack_init(struct stack *stack);

/**
 * Pushes a new pointer on the top of the stack
 *
 * \param stack non NULL pointer to the stack structure
 * \param data  pointer to push
 *
 * \return true if successful, false otherwise
 */
bool stack_push(struct stack *stack, void *data);

/**
 * Gets pointer stored on the top of the stack_top
 *
 * \param stack non NULL pointer to the stack structure
 *
 * \return pointer stored on the top the stack or NULL if the stack is empty
 */
void *stack_top(struct stack *stack);

/**
 * Pops a pointer from the stack
 *
 * \param stack non NULL pointer to the stack structure
 *
 * \return pointer popped from the stack or NULL if the stack is empty
 */
void *stack_pop(struct stack *stack);

/**
 * Checks if the stack is empty
 *
 * \param stack non NULL pointer to the stack structure
 */
bool stack_empty(struct stack *stack);

#endif
