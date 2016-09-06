
//#define __inline__ 
////#define __inline__ inline 
//
//__inline__ void QueuePend(S_QUEUE *pQueBuf)
//{
//    while (pQueBuf->semaphore == 0);
//    pQueBuf->semaphore = 0;
//}
//
//__inline__ void QueuePost(S_QUEUE *pQueBuf)
//{
//    pQueBuf->semaphore = 1;
//}
//
//__inline__ void QueueInit(S_QUEUE *pQueBuf, void *pBuffer, unsigned int bufferSize)
//{
//    memset(pQueBuf, 0, sizeof(S_QUEUE));
//    memset(pBuffer, 0, bufferSize);
//    pQueBuf->pBuffer = pBuffer;
//    pQueBuf->bufSize = bufferSize;
//    pQueBuf->semaphore = 1;
//}
//
//__inline__ void QueueReset(S_QUEUE *pQueBuf)
//{
//    pQueBuf->head = 0;
//    pQueBuf->tail = 0;
//}
//
//__inline__ int QueueEmpty(S_QUEUE *pQueBuf)
//{
//    return pQueBuf->head == pQueBuf->tail;
//}
//
//__inline__ unsigned int QueueDataSize(S_QUEUE *pQueBuf)
//{
//    return ((pQueBuf->head + pQueBuf->bufSize) - pQueBuf->tail) % pQueBuf->bufSize;
//}
//
//__inline__ unsigned int QueueFreeSize(S_QUEUE *pQueBuf)
//{
//    return ((pQueBuf->tail + pQueBuf->bufSize - 1) - pQueBuf->head) % pQueBuf->bufSize;
//}
//
//__inline__ int QueueWrite(S_QUEUE *pQueBuf, unsigned char *pData, unsigned int size)
//{
//    int tmpSize;
//
//    tmpSize = ((pQueBuf->tail + pQueBuf->bufSize - 1) - pQueBuf->head) % pQueBuf->bufSize;
//    if (size > tmpSize)
//    {
//        pQueBuf->tail = (pQueBuf->tail + (size - tmpSize)) % pQueBuf->bufSize;
///*         size = tmpSize; */
//    }
//
//    if ((pQueBuf->head + size) >= pQueBuf->bufSize)
//    {
//        tmpSize = pQueBuf->bufSize - pQueBuf->head;
//        memcpy(&pQueBuf->pBuffer[pQueBuf->head], pData, tmpSize);
//        pData += tmpSize;
//        memcpy(&pQueBuf->pBuffer[0], pData, (size - tmpSize));
//    } else
//    {
//        memcpy(&pQueBuf->pBuffer[pQueBuf->head], pData, size);
//    }
//    pQueBuf->head = (pQueBuf->head + size) % pQueBuf->bufSize;
//
//    return size;
//}
//
//__inline__ int QueueRead(S_QUEUE *pQueBuf, unsigned char *pData, unsigned int size)
//{
//    unsigned int tmpSize;
//
//    tmpSize = ((pQueBuf->head + pQueBuf->bufSize) - pQueBuf->tail) % pQueBuf->bufSize;
//    if (size > tmpSize)
//    {
//        size = tmpSize;
//    }
//
//    if ((pQueBuf->tail + size) >= pQueBuf->bufSize)
//    {
//        tmpSize = pQueBuf->bufSize - pQueBuf->tail;
//        memcpy(pData, &pQueBuf->pBuffer[pQueBuf->tail], tmpSize);
//        pData += tmpSize;
//        memcpy(pData, &pQueBuf->pBuffer[0], (size - tmpSize));
//    } else
//    {
//        memcpy(pData, &pQueBuf->pBuffer[pQueBuf->tail], size);
//    }
//    pQueBuf->tail = (pQueBuf->tail + size) % pQueBuf->bufSize;
//
//    return size;
//}
//
