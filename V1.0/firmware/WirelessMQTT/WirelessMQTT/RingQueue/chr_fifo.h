#ifndef _CHR_FIFO_H
#define _CHR_FIFO_H

#include "ring_queue.h"

typedef Ring_q_t chr_fifo_t;

eRingQError_t chr_fifo_create(chr_fifo_t *chrFifo,void *buffer,size_t size);
eRingQError_t chr_fifo_destroy(chr_fifo_t *chrFifo);
eRingQError_t chr_fifo_push(chr_fifo_t *chrFifo,uint8_t data);
int chr_fifo_push_stream(chr_fifo_t *chrFifo,uint8_t *stream,size_t size);
eRingQError_t chr_fifo_pop(chr_fifo_t *chrFifo,uint8_t *data);
int chr_fifo_pop_stream(chr_fifo_t *chrFifo,uint8_t *stream,size_t size);
eRingQError_t chr_fifo_flush(chr_fifo_t *chrFifo);

#endif
