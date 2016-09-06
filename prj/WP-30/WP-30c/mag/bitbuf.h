/*
********************************************************************************
*     
*     
*     
********************************************************************************
*/

#ifndef BITBUF_H
#define BITBUF_H

// structure/typdefs

// the BitBuffer structure
typedef struct struct_BitBuf
{
	unsigned char *dataptr;			// the physical memory address where the buffer is stored
	unsigned short	size;			// the allocated byte size of the buffer
	unsigned short bytePos;			// current byte position
	unsigned short bitPos;			// current bit position
	unsigned short datalength;		// the length of the data (in bits) currently in the buffer
	unsigned short dataindex;		// the index (in bits) into the buffer where the data starts
} BitBuf;

// function prototypes

//! initialize a buffer to start at a given address and have given size
void bitbufInit(BitBuf* bitBuffer, unsigned char *start, unsigned short bytesize);

//! get the bit at the current position in the buffer
unsigned char bitbufGet(BitBuf* bitBuffer);

//! get a bit at the specified index in the buffer (kind of like array access)
// ** note: this does not remove/delete the bit that was read
unsigned char bitbufGetAtIndex(BitBuf* bitBuffer, unsigned short bitIndex);

//! store a bit at the current position in the buffer
void bitbufStore(BitBuf* bitBuffer, unsigned char bit_value);

//! return the number of bits in the buffer
unsigned short bitbufGetDataLength(BitBuf* bitBuffer);

// check if the buffer is full/not full (returns non-zero value if not full)
//unsigned char  bitbufIsNotFull(cBuffer* buffer);

//! resets the read/write position of the buffer to beginning
void bitbufReset(BitBuf* bitBuffer);

//! flush (clear) the contents of the buffer
void bitbufFlush(BitBuf* bitBuffer); 

unsigned char bitbufIsFull(BitBuf* buffer);  //

#endif
//@}


