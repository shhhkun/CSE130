#include "queue.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread1(void *args) {
    //printf("THREAD 1 BEGIN\n");
    queue_t *q = ((void **) args)[0];

    queue_push(q, (void *) 2); // push to queue (will wait indefinitely)

    //printf("THREAD 1 FINISHED\n");
    return NULL;
}

void *thread2(void *args) {
    //printf("THREAD 2 BEGIN\n");
    queue_t *q = ((void **) args)[0];

    void *ret;
    queue_pop(q, &ret);
    if ((int) ret != 1) {
        return (void *) 1;
    }

    //printf("THREAD 2 FINISHED\n");
    return NULL;
}

int main() {
    queue_t *q = queue_new(10);
    if (q == NULL) {
        return 1;
    }
    // fill queue to full
    for (int i = 1; i <= 10; i += 1) {
        queue_push(q, (void *) 1);
    }

    pthread_t t1, t2;

    void *args[1] = { q };

    // pt_c(thread ad, NULL attr, func, func args)
    pthread_create(&t1, NULL, thread1, args);

    sleep(5);

    pthread_create(&t2, NULL, thread2, args);

    void *rc;

    // close threads
    pthread_join(t1, &rc);
    // if rc "ret val"  != NULL, error code occurred
    if (rc != NULL) {
        return 1;
    }
    pthread_join(t2, &rc);
    if (rc != NULL) {
        return 1;
    }

    queue_delete(&q);
    return 0;
}
