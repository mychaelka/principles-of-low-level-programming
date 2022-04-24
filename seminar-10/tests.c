#define CUT
#define CUT_MAIN
#ifndef DEBUG
#  define CUT_FORK_MODE
#endif
#include "cut.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"

//=============================================================================
//  Examples
//=============================================================================

/*
 * Trivial test that succeeds.
 */
TEST(trivial_success)
{
    int zero = 0;
    CHECK(zero == 0);
}

/*
 * Trivial test that fails.
 */
TEST(trivial_failure)
{
    int zero = 0;
    CHECK(zero == 1);
}

//-----------------------------------------------------------------------------

/*
 * Example of ‹queue_create()› test.
 *
 * Scenario
 * --------
 * WHEN     ‹queue_create()› gets called
 * WHEN     requested capacity is zero
 * THEN     queue is created successfully
 */
TEST(queue_create__zero_capacity)
{
    struct queue *queue = NULL;
    // The ‹queue_create()› should succeed.
    CHECK(queue_create(&queue, sizeof(int), 0u));

    // ‹queue› should now point to allocated structure.
    CHECK(queue != NULL);

    // Attributes should be set properly.
    CHECK(queue->element   == sizeof(int));
    CHECK(queue->capacity  == 0u);
    CHECK(queue->index     == 0u);
    CHECK(queue->size      == 0u);
    // ‹queue.memory› may or may not be ‹NULL›, the documentation does not
    // specify this.

    // Well done, free the structure.
    free(queue);
    // We didn't use ‹queue_destroy()› since we are supposed to call only
    // one function from the queue interface in each test. If we did call
    // it, we would risk having two sources of failure in one test.
}

/**
 * Example of ‹queue_enqueue()› test.
 *
 * Scenario
 * --------
 * GIVEN    empty queue of integers with capacity 32
 * WHEN     a single element is enqueued
 * THEN     size changes to 1
 */
TEST(queue_enqueue__single)
{
    int memory[32u]; // This simulates queue's internal memory.
    struct queue queue = {
        // We will set up the structure ourselves instead of calling
        // ‹queue_create()›, since we are not (yet) sure it works as it should.
        .element    = sizeof(*memory),
        .capacity   = sizeof(memory) / sizeof(*memory),
        .memory     = memory,
        // All other attributes not mentioned here will
        // be initialized to 0.
    };

    // Now let's enqueue a single element.
    int number = 123456;
    // The function shall not fail.
    CHECK(queue_enqueue(&queue, &number));

    // Check that the element was indeed stored in the memory.
    CHECK(memory[0] == number);
    // Make sure that size has increased.
    CHECK(queue.size == 1u);
    // Index should remain 0.
    CHECK(queue.index == 0u);
}

//=============================================================================
//  Your tests go here
//=============================================================================

TEST(full_capacity)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 1u));

    CHECK(queue_capacity(queue) == 1);
    int data = 10;
    CHECK(queue_enqueue(queue, &data));
    CHECK(queue_is_full(queue));
    int data2 = 20;
    CHECK(!queue_enqueue(queue, &data2));

}

TEST(empty_after_dequeue)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 1u));
    int data = 10;
    CHECK(queue_enqueue(queue, &data));

    CHECK(!queue_is_empty(queue));
    CHECK(queue_dequeue(queue, &data));
    CHECK(queue_is_empty(queue));
    CHECK(queue->size == 0);
    CHECK(queue->index == 0);
}

TEST(test_queue_destroy)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 1u));
    int data = 10;
    CHECK(queue_enqueue(queue, &data));
    CHECK(queue->size == 1);
    CHECK(queue->index == 0);

    queue_destroy(queue);
}

TEST(full_queue_multiple)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 10u));

    int data[10];
    for (int i = 0; i < 10; i++) {
        data[i] = i;
        CHECK(queue_enqueue(queue, &data[i]));
    }

    CHECK(!queue_is_empty(queue));
    CHECK(queue_is_full(queue));

    CHECK(queue_dequeue(queue, &data[0]));
    CHECK(!queue_is_full(queue));
}

TEST(oldest_element)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 10u));

    int data[10];
    for (int i = 0; i < 10; i++) {
        data[i] = i;
        CHECK(queue_enqueue(queue, &data[i]));
    }

    int* a = (int*) queue_peek(queue);
    CHECK(*a == 0);

}

TEST(test_index)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 10u));

    int data[10];
    for (int i = 0; i < 7; i++) {
        data[i] = i;
        CHECK(queue_enqueue(queue, &data[i]));
    }

    CHECK(queue->index == 6);

}

TEST(test_size)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 10u));

    int data[10];
    for (int i = 0; i < 7; i++) {
        data[i] = i;
        CHECK(queue_enqueue(queue, &data[i]));
    }

    CHECK(queue->size == 7);
}

TEST(test_element_size)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 10u));
    CHECK(queue_capacity(queue) == 10);
    CHECK(queue_size(queue) == 0);
    CHECK(queue_element_size(queue) == sizeof(int));
}

TEST(empty_queue_peek)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(int), 10u));

    CHECK(queue_peek(queue) == NULL);
}

TEST(small_element_size)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(char), 1u));

    long long int a = 25245234;
    long long int b = 25245234;
    CHECK(!queue_enqueue(queue, &a));
    CHECK(queue_dequeue(queue, &a));
    CHECK(a == b);
}

TEST(modify_data)
{
    struct queue *queue = NULL;

    int array[5] = {1, 2, 3, 4, 5};

    CHECK(queue_create(&queue, sizeof(int*), 1u));
    CHECK(queue_enqueue(queue, array));
    CHECK(queue_dequeue(queue, array));

    CHECK(array[0] == 1);
    CHECK(array[1] == 2);
    CHECK(array[2] == 3);

}

TEST(dequeue_empty)
{
    struct queue *queue = NULL;
    int data[10];
    for (int i = 0; i < 7; i++) {
        data[i] = i;
        }
    CHECK(queue_create(&queue, sizeof(char), 1u));
    CHECK(!queue_dequeue(queue, &data[0]));
    CHECK(data[0] == 0);
}

TEST(is_full_size_capacity)
{
    struct queue queue = {
            .capacity  = 1u,
            .size      = 1u
    };

    CHECK(queue_is_full(&queue));
    CHECK(!queue_is_empty(&queue));
}

TEST(create_queue_zero_capacity)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(char), 0));
    queue_destroy(queue);
    struct queue *queue2 = NULL;
    CHECK(!queue_create(&queue2, 0, 1u));
}

TEST(dequeue_fail)
{
    struct queue *queue = NULL;
    CHECK(queue_create(&queue, sizeof(char), 5u));
    int data[10];
    for (int i = 0; i < 7; i++) {
        data[i] = i;
    }
    CHECK(queue_enqueue(queue, &data[0]));
    CHECK(queue_enqueue(queue, &data[1]));
    CHECK(queue_enqueue(queue, &data[2]));
    CHECK(queue_enqueue(queue, &data[3]));
    CHECK(queue_dequeue(queue, NULL));
    CHECK(queue->size == 3);
    CHECK(queue_dequeue(queue, &data[1]));
    CHECK(data[1] == 1);
    CHECK(queue->size == 2);
}
