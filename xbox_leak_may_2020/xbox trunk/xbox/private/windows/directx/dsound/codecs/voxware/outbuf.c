/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       OutBuf.c                                                           
*                                                                              
* Purpose:        Prepare all frame parameters for the current frame or subframe                                                             
*                                                                              
* Functions:      VOXErr VoxInitOutputBuffers(OutputBuffer *myOutputBuffer_mblk);
*                 VOXErr VoxFreeOutputBuffers(OutputBuffer *myOutputBuffer_mblk);
*                 VOXErr VoxOutputManager(OutputBuffer *myOutputBuffer_mblk, 
*                                         void *mySubFrameParam_mblk);                                                             
*                                                                              
* Author/Date:    Rob Zopf 03/01/96                                                            
********************************************************************************
* Modifications:Ilan Berci/ 10/96 Removed global dependencies
*                                 Modularized code segments (Encapsulation)
*                                 Removed scope changes
*                                 Changed error code procedure
*                                 Removed useless code segments
*                                 General optimization                                                                 
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/OutBuf.c_v   1.0   26 Jan 1998 10:52:06   johnp  $
*******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "model.h"
#include "VoxMem.h"

#include "param.h"

#include "OutBuf.h"

#include "xvocver.h"

#ifdef _WIN64
#undef WIN32
#endif

/*******************************************************************************
* Function:      VOXErr VoxOutputManager(OutputBuffer *myOutputBuffer_mblk, 
*                                        void *SubFrameParam_mblk)
*
* Action:        The basic idea involves the desire to output frame buffers of
*                180 samples, no matter what the warp factor is, and also to minimize
*                the amount of RAM necessary.
*
*                - there are 3 buffers used :
*
*                      1) outBufferA - used to store the current WHOLE synthesized 
*                                      frame or subframe
*                      2) outBufferB1- 1 of 2 buffers for a double-buffered output
*                      3) outBufferB2- 2 of 2 buffers for a double-buffered output
*
*                ALGORITHM
*                
*                - synthesize current frame into A (done in LPCsynthesis)
*                - if current frame does not overflow B
*                       - append all of A to B
*                - else
*                       - append as much of A to B in order to fill B
*                       - switch output buffers
*                       - wait for DSP (if necessary)
*                       - copy remaining part of A to start of B
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		   VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxOutputManager(void *OutputBuffer_mblk, int outFrameLEN, float *InBuf)
{
   OutputBuffer *myOutputBuffer_mblk=(OutputBuffer *)OutputBuffer_mblk;

   short		*outBufB1 = myOutputBuffer_mblk->outBufferB1;
   short		*outBufB2 = myOutputBuffer_mblk->outBufferB2;
   short		*outBufB = myOutputBuffer_mblk->outBufferB;
   int		dataB = myOutputBuffer_mblk->dataB;
   short		dataAstart, dataAend;
   int      i;
   short    *pOutBufB;
   float    *pInBuf;

   /* check if buffer B will be overflowed if the current frame is copied to it */
   if ((outFrameLEN+dataB) < FRAME_LEN)
   { 
      pOutBufB = outBufB + dataB;
#ifndef WIN32
      for (i=0; i<outFrameLEN; i++)
         pOutBufB[i] = (short) InBuf[i];
#else
      _asm
      {
         push ecx
         push eax

         push ebx
         mov ecx, outFrameLEN

         mov eax, pOutBufB
         mov ebx, InBuf

l1:      fld dword ptr [ebx]
         
         fistp word ptr [eax]
         
         add eax, 2
         add ebx, 4
         
         dec ecx
         
         jnz l1
         
         pop ebx
         pop eax

         pop ecx
      }
#endif
      myOutputBuffer_mblk->dataB = dataB + outFrameLEN;
      myOutputBuffer_mblk->writeBuffer = 0;
   }
   else
   {
   /* Copy only part of buffer A */
      pOutBufB = outBufB + dataB;
#ifndef WIN32
      for (i=0; i<(FRAME_LEN-dataB); i++)
         pOutBufB[i] = (short) InBuf[i];
#else
      _asm
      {
         push ecx
         push eax

         push ebx
         mov ecx, FRAME_LEN

         mov eax, pOutBufB
         sub ecx, dataB

         mov ebx, InBuf
         jle l2

l3:      fld dword ptr [ebx]
         
         fistp word ptr [eax]
         
         add eax, 2
         add ebx, 4
         
         dec ecx
         jnz l3
         
l2:      pop ebx
         pop eax

         pop ecx
      }

#endif

      dataAstart = FRAME_LEN-dataB;
      dataAend = (short) outFrameLEN;    

   /* switch data buffers */
      if (outBufB==outBufB1)
      {
         outBufB = myOutputBuffer_mblk->outBufferB = outBufB2;
         myOutputBuffer_mblk->writeBuffer = 1;
      }
      else if (outBufB==outBufB2)
      {
         outBufB = myOutputBuffer_mblk->outBufferB = outBufB1;
         myOutputBuffer_mblk->writeBuffer = 2;
      }
      else {
         return 1;
      }

   /* Copy remaining part of buffer A to start of new Buffer B */
      if (dataAstart < FRAME_LEN)
      {
         pInBuf = InBuf + dataAstart;
         for (i=0; i<(dataAend-dataAstart); i++)
         {
            outBufB[i] = (short) pInBuf[i];
         }
      }
      myOutputBuffer_mblk->dataB = dataAend-dataAstart;
   } 
    
   return 0; 
}

/*******************************************************************************
* Function:       VOXErr VoxInitOutputBuffers(OutputBuffer *myOutputBuffer_mblk)
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxInitOutputBuffers(void **OutputBuffer_mblk)
{
   OutputBuffer *myOutputBuffer_mblk;

   if(VOX_MEM_INIT(myOutputBuffer_mblk=*OutputBuffer_mblk,1,sizeof(OutputBuffer))) 
      return 1;

   if(VOX_MEM_INIT(myOutputBuffer_mblk->outBufferB1,FRAME_LEN,sizeof(short))) 
      return 1;

   if(VOX_MEM_INIT(myOutputBuffer_mblk->outBufferB2,FRAME_LEN,sizeof(short))) 
      return 1;

   myOutputBuffer_mblk->outBufferB = myOutputBuffer_mblk->outBufferB1;

   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxFreeOutputBuffers(OutputBuffer *myOutputBuffer_mblk)
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		   VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/ 
unsigned short VoxFreeOutputBuffers(void **OutputBuffer_mblk)
{
   OutputBuffer *myOutputBuffer_mblk=(OutputBuffer *)*OutputBuffer_mblk;

   if(*OutputBuffer_mblk) {
      VOX_MEM_FREE(myOutputBuffer_mblk->outBufferB1);
      VOX_MEM_FREE(myOutputBuffer_mblk->outBufferB2);

      VOX_MEM_FREE(*OutputBuffer_mblk);
   }
   return 0;
}

#ifdef _WIN64
#define WIN32
#endif

