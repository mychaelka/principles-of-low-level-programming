#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "list.h"
#include "list_utils.h"

int main(void)
{
    struct list list_t;
    list_init(&list_t, sizeof(int));
    assert(list_t.head == NULL);
    assert(list_t.tail == NULL);
    assert(list_t.elem_size == sizeof(int));
    assert(list_is_empty(&list_t));

    int num = 1;
    list_push_back(&list_t, &num);
    assert(*((int *) (list_t.head->data)) == 1);
    assert(!list_is_empty(&list_t));
    assert(list_size(&list_t) == 1);

    int num2 = 2;
    list_push_back(&list_t, &num2);
    int data = 0;
    assert(list_pop_front(&list_t, &data));
    assert(*((int *) (list_t.head->data)) == 2);
    assert(data == 1);

    assert(list_pop_front(&list_t, NULL));
    assert(list_is_empty(&list_t));





    return 0;
}
