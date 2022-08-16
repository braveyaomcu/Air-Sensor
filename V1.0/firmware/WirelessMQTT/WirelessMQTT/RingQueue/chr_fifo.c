#include "chr_fifo.h"

eRingQError_t chr_fifo_create(chr_fifo_t *chrFifo,void *buffer,size_t size)
{
    eRingQError_t err;
    err=ring_q_create(chrFifo,buffer,sizeof(uint8_t),size);
    
    return err;
}

eRingQError_t chr_fifo_destroy(chr_fifo_t *chrFifo)
{
    eRingQError_t err;
    err=ring_q_destroy(chrFifo);
    return err;
}

eRingQError_t chr_fifo_flush(chr_fifo_t *chrFifo)
{
    return ring_q_flush(chrFifo);
}

eRingQError_t chr_fifo_push(chr_fifo_t *chrFifo,uint8_t data)
{  
    return ring_q_enqueue(chrFifo,&data,1);
}

int chr_fifo_push_stream(chr_fifo_t *chrFifo,uint8_t *stream,size_t size)
{
    int  i=0;
    eRingQError_t err;
    while(i<size)
    {
        err=ring_q_enqueue(chrFifo,stream+i,1);
        if(err != ERR_RING_Q_NONE)
            return i;
        ++i;
    }
    
    return i;
}

eRingQError_t chr_fifo_pop(chr_fifo_t *chrFifo,uint8_t *data)
{
    return ring_q_dequeue(chrFifo,data,NULL);
}

int chr_fifo_pop_stream(chr_fifo_t *chrFifo,uint8_t *stream,size_t size)
{
    int  i=0;
    //uint8_t data;
    eRingQError_t err;
    while(i<size)
    {
        err=ring_q_dequeue(chrFifo,stream+i,NULL);
        if(err != ERR_RING_Q_NONE)
            return i;
        ++i;
    }
    
    return i;
}
