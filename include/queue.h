
#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>

typedef struct chunk_pos_t chunk_pos;
typedef struct seedqueue_t seedqueue;

int enq(seedqueue *queue, chunk_pos *pos);
chunk_pos *deq(seedqueue *queue);
seedqueue *new_queue(void);
int init_queue(seedqueue *queue, size_t size);
int destroy_queue(seedqueue *queue);

#endif
