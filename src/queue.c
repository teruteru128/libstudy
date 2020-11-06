
#include <stdint.h>
#include <stdio.h>
#include "internal_queue.h"

//リングバッファ
int enq(seedqueue *queue, chunk_pos *pos)
{
    return 0;
}

chunk_pos *deq(seedqueue *queue)
{
}

int init_queue(seedqueue *queue, size_t size)
{
}

seedqueue *queue_new()
{
    return 0;
}

/** TODO: destroy時にqueue内に残されたアイテムの扱いは？ */
int destroy_queue(seedqueue *queue)
{
}
