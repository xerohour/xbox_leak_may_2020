/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:      BitPackC.c
*
* Purpose:       Defines Circular buffer bit packing and unpacking routines.
*
* Functions:     BitPackCBuffer
*                BitUnPackCBuffer
*
* Author/Date:   Epiphany Vera
*******************************************************************************
*
* Modifications:
* (Oct 25, '96)
*    Created from original linear buffer packing routines written by Lee
*    Stewart and modified by Wei Wang and Ilan Berci. Ref: BitPack.c.  (E.V)
* (Aug 13, '97)
*    Added the compiler define PACK_BITS_PER_CHAR to allow DSP platforms to
*    use all bits in a char in cases whereby sizeof(char) > 8 bits.    (E.V)
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/BitPackC.c_v   1.2   02 Mar 1998 16:25:34   weiwang  $
*
******************************************************************************/

#include <assert.h>
#include "vLibPack.h"

#include "xvocver.h"


/******************************************************************************
*
* Function:  BitPackCBuffer
*
* Action:    Packs a bitstream into a circular bitstream buffer.
*
* Input:     CBuffer -- The circular buffer into which data is to be packed. 
*                       The data struct is defined in BitPackC.h. This routine
*                       only writes to the buffer, thus the read offsets are 
*                       not needed. 
*            cElement -- The char containing the data
*            cDataWidth -- The number of bits to be written into the buffer.
*
* Output:    CBuffer -- The circular buffer, the write-byte-offset and write-
*                       bit-offset are updated.
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*           The algorithm is written to write at most 8 bits, which are 
*           all from a single byte, into the buffer. The diagrams below 
*           illustrate the addition of 7 bits into the buffer. The symbol |
*           represents a byte boundary, symbol ` represents a bit boundary,
*           symbol x represents a valid bit and a space represents a vacant
*           bit position. The data to be written into the buffer looks like:
*           | `x`x`x`x`x`x`x|                                               
*           The circular buffer size is 4 bytes                              
*                                                                           
*           BEFORE ADDING SEVEN BITS:                                       
*           | ` ` ` ` ` ` ` |x`x`x`x`x`x`x`x| ` `x`x`x`x`x`x| ` ` ` ` ` ` ` |
*           WriteByteOffset = 2                                              
*           WriteBitOffset  = 6                                              
*           (ReadByteOffset = 1  Not needed for this operation.)             
*           (ReadBitOffset  = 0  Not needed for this operation.)             
*                                                                            
*           AFTER ADDING 7 BITS:                                             
*                                                                            
*           | ` ` ` ` ` ` ` |x`x`x`x`x`x`x`x|x`x`x`x`x`x`x`x| ` ` `x`x`x`x`x|
*           WriteByteOffset = 3                                              
*           WriteBitOffset  = 5                                              
*           (ReadByteOffset = 1  Not needed for this operation.)             
*           (ReadBitOffset  = 0  Not needed for this operation.)             
*                                                                            
*           AFTER ADDING ANOTHER 7 BITS:                                     
*                                                                            
*           | ` ` ` `x`x`x`x|x`x`x`x`x`x`x`x|x`x`x`x`x`x`x`x|x`x`x`x`x`x`x`x|
*           WriteByteOffset = 0                                              
*           WriteBitOffset  = 4                                              
*           (ReadByteOffset = 1  Not needed for this operation.)             
*           (ReadBitOffset  = 0  Not needed for this operation.)             
*                                                                            
*         NOTE: When the routine advances to the next byte in the buffer it  
*               clears it first.                                             
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void BitPackCBuffer(C_BUFFER_TYPE *CBuffer, unsigned char cElement, short cDataWidth) 
{
  unsigned char  *pBuffer;                                       
  unsigned short wBufferSize;
  unsigned short *pByteOffset;
  unsigned char  *pBitOffset;
  short          dwElement;

  assert(cDataWidth<=8); /* This routine only writes up to 8 bits at a time.  */


  /*--------------------------------------------------
    Set the circular buffer to the right position.
    And initialize the variables.
    --------------------------------------------------*/
  wBufferSize = CBuffer->wBufferSize;
  pByteOffset = CBuffer->pBufferWriteByteOffset;
  pBitOffset  = CBuffer->pBufferWriteBitOffset;
  pBuffer     = CBuffer->pBuffer + *pByteOffset;                                       
  dwElement   = cElement;

  /*--------------------------------------------------
    Pack to the current byte
    --------------------------------------------------*/
  if(*pBitOffset==0)
     *pBuffer = ((unsigned char)(dwElement&0x00ff));
  else
     *pBuffer |= ((unsigned char)((dwElement << *pBitOffset)&0x00ff));
  

  /*--------------------------------------------------
    When bit-offset >= PACK_BITS_PER_CHAR, pack the 
    next byte of the circular buffer.
    --------------------------------------------------*/
  *pBitOffset = (unsigned char)(*pBitOffset+cDataWidth); 
  if (*pBitOffset > PACK_BITS_PER_CHAR) 
  {
    pBuffer++;
    (*pByteOffset)++;
    if(*pByteOffset>=wBufferSize)
    {
      *pByteOffset=0;
      pBuffer = CBuffer->pBuffer; 
    }
    *pBuffer = (unsigned char)
               (dwElement>>(PACK_BITS_PER_CHAR-(*pBitOffset-cDataWidth)));
    *pBitOffset=(unsigned char)(*pBitOffset-PACK_BITS_PER_CHAR);
  }
  else if (*pBitOffset == PACK_BITS_PER_CHAR)
  {
    *pBitOffset = 0;
    (*pByteOffset)++;
    if(*pByteOffset>=wBufferSize)
      *pByteOffset=0;
  }
}

/******************************************************************************
*
* Function:  BitUnPackCBuffer
*
* Action:    Unpacks a bitstream from a circular bitstream buffer.
*
* Input:     CBuffer -- The circular buffer into which data is to be packed. 
*                       The data struct is defined in BitPackC.h. This routine
*                       only reads to the buffer, thus the write offsets are 
*                       not needed. 
*            cElement -- The char containing the data
*            cDataWidth -- The number of bits to be read from the buffer.
*
* Output:    CBuffer -- The read-byte-offset and read-bit-offset are updated.
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*           The algorithm is written to read at most 8 bits.
*           The diagrams below illustrate the reading of 7 bits from the     
*           buffer. The symbol | represents a byte boundary, symbol `        
*           represents a bit boundary, symbol x represents a valid bit and a 
*           space represents a vacant bit position. The data to be read from 
*           the buffer looks like:                                           
*                                 | `x`x`x`x`x`x`x|                          
*           The circular buffer size is 4 bytes                              
*                                                                            
*           BEFORE READING SEVEN BITS:                                       
*           | ` ` ` `x`x`x`x|x` ` ` ` ` ` ` |x`x`x`x`x`x`x`x|x`x`x`x`x`x`x`x|
*           ReadByteOffset = 1                                               
*           ReadBitOffset  = 7                                               
*           (WriteByteOffset = 0 Not needed for this operation.)             
*           (WriteBitOffset  = 4 Not needed for this operation.)             
*                                                                            
*           AFTER READING 7 BITS:                                            
*           |x`x`x`x`x`x`x`x| ` ` ` ` ` ` ` |x`x` ` ` ` ` ` |x`x`x`x`x`x`x`x|
*           ReadByteOffset = 2                                               
*           ReadBitOffset  = 6                                               
*           (WriteByteOffset = 0 Not needed for this operation.)             
*           (WriteBitOffset  = 4 Not needed for this operation.)             
*                                                                            
*           AFTER READING ANOTHER 7 BITS:                                    
*           |x`x`x`x` ` ` ` | ` ` ` ` ` ` ` | ` ` ` ` ` ` ` |x`x`x` ` ` ` ` |
*           ReadByteOffset = 3                                               
*           ReadBitOffset  = 5                                               
*           (WriteByteOffset = 0 Not needed for this operation.)             
*           (WriteBitOffset  = 4 Not needed for this operation.)             
*                                                                            
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void BitUnpackCBuffer(C_BUFFER_TYPE *CBuffer, unsigned char *cElement,
                      short cDataWidth)
{
  int Mask = (1<<cDataWidth) - 1;
  int Shift;
  unsigned char NewBitOffset;
 
  unsigned char  *pBuffer;                                       
  unsigned short wBufferSize;
  unsigned short *pByteOffset;
  unsigned char  *pBitOffset;



  /*--------------------------------------------------
    Set the circular buffer to the right position.
    And initialize the variables.
    --------------------------------------------------*/
  wBufferSize = CBuffer->wBufferSize;
  pByteOffset = CBuffer->pBufferReadByteOffset;
  pBitOffset  = CBuffer->pBufferReadBitOffset;
  pBuffer     = CBuffer->pBuffer + *pByteOffset;                                       

  NewBitOffset = (unsigned char)(*pBitOffset + cDataWidth);

  /*--------------------------------------------------
    When NewBitOffset > PACK_BITS_PER_CHAR, need unpack
    two bytes. Otherwise, only unpack the current byte.
    --------------------------------------------------*/
  if (NewBitOffset > PACK_BITS_PER_CHAR) 
  {
    Shift = (int)(PACK_BITS_PER_CHAR - *pBitOffset);
    *cElement = (unsigned char)(*pBuffer >> *pBitOffset);
    pBuffer++;
    (*pByteOffset)++;
    if(*pByteOffset>=wBufferSize)
    {
      *pByteOffset=0;
      pBuffer = CBuffer->pBuffer;                                       
    }
    *cElement |= (*pBuffer & (Mask >> Shift)) << Shift;
    NewBitOffset = (unsigned char)(NewBitOffset - PACK_BITS_PER_CHAR);
  }
  else
  {
    *cElement = (unsigned char)((*pBuffer >> *pBitOffset) & Mask);
    if (NewBitOffset == PACK_BITS_PER_CHAR)
    {
      NewBitOffset = 0;
      (*pByteOffset)++;
      if(*pByteOffset>=wBufferSize)
        *pByteOffset=0;
    }
  }
  
  *pBitOffset = NewBitOffset;
}
  

