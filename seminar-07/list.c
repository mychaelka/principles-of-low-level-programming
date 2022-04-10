#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "list.h"

//-----------------------------------------------------------------------------
//  Convenience macros
//-----------------------------------------------------------------------------

// --<----<----<----<----<----<----<----<----<----<----<----<----<----<----<--
// REMOVE THESE MACROS WHEN DONE
// --<----<----<----<----<----<----<----<----<----<----<----<----<----<----<--

// This is a common way to say that some parameter is not used.
#define UNUSED(VARIABLE)                                                    \
    ((void) (VARIABLE))

// If a macro consists of more than one statement, there must be some
// way to hold these statements together, otherwise a construct like this
//
//      if (CONDITION)
//          MACRO(...);
//
// would wreak havoc in the code. To make things worse, programmers naturally
// add ';' after the statements in C, so the macro should expand to something
// that allows a semicolon afterwards.
//
// A reasonable candidate is therefore ⟨do { ... } while (0)⟩, since it
// contains a block, executes exactly once and is friendly to following
// semicolon.

#define NOT_IMPLEMENTED()                                                   \
    do {                                                                    \
        fprintf(stderr, "Unimplemented function '%s' called.\n", __func__); \
        abort();                                                            \
    } while (0)

//-----------------------------------------------------------------------------
//  Static functions
//-----------------------------------------------------------------------------

/*
 * Feel free to add custom functions here. However, to make them invisible
 * to other translation units (basically ⟨*.c⟩ files) you should use ⟨static⟩
 * keyword like this:
 *
 *      static void list_helper(struct list *list)
 *      {
 *          ...
 *      }
 */

//-----------------------------------------------------------------------------
//  Task 1: List initialization
//-----------------------------------------------------------------------------

void list_init(struct list *list, size_t elem_size)
{
    // TODO: You should add similar sanity checks to other functions as well.
    assert(list != NULL);
    assert(elem_size > 0);

    // You might think that previous checks could be written as
    //
    //      assert(list != NULL && elem_size > 0);
    //
    // However, if another programmer passed an invalid value, they would
    // get an unhelpful error like:
    //
    //      program: list.c: Assertion “list != NULL && elem_size > 0” failed
    //
    // It is not clear which one of the conditions (or perhaps both of them)
    // is false. So you better keep the assertions separate.

    list->head = NULL;
    list->tail = NULL;
    list->elem_size = elem_size;
}

//-----------------------------------------------------------------------------
//  Task 2: Adding elements to the end of the list
//-----------------------------------------------------------------------------

bool list_push_back(struct list *list, const void *data)
{
    void *new_data = malloc(list->elem_size);
    if (new_data == NULL){
        return false;
    }
    memcpy(new_data, data, list->elem_size);
    struct node *new_node = malloc(sizeof(struct node));
    if (new_node == NULL){
        free(new_data);
        return false;
    }

    new_node->next = NULL;
    new_node->data = new_data;

    if (list->head == NULL){
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }

    return true;
}

//-----------------------------------------------------------------------------
//  Task 3: Basic queries
//-----------------------------------------------------------------------------

size_t list_size(const struct list *list)
{
    size_t counter = 0;
    struct node *curr_node = list->head;
    while (curr_node != NULL)
    {
        counter++;
        curr_node = curr_node->next;
    }
    return counter;
}

bool list_is_empty(const struct list *list)
{
    return list->head == NULL && list->tail == NULL;
}

//-----------------------------------------------------------------------------
//  Task 4: Element retrieval
//-----------------------------------------------------------------------------

bool list_pop_front(struct list *list, void *data)
{
    if (list->head == NULL){
        return false;
    }
    if (data != NULL){
        memcpy(data, list->head->data, list->elem_size);
    }

    struct node *first_node = list->head;
    if (list->head == list->tail){
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->head = list->head->next;
    }

    free(first_node);
    return true;
}

//-----------------------------------------------------------------------------
//  Task 5: Destruction and mayhem
//-----------------------------------------------------------------------------

void list_destroy(struct list *list)
{
    while (!list_is_empty(list))
    {
        list_pop_front(list, NULL);
    }
    free(list);
}

//-----------------------------------------------------------------------------
//  Bonus 1: Operations at both ends
//-----------------------------------------------------------------------------

/**  NOTE  **
 *
 * You ⟨MUST⟩ uncomment the ⟨LLIST_ENABLE_ALL_ENDS⟩ macro at the beginning
 * of ⟨list.h⟩ to enable tests and implementation of these functions.
 */

#if defined(LLIST_ENABLE_ALL_ENDS)

bool list_push_front(struct list *list, const void *data)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(data);
    /* ! */ NOT_IMPLEMENTED();
}

bool list_pop_back(struct list *list, void *data)
{
    // Note that without ⟨LLIST_ENABLE_TWO_LINKS⟩ this function can
    // only be implemented in ⟦𝒪(n)⟧ time (unless other crazy optimizations
    // are in place).

    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ NOT_IMPLEMENTED();
}

#endif // LLIST_ENABLE_ALL_ENDS
