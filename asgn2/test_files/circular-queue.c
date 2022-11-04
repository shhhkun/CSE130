#include "queue.h"

#include <stdio.h>
#include <unistd.h>

int main() {
    queue_t *q = queue_new(10);
    if (q == NULL) {
        return 1;
    }

    // push 10 elements (queue full)
    for (int i = 1; i <= 10; i += 1) {
        queue_push(q, (void *)1);
    }

    void *ret;
    // pop 5 elements (queue not full, but tail at last index)
    for (int i = 1; i <= 5; i += 1) {
        queue_pop(q, &ret);
    }

    // push 5 elements (push wraps to front indices of queue "circular")
    for (int i = 1; i <= 5; i += 1) {
	    queue_push(q, (void *)2);
    }

    queue_delete(&q);
    return 0;
}
