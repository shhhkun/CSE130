#include "queue.h"

#include <stdio.h>
#include <stdbool.h>

int main() {
    queue_t *q = NULL;

    bool x = queue_push(q, (void *) 1);

    void *r;
    bool y = queue_pop(q, &r);
    if (x == true || y == true) {
        printf("cant do\n");
        // if either push/pop return doesnt return false
        return 1;
    }

    return 0;
}
