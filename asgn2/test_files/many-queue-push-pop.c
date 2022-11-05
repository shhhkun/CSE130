#include "queue.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread1(void *args) {
    //printf("THREAD 1 BEGIN\n");
    queue_t *q1 = ((void **) args)[0];
    queue_t *q2 = ((void **) args)[1];
    int *v = ((void **) args)[2];
    pthread_cond_t *c = ((void **) args)[3];

    queue_push(q1, (void *) 1);
    queue_push(q1, (void *) 2);
    queue_push(q1, (void *) 3);

    queue_push(q2, (void *) 4);
    queue_push(q2, (void *) 5);
    queue_push(q2, (void *) 6);

    *v = 1;

    pthread_cond_signal(c);

    //printf("THREAD 1 FINISHED\n");
    return NULL;
}

void *thread2(void *args) {
    //printf("THREAD 2 BEGIN\n");
    queue_t *q1 = ((void **) args)[0];
    queue_t *q2 = ((void **) args)[1];
    int *v = ((void **) args)[2];
    pthread_cond_t *c = ((void **) args)[3];


    if (*v != 1) {
	    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	    pthread_mutex_lock(&lock);
	    pthread_cond_wait(c, &lock);
	    pthread_mutex_unlock(&lock);
    }

    void *ret;

    queue_pop(q1, &ret); // q1 pop 1
    if ((int) ret != 1) {
        return (void *) 1;
    }
    queue_pop(q2, &ret); // q2 pop 4
    if ((int) ret != 4) {
        return (void *) 1;
    }
    queue_pop(q1, &ret); // q1 pop 2
    if ((int) ret != 2) {
        return (void *) 1;
    }

    queue_pop(q2, &ret); // q2 pop 5
    if ((int) ret != 5) {
        return (void *) 1;
    }
    queue_pop(q1, &ret); // q1 pop 3
    if ((int) ret != 3) {
        return (void *) 1;
    }
    queue_pop(q2, &ret); // q2 pop 6
    if ((int) ret != 6) {
        return (void *) 1;
    }

    //printf("THREAD 2 FINISHED\n");
    return NULL;
}

int main() {
    queue_t *q1 = queue_new(10);
    queue_t *q2 = queue_new(10);
    if (q1 == NULL || q1 == NULL) {
        return 1;
    }

    pthread_t t1, t2;
    pthread_cond_t c;
    pthread_cond_init(&c, NULL);
    int v = 0;

    void *args[4] = { q1, q2, &v, &c };

    // pt_c(thread ad, NULL attr, func, func args)
    pthread_create(&t1, NULL, thread1, args);
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

    queue_delete(&q1);
    queue_delete(&q2);
    return 0;
}
