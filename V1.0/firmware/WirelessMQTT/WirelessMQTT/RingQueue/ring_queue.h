#ifndef _RING_QUEUE_H
#define _RING_QUEUE_H

#include <stdint.h>
#include <stdio.h>

#define TRUE    1u
#define FALSE   0u

typedef enum    eRingQError_en{
    ERR_RING_Q_NONE,
    ERR_RING_Q_FULL,
    ERR_RING_Q_EMPTY,
    ERR_RING_Q_ITEM_SIZE_NOT_MATCH,
    ERR_RING_Q_BUSY
}eRingQError_t;

typedef struct Ring_q_st{
    uint16_t    head;   
    uint16_t    tail;   
    size_t      total;  
    
    uint8_t     *pool;  
    
    size_t      itemSize;   
    size_t      itemCount;  
    
}Ring_q_t;

#define RING_HEAD_ITEM(ringQueue)   (uint8_t *)(&ringQueue->pool[ringQueue->head*ringQueue->itemSize])
#define RING_TAIL_ITEM(ringQueue)   (uint8_t *)(&ringQueue->pool[ringQueue->tail*ringQueue->itemSize])
#define RING_NEXT_ITEM(ringQueue)   (ringQueue->head+1)/(ringQueue->total)

eRingQError_t ring_q_create(Ring_q_t *ringQueue,void *pool,size_t  itemSize,size_t itemCount);
eRingQError_t ring_q_destroy(Ring_q_t *ringQueue);
eRingQError_t ring_q_enqueue(Ring_q_t *ringQueue,void *item,size_t  itemSize);
eRingQError_t ring_q_dequeue(Ring_q_t *ringQueue,void *item,size_t  *itemSize);
eRingQError_t ring_q_flush(Ring_q_t *ringQueue);


#endif
