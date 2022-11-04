#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

struct queue {
    int head;
    int tail;
    int size; // total capacity
    int len; // # of elems in queue
    void **elems;
    pthread_mutex_t lock;
    pthread_cond_t full;
    pthread_cond_t empty;
};

queue_t *queue_new(int size) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if (q) {
        int x = pthread_mutex_init(&q->lock, NULL);
        int y = pthread_cond_init(&q->full, NULL);
        int z = pthread_cond_init(&q->empty, NULL);
        q->head = 0;
        q->tail = 0;
        q->len = 0;
        q->size = size;
        q->elems = calloc(size, sizeof(void *));
	// check that init's and array allocation worked
        if (!q->elems || x != 0 || y != 0 || z != 0) {
            free(q);
            q = NULL;
        }
    }
    return q;
}

void queue_delete(queue_t **q) {
    if (*q && (*q)->elems) {
        free((*q)->elems);
        free(*q);
        pthread_mutex_destroy(&(*q)->lock);
        pthread_cond_destroy(&(*q)->full);
        pthread_cond_destroy(&(*q)->empty);
        *q = NULL;
    }
    return;
}

bool queue_push(queue_t *q, void *elem) {
    if (q) {
        pthread_mutex_lock(&q->lock);
        while (q->len == q->size) { // if full
            //printf("QUEUE IS FULL\n");
            pthread_cond_wait(&q->full, &q->lock);
        }
        int i = q->tail % q->size;
        q->elems[i] = elem; // push
        //printf("pushing %d\n", (int) elem);
        q->tail += 1;

        q->len += 1;
        pthread_cond_signal(&q->empty);
        pthread_mutex_unlock(&q->lock);
        return true;
    }
    return false;
}

bool queue_pop(queue_t *q, void **elem) {
    if (q) {
        pthread_mutex_lock(&q->lock);
        while (q->len == 0) { // if empty
            //printf("QUEUE IS EMPTY\n");
            pthread_cond_wait(&q->empty, &q->lock);
        }
        int i = q->head % q->size;
        *elem = q->elems[i]; // pop (dereference)
        //printf("popped %d\n", (int) *elem);
        q->head += 1;

        q->len -= 1;
        // if empty after dequeue (assuming not zero-state, reset)
        if (q->head > q->tail) {
            q->head = q->tail = 0;
        }

        pthread_cond_signal(&q->full);
        pthread_mutex_unlock(&q->lock);
        return true;
    }
    return false;
}

/*
void queue_print(queue_t *q) {
	printf("queue: ");
	for (int i = 0; i < q->size; i += 1) {
		printf("%d, ", (int)q->elems[i]);
	}
	printf("\n");
	return;
}
*/
