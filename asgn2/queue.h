#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "queue.h"

typedef struct queue_t queue_t;

queue_t *queue_new(int size);
void queue_delete(queue_t **q);
bool queue_push(queue_t *q, void *elem);
bool queue_pop(queue_t *q, void **elem);
