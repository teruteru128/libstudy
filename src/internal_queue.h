
#include <stdint.h>
#include <stdio.h>

typedef struct chunk_pos
{
    int64_t seed;
    int32_t x;
    int32_t z;
} chunk_pos;

// 配列キュー
// TODO chunk_pos* を void* にする -> マイクラのチャンク探索向けから汎用化する
typedef struct seedqueue
{
    chunk_pos *data;
    size_t size;
    size_t head;
    size_t tail;
} seedqueue;
