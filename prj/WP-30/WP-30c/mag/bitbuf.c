/*
********************************************************************************
*     
*     
*     
********************************************************************************
*/

#include "common.h"
#include "bitbuf.h"




/*
*****************************************************************************************
*Func:     Initialize the bit buffer
*Note:     sets the start location and size of the buffer in memory
*
*
*****************************************************************************************
*/	
void bitbufInit(BitBuf* bitBuffer, unsigned char *start, unsigned short bytesize)
{
    bitBuffer->dataptr = start;   	// set start pointer of the buffer
    bitBuffer->size = bytesize;     
    bitBuffer->dataindex = 0;
    bitbufFlush(bitBuffer);             // initialize indexing and length
}

/*
*****************************************************************************************
*Func:     access routines
*Note:     
*          Get a bit from the current position in the buffer
*	   returns the bit at the current position in the buffer
*	   and increments the bit position
*****************************************************************************************
*/
unsigned char bitbufGet(BitBuf* bitBuffer)
{
    unsigned char byte;
    unsigned char bit_value;
        	
    byte = bitBuffer->dataptr[bitBuffer->bytePos];	     /* get current working byte */
    bit_value = (uchar)((byte & (1<<bitBuffer->bitPos))?(1):(0));         /* read data bit */
    
    if (bitBuffer->bitPos < 7) {                                 /* increment bit counter */
        bitBuffer->bitPos++;
    } else {
	    bitBuffer->bitPos = 0;                               /* increment byte counter */
	    bitBuffer->bytePos++;
    }

    return bit_value;
}

/*
*****************************************************************************************
*Func:     Get a bit from a given index into the buffer
*Note:     returns the bit at position [bitIndex] in the buffer
*          index:from 0
*
*****************************************************************************************
*/	
unsigned char bitbufGetAtIndex(BitBuf* bitBuffer, unsigned short bitIndex)
{
    return (bitBuffer->dataptr[bitIndex>>3] & (1<<(bitIndex & 0x07)))?(1):(0);  
                                                         // return bit at index in buffer
}

/*
*****************************************************************************************
*Func:     Store a bit at the current position in the buffer
*Note:     stores the bit at the current position in the buffer
* 	   and increments the bit position
*          
*****************************************************************************************
*/
void bitbufStore(BitBuf* bitBuffer, unsigned char bit_value)
{
    unsigned char byte;

    if (bitBuffer->datalength == bitBuffer->size) {     /* MODIFY BY HJZ 06.6.1 */ 
        return;
    }
    
    byte = bitBuffer->dataptr[bitBuffer->bytePos];     /* get current working byte */
	
    if (bit_value) {                                   /* apply data bit  */
        byte |=  (1<<bitBuffer->bitPos); 
    } else {
        byte &= ~(1<<bitBuffer->bitPos);
    }
	   
    bitBuffer->dataptr[bitBuffer->bytePos] = byte;     /* store data  */
    bitBuffer->datalength++;                           /*  */
    if (bitBuffer->bitPos < 7) {                       /* increment bit counter  */
        bitBuffer->bitPos++;
    } else {                                           /* increment byte counter */
        bitBuffer->bitPos = 0;
	    bitBuffer->bytePos++;
    }
}

/*
*****************************************************************************************
*Func:     
*Note:     
* 	   
*
*****************************************************************************************
*/
void bitbufReset(BitBuf* bitBuffer)
{
    bitBuffer->bytePos = 0; // reset counters
    bitBuffer->bitPos  = 0;
}

/*
*****************************************************************************************
*Func:     
*Note:     
* 	   
*
*****************************************************************************************
*/
void bitbufFlush(BitBuf* bitBuffer)
{
	
    bitBuffer->datalength = 0; // flush contents of the buffer
	
    bitbufReset(bitBuffer);    // reset indexing
}

/*
*****************************************************************************************
*Func:     
*Note:     
* 	   
*
*****************************************************************************************
*/
unsigned short bitbufGetDataLength(BitBuf* bitBuffer)
{
    return bitBuffer->datalength;
}

unsigned char bitbufIsFull(BitBuf* buffer)
{
    // check to see if the buffer has room
    // return true if there is room
    return ((buffer->datalength) == (buffer->size));
}




