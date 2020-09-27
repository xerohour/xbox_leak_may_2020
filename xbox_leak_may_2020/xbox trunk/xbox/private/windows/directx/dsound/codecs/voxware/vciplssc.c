/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vciPlsSC.c  
*
* Purpose:     Additional VCI functions specific to the SC codecs      
*
* Functions:   vciSetFramesSkippedSC, ...etc
*
* Author/Date: John Puterbaugh, Ilan Berci & Epiphany Vera    97/05/20
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   P:/r_and_d/archives/scaleable/vci/vciPlsSC.c_v   1.17   02 Sep 1998 08:39:38   xiaoqins  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h> /* log10( ) */

#include "vci.h"
#include "vciError.h"
#include "vciPriv.h"
#include "VoxMem.h"
#include "codec.h"
#include "rate.h"
#include "DecPlsSC.h"
#include "paramSC.h"
#include "DecPriSC.h"

#ifndef DECODE_ONLY
#include "EncSC.h"
#endif

#include "FadeSC.h"

#include "vciPlus.h"
#include "vciPlsSC.h"

#include "xvocver.h"

#define MIN_WARP 0.5F
#define MAX_WARP 5.0F
#define DB2LEVEL_VALUE  28.3333333333F 

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetFramesSkippedSC()                                         */
/* PURPOSE : Tell the decoder that frames have been skipped/lost             */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSetFramesSkippedSC(void *pDecodeMemBlk, int iFramesSkipped)
{
   VCI_CODEC_BLOCK *vciCodecMblk=NULL;
   
   if(!VCI_BLOCK_CHECK(pDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(pDecodeMemBlk))
      return VCI_NOT_DEC_HANDLE;
      
   vciCodecMblk = (VCI_CODEC_BLOCK *)pDecodeMemBlk;

   voxSetFrameSkippedSC(vciCodecMblk->hCodecMblk, iFramesSkipped);

   return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetRepeatFrameFlagSC()                                       */
/* PURPOSE : Tell the decoder that the frame is a repeat.                    */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSetRepeatFrameFlagSC(void *pDecodeMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk;
 
   if(!VCI_BLOCK_CHECK(pDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(pDecodeMemBlk))
      return VCI_NOT_DEC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pDecodeMemBlk;

   voxSetRepeatFrameSC(vciCodecMblk->hCodecMblk, 1);
 
   return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetWarpFactorSC()                                            */
/* PURPOSE : Sets the warp factor for the decoder.                           */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSetWarpFactorSC(void *pDecodeMemBlk, float wWarpFactor)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   if(!VCI_BLOCK_CHECK(pDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(pDecodeMemBlk))
      return VCI_NOT_DEC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pDecodeMemBlk;

   /* First make sure that the handle passed is the correct one. */
   if (vciCodecMblk->dwVciChunkID != SANITY_CHECK) 
      return VCI_NOT_VCI_HANDLE;
   if (vciCodecMblk->chEncOrDec   != DECODE_ID)    
      return VCI_NOT_DEC_HANDLE;

   if(wWarpFactor<MIN_WARP||wWarpFactor>MAX_WARP)
      return VCI_ARG_OUT_OF_RANGE;

   voxSetWarpFactorSC(vciCodecMblk->hCodecMblk, wWarpFactor);

   return VCI_NO_ERROR;
}


/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetWarpFactor()                                              */
/* PURPOSE : Retrieves the warp factor setting in the  decoder.              */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetWarpFactorSC(void *pDecodeMemBlk, float *pwWarpFactor)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(pDecodeMemBlk))
      return VCI_NOT_DEC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pDecodeMemBlk;

   assert(pwWarpFactor != NULL);
   *pwWarpFactor = voxGetWarpFactorSC(vciCodecMblk->hCodecMblk);

  return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciDecodeNeedsInput()                                           */
/* PURPOSE : Checks whether the decoder needs input or not.                  */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciDecodeNeedsInputSC(void *pDecodeMemBlk, unsigned char *pUnPack)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(pDecodeMemBlk))
      return VCI_NOT_DEC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pDecodeMemBlk;
   assert(pUnPack!= NULL);

   *pUnPack=voxGetDecReqDataFlagSC(vciCodecMblk->hCodecMblk);

   return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetEnergyLevelSC()                                           */
/* PURPOSE : Gets the energy level.                                          */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetEnergyLevelSC(void *pCodecMemBlk, short *pwEnergyLevel)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   VCI_CODEC_IO_BLOCK *vciCodecIOBlk = NULL;
   float fFrameEnergy = 0.0F ;
   float fSample;
   int i;

   assert(pwEnergyLevel!=NULL);

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;
   vciCodecMblk = (VCI_CODEC_BLOCK *)pCodecMemBlk;

   /* vciCodeIOBlk must be attached to vciCodeMblk */
   vciCodecIOBlk = (VCI_CODEC_IO_BLOCK *)(vciCodecMblk->pVciCodecIOBlock);
   VCI_IO_BLK_PARAM_CHECK(vciCodecIOBlk);

#ifndef DECODE_ONLY
   if(VCI_IS_ENCODE(pCodecMemBlk)) {
     fFrameEnergy = ((ENC_MEM_BLK *)(vciCodecMblk->hCodecMblk))->frameEnergy;
   }
#endif

   if(VCI_IS_DECODE(pCodecMemBlk)) {
     /* compute frame_energy from PCMBuffer */
     if (vciCodecIOBlk->wSamplesInPCMBuffer > 0) {
       for (i = 0; i < vciCodecIOBlk->wSamplesInPCMBuffer; i++) {
	 fSample = (float)vciCodecIOBlk->pPCMBuffer[i];
	 fFrameEnergy += fSample * fSample;
       }

       fFrameEnergy /= (float)vciCodecIOBlk->wSamplesInPCMBuffer;
     }
   }

   *pwEnergyLevel=(short)(log10(fFrameEnergy+1.0)* DB2LEVEL_VALUE);
  
  return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetInjectSilenceFlagSC()                                     */
/* PURPOSE : Perform fade-in/fade-out and background noise injection during  */
/*           packet loss.                                                    */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSetInjectSilenceFlagSC(void *pDecodeMemBlk)
{
   DEC_MEM_BLK *pDecMemBlk = NULL;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(pDecodeMemBlk))
      return VCI_NOT_DEC_HANDLE;

   pDecMemBlk = (DEC_MEM_BLK *)((VCI_CODEC_BLOCK *)pDecodeMemBlk)->hCodecMblk;
   SetCurrentFrameStateSC(pDecMemBlk->pFadeIOMemBlk, SILENCE);

   return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciNumFramesInBufferSC()                                        */
/* PURPOSE : Counts the number of frames in the Vox buffer of the given      */
/*           codec I/O block.                                                */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciNumFramesInBufferSC(const void *pVciCodecMemBlk,
                                   const VCI_CODEC_IO_BLOCK *pvciCodecIOBlk,
                                   unsigned short *pwNumFrames,
                                   unsigned short *pwByteOffset,
                                   unsigned char  *pchBitOffset)
{
   short wSeekReturn, wSeekInitial;
   unsigned short wNumFrames=0;
   const VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   const unsigned short *pStreamTable = NULL;
   unsigned short wByteOffset;

    /* Do parameter checking on IO block. */
   VCI_IO_BLK_PARAM_CHECK(pvciCodecIOBlk);

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pVciCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pVciCodecMemBlk;

   if(vciCodecMblk->pVciPlusMethodStruct)
      if(vciCodecMblk->pVciPlusMethodStruct->wStreamTable)
         pStreamTable = vciCodecMblk->pVciPlusMethodStruct->wStreamTable;

   assert(pwNumFrames!= NULL);

   wByteOffset = wSeekReturn=pvciCodecIOBlk->wVoxBufferReadByteOffset;

   while(wSeekReturn != pvciCodecIOBlk->wVoxBufferWriteByteOffset) {

      wSeekInitial = wSeekReturn;
      wSeekReturn = seekForward(pvciCodecIOBlk->pVoxBuffer,
                  wSeekInitial,
                  pvciCodecIOBlk->wVoxBufferWriteByteOffset,
                  pvciCodecIOBlk->wVoxBufferSize,
                  1, pStreamTable);

      if(wSeekReturn < 0)
        break;
      else
        wByteOffset = wSeekReturn;

      wNumFrames++;
   } 

   *pwByteOffset=wByteOffset;
   *pwNumFrames=wNumFrames;
   *pchBitOffset=0;

   return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSeekForwardSC()                                              */
/* PURPOSE : Seeks to a given frame offset.                                  */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSeekForwardSC(const void *pVciCodecMemBlk,
                             const VCI_CODEC_IO_BLOCK *pvciCodecIOBlk,
                             unsigned short wNumFrames,
                             unsigned short *pwReadOffset,
                             unsigned char  *pchBitOffset)
{
   short wSeek; 
   const VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   const unsigned short *pStreamTable = NULL;

   VCI_IO_BLK_PARAM_CHECK(pvciCodecIOBlk);

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pVciCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pVciCodecMemBlk; 

   if(vciCodecMblk->pVciPlusMethodStruct)
      if(vciCodecMblk->pVciPlusMethodStruct->wStreamTable)
         pStreamTable = vciCodecMblk->pVciPlusMethodStruct->wStreamTable;

   wSeek=seekForward(pvciCodecIOBlk->pVoxBuffer,
                  pvciCodecIOBlk->wVoxBufferReadByteOffset,
                  pvciCodecIOBlk->wVoxBufferWriteByteOffset,
                  pvciCodecIOBlk->wVoxBufferSize,
                  wNumFrames, 
                  pStreamTable);

   if(wSeek==-1)
      return VCI_INSUFFICIENT_VOX_DATA;
   else if(wSeek==-2)
      return VCI_BITSTREAM_CORRUPTED;
   else {
      *pwReadOffset=(unsigned short)wSeek;
      *pchBitOffset=0;
   }

   return VCI_NO_ERROR;

}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciExtractFromBitStreamSC()                                     */
/* PURPOSE : Extracts the specified embedded streams from the Vox Buffer of  */
/*           the codec I/O block                                             */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciExtractFromBitStreamSC(const void *pVciCodecMemBlk,
                                      const VCI_CODEC_IO_BLOCK *pvciInputIOBlk,
                                      VCI_CODEC_IO_BLOCK *pvciOutputIOBlk,
                                      unsigned long dwBitField)
{
   const VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   unsigned short wReadByteOffset, wReadIncrementer, wBytesToExtract, wSkipOverBytes;
   unsigned long dwSkipBitField=0x0, dwCopyBitField=dwBitField;
   short wSeekActual, wSeekProjected, wSeekToSkip;
   const unsigned short *pStreamTable = NULL;

   VCI_EXTRACT_IO_BLK_PARAM_CHECK(pvciInputIOBlk);
   VCI_EXTRACT_IO_BLK_PARAM_CHECK(pvciOutputIOBlk);

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pVciCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pVciCodecMemBlk; 

   if(vciCodecMblk->pVciPlusMethodStruct)
      if(vciCodecMblk->pVciPlusMethodStruct->wStreamTable)
         pStreamTable = vciCodecMblk->pVciPlusMethodStruct->wStreamTable;

   wReadByteOffset=pvciInputIOBlk->wVoxBufferReadByteOffset;
   pvciOutputIOBlk->wVoxBufferWriteByteOffset = pvciOutputIOBlk->wVoxBufferReadByteOffset;

   if(isValidStream(dwBitField, pStreamTable)) {
      wBytesToExtract = bytesInStream(dwBitField, pStreamTable);
      wSkipOverBytes  = 0;
   } else {
      if(!isSingleStream(dwBitField))
         return VCI_BITSTREAM_NOT_SUPPORTED;

      while(!(dwCopyBitField&0x01)) {
         dwCopyBitField>>=1;
         dwSkipBitField<<=1;
         dwSkipBitField+=1;
      }
      wBytesToExtract = bytesInStream(dwBitField, pStreamTable);
      wSkipOverBytes  = bytesInStream(dwSkipBitField, pStreamTable);
   }

   wSeekActual = pvciInputIOBlk->wVoxBufferReadByteOffset;
   while(wReadByteOffset != pvciInputIOBlk->wVoxBufferWriteByteOffset) {

      wReadIncrementer = wSeekActual;

      wSeekToSkip = (short)(wReadIncrementer+wSkipOverBytes);
      if(wSeekToSkip >= pvciInputIOBlk->wVoxBufferSize)
         wSeekToSkip = (short)(wSeekToSkip - pvciInputIOBlk->wVoxBufferSize);

      wSeekProjected  = (short)(wSeekToSkip+wBytesToExtract);
      if(wSeekProjected >= pvciInputIOBlk->wVoxBufferSize)
         wSeekProjected = (short)(wSeekProjected - pvciInputIOBlk->wVoxBufferSize);

      wSeekActual=seekForward(pvciInputIOBlk->pVoxBuffer, wReadByteOffset,
         pvciInputIOBlk->wVoxBufferWriteByteOffset,
         pvciInputIOBlk->wVoxBufferSize, 1, pStreamTable);

      if(wSeekActual<0)
         return VCI_BITSTREAM_CORRUPTED;
      else 
         wReadByteOffset=(unsigned short)wSeekActual;

      while((wReadIncrementer!=wSeekToSkip)&&(wReadIncrementer!=wReadByteOffset)) {

         /*-----------------------------------------------------
           This line was:

              pvciInputIOBlk->pVoxBuffer[wReadIncrementer++];

           but that seems unnecessary and produces a warning.
         -----------------------------------------------------*/
         wReadIncrementer++;

         if(wReadIncrementer >= pvciInputIOBlk->wVoxBufferSize)
            wReadIncrementer= (unsigned short)(wReadIncrementer-pvciInputIOBlk->wVoxBufferSize);

      } 

      while((wReadIncrementer!=wSeekProjected)&&(wReadIncrementer!=wReadByteOffset)) {
         pvciOutputIOBlk->pVoxBuffer[pvciOutputIOBlk->wVoxBufferWriteByteOffset++]=
            pvciInputIOBlk->pVoxBuffer[wReadIncrementer++];

         if(pvciOutputIOBlk->wVoxBufferWriteByteOffset >= pvciOutputIOBlk->wVoxBufferSize)
            pvciOutputIOBlk->wVoxBufferWriteByteOffset = 
            (unsigned short)(pvciOutputIOBlk->wVoxBufferWriteByteOffset - 
                    pvciOutputIOBlk->wVoxBufferSize);

         if(wReadIncrementer >= pvciInputIOBlk->wVoxBufferSize)
            wReadIncrementer = (unsigned short)(wReadIncrementer-pvciInputIOBlk->wVoxBufferSize);

         if(pvciOutputIOBlk->wVoxBufferWriteByteOffset==
            pvciOutputIOBlk->wVoxBufferReadByteOffset) 
            return VCI_BITSTREAM_INSUFFICIENT;
      }

      /* set the continuation bit to false */
      if(pvciOutputIOBlk->wVoxBufferWriteByteOffset)
         pvciOutputIOBlk->pVoxBuffer[pvciOutputIOBlk->wVoxBufferWriteByteOffset-1] &= 0x07F;
      else
         pvciOutputIOBlk->pVoxBuffer[pvciOutputIOBlk->wVoxBufferSize-1] &= 0x07F;
   }
   return VCI_NO_ERROR;
}
/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciCombineBitStreamSC()                                         */
/* PURPOSE : Combines the specified emnedded streams from the Vox Buffer of  */
/*           the codec I/O block pointed to be pvciInputIOBlk and writes the */
/*           new stream to that pointed to be pcviOutputIOBlk.               */
/* ************************************************************************* */
VCI_RETCODE vciCombineBitStreamSC(const void *pVciCodecMemBlk,
                                  const VCI_CODEC_IO_BLOCK *pvciInputAIOBlk,
                                  const VCI_CODEC_IO_BLOCK *pvciInputBIOBlk,
                                  VCI_CODEC_IO_BLOCK *pvciOutputIOBlk,
                                  unsigned long  ScCodecA,
                                  unsigned long  ScCodecB)
{
   const VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   unsigned short wBytesA, wBytesB, wBytesOut, wNumOfFrames, i, j;
   short wNumOfBytesInBuffer;
   unsigned short wReadA, wReadB;
   const VCI_CODEC_IO_BLOCK *pCodecIOTempBlk;
   unsigned long ScCodecTemp;
   const unsigned short *pStreamTable = NULL;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pVciCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pVciCodecMemBlk; 

   VCI_EXTRACT_IO_BLK_PARAM_CHECK(pvciInputAIOBlk);
   VCI_EXTRACT_IO_BLK_PARAM_CHECK(pvciInputBIOBlk);
   VCI_EXTRACT_IO_BLK_PARAM_CHECK(pvciOutputIOBlk);

   if(vciCodecMblk->pVciPlusMethodStruct)
      if(vciCodecMblk->pVciPlusMethodStruct->wStreamTable)
         pStreamTable = vciCodecMblk->pVciPlusMethodStruct->wStreamTable;

   
   if(ScCodecA&ScCodecB)
      return VCI_BITSTREAM_NOT_SUPPORTED;

   if(!isValidStream((ScCodecA|ScCodecB), pStreamTable))
      return VCI_BITSTREAM_NOT_SUPPORTED;

   /* maybe we have to do a variable shuffle */
   if(!(ScCodecA&0x01)) {
      ScCodecTemp = ScCodecA;
      ScCodecA    = ScCodecB;
      ScCodecB    = ScCodecTemp;

      pCodecIOTempBlk = pvciInputAIOBlk;
      pvciInputAIOBlk = pvciInputBIOBlk;
      pvciInputBIOBlk = pCodecIOTempBlk;
   }

   wBytesA=bytesInStream(ScCodecA, pStreamTable);
   wBytesB=bytesInStream(ScCodecB, pStreamTable);
   wBytesOut=(unsigned short)(wBytesA+wBytesB);

   /* the following block etablishes the number of itterations that we'll have to do */
   wNumOfBytesInBuffer=
      (short)(pvciInputAIOBlk->wVoxBufferWriteByteOffset
              -pvciInputAIOBlk->wVoxBufferReadByteOffset);

   if(wNumOfBytesInBuffer<=0)
      wNumOfBytesInBuffer=(short)(wNumOfBytesInBuffer + pvciInputAIOBlk->wVoxBufferSize);

   if(wNumOfBytesInBuffer%wBytesA) /* shouldn't have any left over bits */
      return VCI_BITSTREAM_CORRUPTED;

   wNumOfFrames=(unsigned short)(wNumOfBytesInBuffer/wBytesA);

   /* checking B buffer */
   wNumOfBytesInBuffer=
      (short)(pvciInputBIOBlk->wVoxBufferWriteByteOffset
             -pvciInputBIOBlk->wVoxBufferReadByteOffset);

   if(wNumOfBytesInBuffer<=0)
      wNumOfBytesInBuffer=(short)(wNumOfBytesInBuffer+pvciInputBIOBlk->wVoxBufferSize);

   if(wNumOfBytesInBuffer%wBytesB) /* shouldn't have any left over bytes */
      return VCI_BITSTREAM_CORRUPTED;

   if(wNumOfFrames!=wNumOfBytesInBuffer/wBytesB)
      return VCI_BITSTREAM_INSUFFICIENT; /* Num of frames doesn't match with A */

   /* lastly checking C buffer */
   wNumOfBytesInBuffer=
      (short)(pvciOutputIOBlk->wVoxBufferWriteByteOffset
              -pvciOutputIOBlk->wVoxBufferReadByteOffset);

   if(wNumOfBytesInBuffer<=0)
      wNumOfBytesInBuffer=(short)(wNumOfBytesInBuffer+pvciOutputIOBlk->wVoxBufferSize);

   if(wNumOfBytesInBuffer/wBytesOut<wNumOfFrames)
      return VCI_BITSTREAM_INSUFFICIENT;
   else if(wNumOfBytesInBuffer/wBytesOut==wNumOfFrames)
      if(!(wNumOfBytesInBuffer%wBytesOut)) /* need at least one extra byte!! */
         return VCI_BITSTREAM_INSUFFICIENT;
  
   wReadA=pvciInputAIOBlk->wVoxBufferReadByteOffset;
   wReadB=pvciInputBIOBlk->wVoxBufferReadByteOffset;

   for(i=0;i<wNumOfFrames;i++) {
      for(j=0;j<wBytesA;j++) {
         pvciOutputIOBlk->pVoxBuffer[pvciOutputIOBlk->wVoxBufferWriteByteOffset++]=
            pvciInputAIOBlk->pVoxBuffer[wReadA++];

         if(pvciOutputIOBlk->wVoxBufferWriteByteOffset>=pvciOutputIOBlk->wVoxBufferSize)
            pvciOutputIOBlk->wVoxBufferWriteByteOffset=
                (unsigned short)(pvciOutputIOBlk->wVoxBufferWriteByteOffset-
                                 pvciOutputIOBlk->wVoxBufferSize);

         if(wReadA>=pvciInputAIOBlk->wVoxBufferSize)
            wReadA=(unsigned short)(wReadA-pvciInputAIOBlk->wVoxBufferSize);
      }
      /* set the continuation bit */
      if(pvciOutputIOBlk->wVoxBufferWriteByteOffset)
         pvciOutputIOBlk->pVoxBuffer[pvciOutputIOBlk->wVoxBufferWriteByteOffset-1]|=0x080;
      else
         pvciOutputIOBlk->pVoxBuffer[pvciOutputIOBlk->wVoxBufferSize-1]|=0x080;

      for(j=0;j<wBytesB;j++) {
         pvciOutputIOBlk->pVoxBuffer[pvciOutputIOBlk->wVoxBufferWriteByteOffset++]=
            pvciInputBIOBlk->pVoxBuffer[wReadB++];

         if(pvciOutputIOBlk->wVoxBufferWriteByteOffset>=pvciOutputIOBlk->wVoxBufferSize)
            pvciOutputIOBlk->wVoxBufferWriteByteOffset=
               (unsigned short)(pvciOutputIOBlk->wVoxBufferWriteByteOffset-
                                pvciOutputIOBlk->wVoxBufferSize);

         if(wReadB>=pvciInputBIOBlk->wVoxBufferSize)
            wReadB=(unsigned short)(wReadB-pvciInputBIOBlk->wVoxBufferSize);
      }
   }
   return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetRatesInFrameSC()                                          */
/* PURPOSE : Returns the highest rate codec that is present in the           */
/*           bit-stream for the frame ponted to by the read offset pointer.  */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetRatesInFrameSC(const void *pVciCodecMemBlk,
                                 const VCI_CODEC_IO_BLOCK *pvciCodecIOBlk,
                                 unsigned long *ScCodec)  
{
   short wSeekReturn, wSeekInitial, wBytesSeeked;
   unsigned long dwAccumulated=0;
   const VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   const unsigned short *pStreamTable = NULL;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pVciCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

    /* Do parameter checking on IO block. */
   VCI_IO_BLK_PARAM_CHECK(pvciCodecIOBlk);

   vciCodecMblk = (VCI_CODEC_BLOCK *)pVciCodecMemBlk; 

   if(vciCodecMblk->pVciPlusMethodStruct)
      if(vciCodecMblk->pVciPlusMethodStruct->wStreamTable)
         pStreamTable = vciCodecMblk->pVciPlusMethodStruct->wStreamTable;

   wSeekReturn=pvciCodecIOBlk->wVoxBufferReadByteOffset;

   while(wSeekReturn != pvciCodecIOBlk->wVoxBufferWriteByteOffset) {

      wSeekInitial = wSeekReturn;
      
      wSeekReturn = seekForward(pvciCodecIOBlk->pVoxBuffer,
                  wSeekInitial,
                  pvciCodecIOBlk->wVoxBufferWriteByteOffset,
                  pvciCodecIOBlk->wVoxBufferSize,
                  1, pStreamTable);

      if(wSeekReturn < 0)
         return VCI_BITSTREAM_CORRUPTED;

      wBytesSeeked = (short)(wSeekReturn - wSeekInitial);

      if(wBytesSeeked < 0) 
         wBytesSeeked = (short)(wBytesSeeked + pvciCodecIOBlk->wVoxBufferSize);

      dwAccumulated|=bytesToStream(wBytesSeeked, pStreamTable);
   } 

   *ScCodec=dwAccumulated;

   return VCI_NO_ERROR;
}

#ifndef DECODE_ONLY
/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetEncoderRateSC()                                           */
/* PURPOSE : Switches the encoder rate on the fly (depends upon              */
/*           initialization)                                                 */
/*                                                                           */
/* NOTE    : looking for a rate or a valid stream!                           */
/* ************************************************************************* */
VCI_RETCODE vciSetEncoderRateSC(const void *pEncodeMemBlk, unsigned long wBitField)  
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   unsigned long wTemp=wBitField;
   unsigned long wMask=~wTemp;
   const unsigned short *pStreamTable = NULL;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pEncodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_ENCODE(pEncodeMemBlk))
      return VCI_NOT_ENC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pEncodeMemBlk;

   if(vciCodecMblk->pVciPlusMethodStruct)
      if(vciCodecMblk->pVciPlusMethodStruct->wStreamTable)
         pStreamTable = vciCodecMblk->pVciPlusMethodStruct->wStreamTable;

   if(!isValidStream(wBitField, pStreamTable))
      return VCI_BITSTREAM_NOT_SUPPORTED;

   /* adjusting rate to assure that it's a single rate */
   while(wTemp>>=1) {
      wMask=(wMask>>1)|0x8000;
      wBitField&=wMask;
   }

   if (wBitField == 0)
     return VCI_RATE_INVALID;

   if(voxSetEncodeRate(vciCodecMblk->hCodecMblk, wBitField)!=wBitField)
      return VCI_RATE_INVALID;
   else
      return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetEncoderRateSC()                                           */
/* PURPOSE : Returns the current encoder rate being used                     */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetEncoderRateSC(const void *pEncodeMemBlk, 
                                unsigned long *wBitField) 
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   unsigned short wTemp;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(pEncodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_ENCODE(pEncodeMemBlk))
      return VCI_NOT_ENC_HANDLE;

   assert(wBitField != NULL);

   vciCodecMblk = (VCI_CODEC_BLOCK *)pEncodeMemBlk;
   *wBitField=wTemp=(unsigned short)voxGetEncodeRate(vciCodecMblk->hCodecMblk);

   /* adjust rate so that it becomes a valid bit field */
   do
      *wBitField|=(wTemp>>=1);
   while (wTemp);

   return VCI_NO_ERROR;
}
#endif

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetValideRateSC()                                            */
/* PURPOSE : Returns the current either encoder rate or decoder rate         */
/*           being used                                                      */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetValidRatesSC(const void *pCodecMemBlk, 
                               unsigned long *wBitField)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   if(!VCI_BLOCK_CHECK(pCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   assert(wBitField != NULL);
   vciCodecMblk = (VCI_CODEC_BLOCK *)pCodecMemBlk;

   switch(vciCodecMblk->chEncOrDec) {
#ifndef DECODE_ONLY
      case ENCODE_ID:
         *wBitField=voxGetEncodeInitRate(vciCodecMblk->hCodecMblk);
         break;
#endif

      case DECODE_ID:
         *wBitField=voxGetDecodeInitRateSC(vciCodecMblk->hCodecMblk);
         break;

      default:
         *wBitField=0;
         assert(0);
         return VCI_INCOMPATIBLE_HANDLE;
   }
   return VCI_NO_ERROR;
}


/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciCalculateBitRate()                                           */
/* PURPOSE : Returns the bitrate calculated from the rate stream passed in   */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciCalculateBitRateSC(const void *pCodecMemBlk, unsigned long wBitField, 
                                  unsigned short *pwBitRate)
{
   short wBytesInStream=0;
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   const unsigned short *pStreamTable = NULL;

   if(!VCI_BLOCK_CHECK(pCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pCodecMemBlk;

   if(vciCodecMblk->pVciPlusMethodStruct)
      if(vciCodecMblk->pVciPlusMethodStruct->wStreamTable)
         pStreamTable = vciCodecMblk->pVciPlusMethodStruct->wStreamTable;

   if((wBytesInStream=bytesInStream(wBitField, pStreamTable))==-1)
      return VCI_BITSTREAM_CORRUPTED;

   *pwBitRate=(unsigned short)((float)RATE8K*(float)(wBytesInStream*8)/(float)FRAME_LENGTH_SC);

   return VCI_NO_ERROR;
}

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciCalculateBitField()                                          */
/* PURPOSE : Returns the bitrate calculated from the rate stream passed in   */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciCalculateRateSC(const void *pCodecMemBlk, unsigned short wBitRate, 
                                   unsigned long *wBitField)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   float rateBytes=(float)wBitRate*((float)FRAME_LENGTH_SC/((float)RATE8K*8));
   const unsigned short *pStreamTable = NULL;

   if(!VCI_BLOCK_CHECK(pCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pCodecMemBlk;   

   if(vciCodecMblk->pVciPlusMethodStruct)
      if(vciCodecMblk->pVciPlusMethodStruct->wStreamTable)
         pStreamTable = vciCodecMblk->pVciPlusMethodStruct->wStreamTable;

   *wBitField = bytesToStream((unsigned short)rateBytes, pStreamTable);

   return VCI_NO_ERROR;
}


/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciPlusInitSC()                                                 */
/* PURPOSE : Initialize pVciPlusMethodStruct with all the plus functions     */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciPlusInitSC(const void *pCodecMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   if(!VCI_BLOCK_CHECK(pCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pCodecMemBlk;

   if(VOX_MEM_INIT(vciCodecMblk->pVciPlusMethodStruct,1,sizeof(VCI_PLUS_METHOD_STRUCT)))
      return VCI_FAILED_MEM_ALLOC;

   vciCodecMblk->pVciPlusMethodStruct->pfVciDecodeNeedsInput=vciDecodeNeedsInputSC;
   vciCodecMblk->pVciPlusMethodStruct->pfVciSetWarpFactor=vciSetWarpFactorSC;
   vciCodecMblk->pVciPlusMethodStruct->pfVciGetWarpFactor=vciGetWarpFactorSC;

   vciCodecMblk->pVciPlusMethodStruct->pfVciSetInjectSilenceFlag=vciSetInjectSilenceFlagSC;

   vciCodecMblk->pVciPlusMethodStruct->pfVciGetEnergy=vciGetEnergyLevelSC;   
  
   vciCodecMblk->pVciPlusMethodStruct->pfVciNumFramesInBuffer=vciNumFramesInBufferSC;
   vciCodecMblk->pVciPlusMethodStruct->pfVciSeekForward=vciSeekForwardSC;

   vciCodecMblk->pVciPlusMethodStruct->pfVciSetFramesSkipped=vciSetFramesSkippedSC;
   vciCodecMblk->pVciPlusMethodStruct->pfVciSetRepeatFrameFlag=vciSetRepeatFrameFlagSC;


   vciCodecMblk->pVciPlusMethodStruct->pfVciExtractFromBitStream=vciExtractFromBitStreamSC;

   vciCodecMblk->pVciPlusMethodStruct->pfVciCombineBitStream=vciCombineBitStreamSC;

   vciCodecMblk->pVciPlusMethodStruct->pfVciGetRatesInFrame=vciGetRatesInFrameSC;

#ifndef DECODE_ONLY
   vciCodecMblk->pVciPlusMethodStruct->pfVciSetEncoderRate=vciSetEncoderRateSC;
   vciCodecMblk->pVciPlusMethodStruct->pfVciGetEncoderRate=vciGetEncoderRateSC; 
#endif

   vciCodecMblk->pVciPlusMethodStruct->pfVciGetValidRates=vciGetValidRatesSC;

   vciCodecMblk->pVciPlusMethodStruct->pfVciCalculateBitRate=vciCalculateBitRateSC;
   vciCodecMblk->pVciPlusMethodStruct->pfVciCalculateRateSC=vciCalculateRateSC;

   vciCodecMblk->pVciPlusMethodStruct->wStreamTable = NULL;

   return VCI_NO_ERROR;
}


/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciPlusFreeSC()                                                 */
/* PURPOSE : Free pVciPlusMethodStruct                                       */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciPlusFreeSC(const void *pCodecMemBlk)
{

   if(!VCI_BLOCK_CHECK(pCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   VOX_MEM_FREE(((VCI_CODEC_BLOCK *)pCodecMemBlk)->pVciPlusMethodStruct);

   return VCI_NO_ERROR;
}

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciGetPlusInfoSC()                                               */
/* PURPOSE : Fills in the VCI plus info blk                                   */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciGetPlusInfoSC(VCI_PLUS_INFO_BLOCK *pVciPlusInfoBlk)
{
   pVciPlusInfoBlk->wPlusFlags=VCI_CODEC_HAS_WARPING; 
   pVciPlusInfoBlk->wPlusFlags|=VCI_CODEC_HAS_SILENCE_INJECTION;
   pVciPlusInfoBlk->wPlusFlags|=VCI_CODEC_HAS_GET_ENERGY;
   pVciPlusInfoBlk->wPlusFlags|=VCI_CODEC_HAS_FRAME_TRAVERSAL;
   pVciPlusInfoBlk->wPlusFlags|=VCI_PRIME_FLUSH;
   pVciPlusInfoBlk->wPlusFlags|=VCI_STREAM_MANIPULATION;
   pVciPlusInfoBlk->wPlusFlags|=VCI_CODEC_IS_SCALEABLE;
   pVciPlusInfoBlk->dwMinWarp=MIN_WARP;
   pVciPlusInfoBlk->dwMaxWarp=MAX_WARP;

   return VCI_NO_ERROR;
}



