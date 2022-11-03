#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

struct queue_t {
    int head;
    int tail;
    int size; // total capacity
    void **elems;
    pthread_mutex_t lock;
    pthread_cond_t full;
    pthread_cond_t empty;
};

queue_t *queue_new(int size) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if (q) {
        pthread_mutex_init(&q->lock, NULL);
        pthread_cond_init(&q->full, NULL);
        pthread_cond_init(&q->empty, NULL);
        q->head = 0;
        q->tail = 0;
        q->size = size;
        //printf("size = %d\n", q->size);
        q->elems = malloc(sizeof(void *) * size);
        if (!q->elems) {
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
    pthread_mutex_lock(&q->lock);
    while (q->tail == q->size) { // if full
        //printf("QUEUE IS FULL\n");
        pthread_cond_wait(&q->full, &q->lock);
    }
    q->elems[q->tail] = elem; // push
    //printf("pushing %d\n", (int) elem);
    q->tail += 1;

    pthread_cond_signal(&q->empty);
    pthread_mutex_unlock(&q->lock);
    return true;
}

bool queue_pop(queue_t *q, void **elem) {
    pthread_mutex_lock(&q->lock);
    while (q->head == q->tail) { // if empty
        //printf("QUEUE IS EMPTY\n");
        pthread_cond_wait(&q->empty, &q->lock);
    }
    /*
    if (*elem == NULL) {
        printf("POP ERROR\n");
        pthread_mutex_unlock(&q->lock);
        return false;
    }
    */
    *elem = q->elems[q->head]; // pop (dereference)
    //printf("popped %d\n", (int) *elem);
    q->head += 1;
    // if empty after dequeue (assuming not zero-state, reset)
    if (q->head > q->tail) {
        q->head = q->tail = 0;
    }

    pthread_cond_signal(&q->full);
    pthread_mutex_unlock(&q->lock);
    return true;
}
