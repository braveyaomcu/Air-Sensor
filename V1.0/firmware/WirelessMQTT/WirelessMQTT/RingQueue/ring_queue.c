#include "ring_queue.h"

#include <string.h>

static void ring_q_item_copy_to(Ring_q_t *ringQueue ,void *itemOut,size_t *itemSize)
{
    memcpy(itemOut,RING_HEAD_ITEM(ringQueue),ringQueue->itemSize);
    if(itemSize)
    {
        *itemSize=ringQueue->itemSize;
    }
}

static void ring_q_item_copy_from(Ring_q_t *ringQueue ,void *itemIn)
{
    memcpy(RING_TAIL_ITEM(ringQueue),itemIn,ringQueue->itemSize);
    
}

static void ring_q_item_increase(Ring_q_t *ringQueue)
{
    ringQueue->tail=(ringQueue->tail+1)%ringQueue->itemCount;
    ++ringQueue->total;
}

static void ring_q_item_decrease(Ring_q_t *ringQueue)
{
    ringQueue->head=(ringQueue->head+1)%ringQueue->itemCount;
    if(ringQueue->total>0)
        --ringQueue->total;
}

eRingQError_t ring_q_create(Ring_q_t *ringQueue,void *pool,size_t  itemSize,size_t itemCount)
{
    ringQueue->head=0;
    ringQueue->tail=0;
    ringQueue->total=0;
    
    ringQueue->pool=(uint8_t *)pool;
    ringQueue->itemSize=itemSize;
    ringQueue->itemCount=itemCount;
    
    return ERR_RING_Q_NONE;
}

eRingQError_t ring_q_destroy(Ring_q_t *ringQueue)
{
    ringQueue->head=0;
    ringQueue->tail=0;
    ringQueue->total=0;
    
    ringQueue->pool=NULL;
    
    ringQueue->itemSize=0;
    ringQueue->itemCount=0;
    
    return ERR_RING_Q_NONE;
}

/*
eRingQError_t ring_q_create_dyn()
{
    
}

eRingQError_t ring_q_destroy_dyn()
{
    
}
*/

static int tos_ring_q_is_empty(Ring_q_t *ringQueue)
{
    int isEmpty;
//    if(ringQueue->head == ringQueue->tail)
//        isEmpty=ringQueue->total == 0? TRUE:FALSE;
//    else
//        isEmpty=FALSE;
//    
    if(ringQueue->total == 0)
        isEmpty=ringQueue->head == ringQueue->tail? TRUE:FALSE;
    else
        isEmpty=FALSE;
    return isEmpty;
}

static int tos_ring_q_is_full(Ring_q_t *ringQueue)
{
    int isFull;
    
    isFull=ringQueue->total == ringQueue->itemCount? TRUE:FALSE;
    return isFull;
}

eRingQError_t ring_q_enqueue(Ring_q_t *ringQueue,void *item,size_t  itemSize)
{
    if(itemSize != ringQueue->itemSize)
    {
        return ERR_RING_Q_ITEM_SIZE_NOT_MATCH;
    }
    
    if(tos_ring_q_is_full(ringQueue))
    {
        return ERR_RING_Q_FULL;
    }
    
    ring_q_item_copy_from(ringQueue,item);
    ring_q_item_increase(ringQueue);
    
    return ERR_RING_Q_NONE;
}

eRingQError_t ring_q_dequeue(Ring_q_t *ringQueue,void *item,size_t  *itemSize)
{
   
    if(tos_ring_q_is_empty(ringQueue))
    {
        return ERR_RING_Q_EMPTY;
    }
    
    ring_q_item_copy_to(ringQueue,item,itemSize);
    ring_q_item_decrease(ringQueue);
    
    return ERR_RING_Q_NONE;
}

eRingQError_t ring_q_flush(Ring_q_t *ringQueue)
{
    ringQueue->head=0;
    ringQueue->tail=0;
    ringQueue->total=0;
    
    return ERR_RING_Q_NONE;
}
