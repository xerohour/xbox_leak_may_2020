/******************************************************************************
 *                                                                            *
 *                        Voxware Proprietary Material                        *
 *                        Copyright 1996, Voxware, Inc.                       *
 *                             All Rights Resrved                             *
 *                                                                            *
 *                       DISTRIBUTION PROHIBITED without                      *
 *                     written authorization from Voxware                     *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
 *                                                                            *
 * FILENAME:  vmFrames.c                                                      *
 * PURPOSE:   Functions for doing frame traversal on circular vox buffers.    *
 * AURTHOR:   Epiphany Vera                                                   *
 *                                                                            *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/vem/vemFrm12.c_v   1.3   08 Jun 1998 15:37:30   bobd  $
 *****************************************************************************/

/******************************************************************************
 *                                                                            *
 *                          DETAILED REVISION HISTORY                         *
 *                                                                            *
 * (March 24, '97)                                                            *
 *    Created                                                                 *
 * (June 8, '98)                                                              *
 *    Converted ReadByteOffset to type (unsigned long) in seek routine.       *
 *      With a buffer size close to the maximum (unsigned short) the          *
 *      arithmetic could cause an incorrect wrap around in the circular       *
 *      Buffer. (Bob Dunn)                                                    *
 *                                                                            *
 *****************************************************************************/



#include "vci.h" /* Needed for the VCI_CODEC_IO_BLOCK.                        */
#include "quanvr12.h"
#include "vLibPack.h" /* BitUnpackCBuffer() */

#include "vemFrm12.h"

#include "xvocver.h"
/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: framesInBufferVR12()                                             */
/* PURPOSE : Counts the number of frames in the Vox buffer of the given codec */
/*           I/O block. This function is for VR12 only.                       */
/*                                                                            */
/* ************************************************************************** */
void framesInBufferVR12(const VCI_CODEC_IO_BLOCK  *hvciCodecIOBlk,
                        unsigned short            *pwNumFrames)
{
   unsigned short  ReadByteOffset,WriteByteOffset;
   unsigned char   ReadBitOffset, WriteBitOffset;
   unsigned short  BufferSize;
   unsigned long   BitsInBuffer;
   unsigned long   BitsRead;
   unsigned short  NumBits;
   unsigned char   *pBuffer;

   ReadByteOffset  = hvciCodecIOBlk->wVoxBufferReadByteOffset;
   WriteByteOffset = hvciCodecIOBlk->wVoxBufferWriteByteOffset;
   ReadBitOffset   = hvciCodecIOBlk->chVoxBufferReadBitOffset;
   WriteBitOffset  = hvciCodecIOBlk->chVoxBufferWriteBitOffset;
   BufferSize      = hvciCodecIOBlk->wVoxBufferSize;
   pBuffer         = hvciCodecIOBlk->pVoxBuffer;
   BitsRead        = 0;
  *pwNumFrames     = 0;

   /* First we calculate the number of bits in the buffer. We split this      */
   /* into two cases. Case 1 is without wrap-around of the circular buffer    */
   /* Case 2 is with wrap-around of the circular buffer.                      */
   if(WriteByteOffset>=ReadByteOffset) /* This means we have not wraped       */
                                       /* around in the buffer.               */
   {
      BitsInBuffer = (WriteByteOffset-ReadByteOffset+1)*8;
      BitsInBuffer = BitsInBuffer-ReadBitOffset-(8-WriteBitOffset);
   }
   else /* This is the case whereby the buffer has wrapped around.            */
   {
      BitsInBuffer = (WriteByteOffset+(BufferSize-ReadByteOffset)+1)*8;
      BitsInBuffer = BitsInBuffer-ReadBitOffset-(8-WriteBitOffset);
   }

  /* Now read until we exhaust the number of bits we have in the buffer */
   while(BitsRead < BitsInBuffer)
   {
     (*pwNumFrames)++;
      seekForwardVR12(ReadByteOffset, ReadBitOffset, BufferSize, pBuffer, 1,
                      &ReadByteOffset, &ReadBitOffset, &NumBits);
      BitsRead += NumBits;
   }
   if(BitsRead>BitsInBuffer)
      (*pwNumFrames)--;
}


/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: seekForwardVR12()                                                */
/* PURPOSE : Seeks to a given frame offset.                                   */
/*                                                                            */
/* ************************************************************************** */
void seekForwardVR12(unsigned short ReadByteOffset,
                      unsigned char  ReadBitOffset,
                      unsigned short BufferSize,
                      unsigned char  *pBuffer,
                      unsigned short wNumFrames,
                      unsigned short *pwByteOffset,
                      unsigned char  *pchBitOffset,
                      unsigned short *BitsInFrames)
{
   C_BUFFER_TYPE   CBuffer; 
   unsigned short  CurrentFrames;
   unsigned char   DummyRead[BYTESPERFRAME_VR]; /* the maximum bytes/frame */ 
   unsigned short  Class, wBitsInClass=0;
   unsigned long   ulReadByteOffset;

   CurrentFrames       = 0;
   *BitsInFrames       = 0;
   CBuffer.wBufferSize           = BufferSize;
   CBuffer.pBufferReadByteOffset = &ReadByteOffset;
   CBuffer.pBufferReadBitOffset  = &ReadBitOffset;
   CBuffer.pBuffer               = pBuffer;   

   while (CurrentFrames < wNumFrames)
   {
      BitUnpackCBuffer(&CBuffer, DummyRead, CLASS_BITS_VR12);
      Class = (unsigned short) DummyRead[0];
      *BitsInFrames += CLASS_BITS_VR12;

      if (Class == UV_FRAME)
         wBitsInClass=TOTAL_BITS_VR12_UNVOICED-CLASS_BITS_VR12;
      else if (Class == V_FRAME)  
         wBitsInClass=TOTAL_BITS_VR12_VOICED-CLASS_BITS_VR12;
      else if (Class == MIX_FRAME)
         wBitsInClass=TOTAL_BITS_VR12_MIXED-CLASS_BITS_VR12;
      else
         wBitsInClass=0;

      (*CBuffer.pBufferReadBitOffset)+=wBitsInClass;
      *BitsInFrames += wBitsInClass;  
      
      /*-------------------------------------------------------------
        We have to be careful here since the sum
          ulReadByteOffset + (ReadBitOffset/8) could be greater
          than the maximum (unsigned short).  We use an
          (unsigned long) to do the computation and testing.
      -------------------------------------------------------------*/
      ulReadByteOffset  = (unsigned long) (*CBuffer.pBufferReadByteOffset);
      ulReadByteOffset += (unsigned long) (ReadBitOffset/8);

      if ( ulReadByteOffset >= (unsigned long)(CBuffer.wBufferSize) )
         ulReadByteOffset -= (unsigned long) (CBuffer.wBufferSize);

      /*-------------------------------------------------------------
        After the above test we can guarantee that ulReadByteOffset
          can be represented as an (unsigned short).
      -------------------------------------------------------------*/
      (*CBuffer.pBufferReadByteOffset) = (unsigned short) ulReadByteOffset;

      (*CBuffer.pBufferReadBitOffset)%=8;

      CurrentFrames++;
   }
   *pwByteOffset = ReadByteOffset;
   *pchBitOffset = ReadBitOffset;

}

