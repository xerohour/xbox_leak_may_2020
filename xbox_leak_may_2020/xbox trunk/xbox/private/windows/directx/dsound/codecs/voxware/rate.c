/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       rate.c
*                                                                              
* Purpose:        all bit manipulations for the rate change
*                                                                              
* Functions:
*                                                                              
* Author/Date:    Ilan Berci 97/09/23
*
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/rate.c_v   1.6   05 Jun 1998 17:59:24   bobd  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "rate.h"

#include "xvocver.h"

const unsigned short wDefaultTable[]={SC_BYTES_ADDITIONAL_3K,SC_BYTES_ADDITIONAL_6K,0};
/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: isSingleStream()                                                */
/* PURPOSE : determines if the stream is a single instance                   */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
unsigned char isSingleStream(unsigned long dwStream)
{
   if(!dwStream)  /* if there is no instance than leave */
      return 0;

   while(!(dwStream&0x01))
      dwStream>>=1;  /* find first bit in stream */

   if(dwStream>>=1)
      return 0;     /* not a valid stream */
   else 
      return 1;      /* single instance */
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: isValidStream()                                                 */
/* PURPOSE : determines if the stream is a valid stream                      */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
unsigned char isValidStream(unsigned long dwStream, const unsigned short *wStreamTable)
{
   unsigned long dwMask=0;

   if(!dwStream)  /* if there is no instance than leave */
      return 0;

   if(!wStreamTable) /* switch to the default table if none was provided */
      wStreamTable=wDefaultTable;

   dwMask=~dwMask; /* get the inverse for ease of operation */
   while(*wStreamTable++) 
      dwMask<<=1;

   if(dwMask&dwStream)
      return 0; /* bit found in invalid section */

   while(dwStream&0x01)
      dwStream>>=1;

   if(dwStream)
      return 0; /* not a straight sequence */
   else 
      return 1; /* all checks a success */
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: bytesInStream()                                                 */
/* PURPOSE : determines the number of bytes in the stream                    */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
unsigned short bytesInStream(unsigned long dwStream, const unsigned short *wStreamTable)
{
   int wAccumulate=0;

   if(!wStreamTable) /* switch to the default table if none was provided */
      wStreamTable=wDefaultTable;

   while(*wStreamTable) {
      if(dwStream&0x01)
         wAccumulate+=*wStreamTable;
      dwStream>>=1;
      wStreamTable++;
   }
   return (unsigned short)wAccumulate;
}


/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: bytesToStream()                                                 */
/* PURPOSE : Put the number of bytes to the stream.                          */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
unsigned long bytesToStream(unsigned short wBytes, const unsigned short *wStreamTable)
{

   unsigned long dwStream=0;
   if(!wStreamTable) /* switch to the default table if none was provided */
      wStreamTable=wDefaultTable;

   while(*wStreamTable) {
      wBytes = (unsigned short)(wBytes - *wStreamTable++);

      if((short)wBytes<0)
         break;

      dwStream<<=1;
      dwStream|=0x01;
   }

   return(dwStream);
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: seekForward()                                                   */
/* PURPOSE : returns the byte offset after seeking to desired frame          */
/*           returns -1 on insufficient buffer and -2 on corrupted stream    */
/*                                                                           */
/* ************************************************************************* */
short seekForward(unsigned char *pVoxBuffer,
                  unsigned short wReadByteOffset,
                  unsigned short wWriteByteOffset,
                  unsigned short wBufferSize,
                  unsigned short wNumFrames,
                  const unsigned short *wStreamTable)
{
   int            i;
   int            iCrossOver;
   unsigned short wBytesLeft;
   char           cContinue;
   assert(pVoxBuffer != NULL);

   /*---------------------------------------------------
     If wReadByteOffset equals wWriteByteOffset then
       the buffer is empty!
   ---------------------------------------------------*/
   if (wReadByteOffset==wWriteByteOffset)
           return -1;

   if (wWriteByteOffset>wReadByteOffset)
      iCrossOver = 1;
   else
      iCrossOver = 0;

   /*---------------------------------------------------
     switch to the default table if none was provided
   ---------------------------------------------------*/
   if(!wStreamTable)
      wStreamTable=wDefaultTable;

   while((wNumFrames--)>0) {
      i=0;
      do {
         /*---------------------------------------------------
           Increment wReadByteOffset by wStreamTable[i].
             We have to be careful here because 
             wReadByteOffset+wStreamTable[i] could be larger
             than the maximum unsigned short in which case
             we cannot compute the sum directly.
         ---------------------------------------------------*/
         wBytesLeft = wBufferSize - wReadByteOffset;
         if (wBytesLeft <= wStreamTable[i])
         {
            /*---------------------------------------------------
              If we will pass the write pointer as we wrap
                around then there are not enough sample in the 
                buffer.
            ---------------------------------------------------*/
            if (wWriteByteOffset>wReadByteOffset)
              return -1;

            wReadByteOffset = wStreamTable[i] - wBytesLeft;
            iCrossOver      = 1;
         } else {
            wReadByteOffset += wStreamTable[i];
         }
         i++;

         /*---------------------------------------------------
           If wReadByteOffset passses wWriteByteOffset then
             we are reading passed the end of the data in
             the buffer.  If wReadByteOffset is equal to 
             wWriteByteOffset then the buffer is empty, but 
             that's okay as long as we don't try to read 
             any further.
         ---------------------------------------------------*/
         if (iCrossOver&&(wReadByteOffset>wWriteByteOffset))
           return -1; /* not enough space in buffer to continue */

         /*---------------------------------------------------
            check for continuation bit
         ---------------------------------------------------*/
         if (wReadByteOffset)
            cContinue = (char)(pVoxBuffer[wReadByteOffset-1]&0x080);
         else
            cContinue = (char)(pVoxBuffer[wBufferSize-1]&0x080);

         /*---------------------------------------------------
            We have to be careful on these tests...the test
              for the corrupted bitstream comes first.
              If the bitstream is not corrupt, then we
              could be out of data.
         ---------------------------------------------------*/
         if((!wStreamTable[i]) && cContinue)
            return -2; /* bitstream may be corrupted */

         else if((wWriteByteOffset == wReadByteOffset) && cContinue)
            return -1; /* need more in buffer */

      } while(cContinue);
   }

   return(wReadByteOffset);
}


