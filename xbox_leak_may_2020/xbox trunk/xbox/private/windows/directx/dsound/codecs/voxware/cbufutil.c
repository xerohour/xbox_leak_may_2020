/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:      cBufUtil.c  
*
* Purpose:       Defines Circular buffer utility routines.
*
* Functions:     copyCBuf2LBuf
*
* Author/Date:   Epiphany Vera
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/cBufUtil.c_v   1.0   07 Apr 1998 17:58:04   weiwang  $
*
******************************************************************************/
#include "vLibPack.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:  copyCBuf2LBuf
*
* Action:    copy circular buffer to linear buffer
*
* Input:     CBuffer -- The circular buffer into which data is to be packed. 
*            ByteOffset -- byte offset 
*            BitOffset -- bit offset
*            BufferSize -- circular buffer size
*            bitsToCopy  -- how many bits need to copy to linear buffer
*            LBuffer -- linear buffer to copy to 
*
* Output:    ByteOffset -- updated byte offset
*            BitOffset -- updated bit offset
*            LBuffer  -- updated linear buffer
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
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
void copyCBuf2LBuf(unsigned char *CBuffer,    unsigned short *ByteOffset,  
                   unsigned char *BitOffset,  unsigned short  BufferSize,
                   unsigned long  bitsToCopy, unsigned char  *LBuffer)
{
   C_BUFFER_TYPE CircBuffer;
  
   CircBuffer.wBufferSize = BufferSize;
   CircBuffer.pBufferReadByteOffset = ByteOffset;
   CircBuffer.pBufferReadBitOffset  = BitOffset;
   CircBuffer.pBuffer = CBuffer;                                       

   while(bitsToCopy>=PACK_BITS_PER_CHAR)
   {
      BitUnpackCBuffer(&CircBuffer, LBuffer++, PACK_BITS_PER_CHAR);
      bitsToCopy-=PACK_BITS_PER_CHAR;
   }
   BitUnpackCBuffer(&CircBuffer, LBuffer, (short)bitsToCopy);
}




