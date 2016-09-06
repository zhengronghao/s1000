/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : squeue.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/25/2014 9:56:42 AM
 * Description        : 
 *******************************************************************************/

#ifndef __SQUEUE_H__
#define __SQUEUE_H__ 
typedef struct
{
    unsigned char *pBuffer;
    unsigned char bufSize;
    unsigned char head;
    unsigned char tail;
    volatile char semaphore;
} SS_QUEUE;

#define __inline__ inline 

__inline__ void sQueuePend(SS_QUEUE *pQueBuf)
{
    while (pQueBuf->semaphore == 0);
    pQueBuf->semaphore = 0;
}

__inline__ void sQueuePost(SS_QUEUE *pQueBuf)
{
    pQueBuf->semaphore = 1;
}

__inline__ uint8_t sQueueClearSemaphore(SS_QUEUE *pQueBuf)
{
    if (pQueBuf->semaphore)
    {
        pQueBuf->semaphore = 0;
        return 0;
    } else
    {
        return 1;
    }
}

__inline__ uint8_t sQueueGetSemaphore(SS_QUEUE *pQueBuf)
{
    return pQueBuf->semaphore;
}

__inline__ void sQueueInit(SS_QUEUE *pQueBuf, void *pBuffer, unsigned char bufferSize)
{
    memset(pQueBuf, 0, sizeof(SS_QUEUE));
    memset(pBuffer, 0, bufferSize);
    pQueBuf->pBuffer = pBuffer;
    pQueBuf->bufSize = bufferSize;
    pQueBuf->semaphore = 1;
}

__inline__ void sQueueReset(SS_QUEUE *pQueBuf)
{
    pQueBuf->head = 0;
    pQueBuf->tail = 0;
}

__inline__ int sQueueEmpty(SS_QUEUE *pQueBuf)
{
    return pQueBuf->head == pQueBuf->tail;
}

__inline__ unsigned char sQueueDataSize(SS_QUEUE *pQueBuf)
{
    return ((pQueBuf->head + pQueBuf->bufSize) - pQueBuf->tail) % pQueBuf->bufSize;
}

__inline__ unsigned char sQueueFreeSize(SS_QUEUE *pQueBuf)
{
    return ((pQueBuf->tail + pQueBuf->bufSize - 1) - pQueBuf->head) % pQueBuf->bufSize;
}

__inline__ int sQueueWrite(SS_QUEUE *pQueBuf, unsigned char *pData, unsigned char size)
{
    unsigned int tmpSize;
    uint8_t flag=0;

     tmpSize = size
             + (((pQueBuf->head + pQueBuf->bufSize) - pQueBuf->tail) % pQueBuf->bufSize);//queue data size
    if (tmpSize >= pQueBuf->bufSize)
    {
        flag = 1;
    }

    if ((pQueBuf->head + size) >= pQueBuf->bufSize)
    {
        tmpSize = pQueBuf->bufSize - pQueBuf->head;
        memcpy(&pQueBuf->pBuffer[pQueBuf->head], pData, tmpSize);
        pData += tmpSize;
        memcpy(&pQueBuf->pBuffer[0], pData, (size - tmpSize));
    } else
    {
        memcpy(&pQueBuf->pBuffer[pQueBuf->head], pData, size);
    }
    pQueBuf->head = (pQueBuf->head + size) % pQueBuf->bufSize;
    if (flag == 1) {
        pQueBuf->tail = (pQueBuf->head + 1) % pQueBuf->bufSize;
    }

    return size;
}

__inline__ int sQueueRead(SS_QUEUE *pQueBuf, unsigned char *pData, unsigned char size)
{
    unsigned int tmpSize;

    tmpSize = ((pQueBuf->head + pQueBuf->bufSize) - pQueBuf->tail) % pQueBuf->bufSize;
    if (size > tmpSize)
    {
        size = tmpSize;
    }

    if ((pQueBuf->tail + size) >= pQueBuf->bufSize)
    {
        tmpSize = pQueBuf->bufSize - pQueBuf->tail;
        memcpy(pData, &pQueBuf->pBuffer[pQueBuf->tail], tmpSize);
        pData += tmpSize;
        memcpy(pData, &pQueBuf->pBuffer[0], (size - tmpSize));
    } else
    {
        memcpy(pData, &pQueBuf->pBuffer[pQueBuf->tail], size);
    }
    pQueBuf->tail = (pQueBuf->tail + size) % pQueBuf->bufSize;

    return size;
}
#endif
