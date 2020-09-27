/******************************************************************************
 *                                                                            *
 *		                  Voxware Proprietary Material                          *
 *		                  Copyright 1996, Voxware, Inc.                         *
 *		                       All Rights Resrved                               *
 *                                                                            *
 *		                 DISTRIBUTION PROHIBITED without                        *
 *		               written authorization from Voxware                       *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
 *                                                                            *
 * FILENAME:  vciPVR12.c                                                      *
 * PURPOSE:   Functions for the RT version of the VCI Plus Codec Extension.   *
 * AURTHORS:  John Puterbaugh, Ilan Berci & Epiphany Vera                     *
 *                                                                            *
 *****************************************************************************/

/******************************************************************************
 *                                                                            *
 *                          DETAILED REVISION HISTORY                         *
 *                                                                            *
 * (March 17, '97)                                                            *
 *    Created                                                                 *
 *                                                                            *
 *****************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "vci.h"
#include "vciPriv.h"
#include "vciError.h"
#include "vCodecID.h"

#include "VoxMem.h"
#include "vmEngLev.h"
#include "vemFrm12.h"
#include "DecodeRT.h"
#include "EncodeRT.h"
#include "param.h"
#include "FadeIO.h"
#include "Quan12.h"

#include "vciPlus.h"
#include "vciPVR12.h"

#include "xvocver.h"

#define MIN_WARP 0.2F
#define MAX_WARP 5.0F
#define DB2LEVEL_VALUE          28.3333333333F       /***** (255.0/9.0) *****/

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciSetWarpFactorVR12()                                           */
/* PURPOSE : Sets the warp factor for the decoder.                            */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciSetWarpFactorVR12(void *hCodecMemBlk, float wWarpFactor)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(hCodecMemBlk))
      return VCI_NOT_DEC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)hCodecMemBlk;

   if(wWarpFactor<MIN_WARP||wWarpFactor>MAX_WARP)
      return VCI_ARG_OUT_OF_RANGE;

   (((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->fWarpedLengthFactor)=wWarpFactor;

  return VCI_NO_ERROR;
}
/*============================================================================*/



/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciGetWarpFactorVR12()                                           */
/* PURPOSE : Retrieves the warp factor setting in the  decoder.               */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE   vciGetWarpFactorVR12(void *hCodecMemBlk, float *pwWarpFactor)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(hCodecMemBlk))
      return VCI_NOT_DEC_HANDLE;

   if(!pwWarpFactor)
      return VCI_NULL_ARG;

   vciCodecMblk = (VCI_CODEC_BLOCK *)hCodecMemBlk;

   *pwWarpFactor=(((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->fWarpedLengthFactor);
   
   return VCI_NO_ERROR;
}

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciDecodeNeedsInputVR12()                                        */
/* PURPOSE : Checks whether the decoder needs input or not.                   */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciDecodeNeedsInputVR12(void *hCodecMemBlk, 
                                    unsigned char *pUnPack)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(hCodecMemBlk))
      return VCI_NOT_DEC_HANDLE;

   if(!pUnPack)
      return VCI_NULL_ARG;

   vciCodecMblk = (VCI_CODEC_BLOCK *)hCodecMemBlk;

   *pUnPack=((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->unpackFLAG;

   return VCI_NO_ERROR;
}

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciGetEnergyVR12()                                               */
/* PURPOSE : Gets the frame energy                                            */
/*                                                                            */
/*  From energy to dB then map dB from 0 to 255                               */
/*  we map the maximum 90 dB to 255                                           */
/*  so level = 255/90*(10*log10(energy))                                      */
/* ************************************************************************** */
VCI_RETCODE vciGetEnergyVR12(void *hCodecMemBlk, short *pwEnergyLevel)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   float energy;
   *pwEnergyLevel=-1;

   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!pwEnergyLevel)
      return VCI_NULL_ARG;

   vciCodecMblk = (VCI_CODEC_BLOCK *)hCodecMemBlk;

   if (vciCodecMblk->chEncOrDec   == ENCODE_ID) {
      energy=*(((ENC_MEM_BLK*)(vciCodecMblk->hCodecMblk))->dwLpc);
      *pwEnergyLevel=(short)(log10(energy+1.0)* DB2LEVEL_VALUE);
   } else  /* hack from the old days ..GA */
      *pwEnergyLevel=(short)(((QUAN_12*)(vciCodecMblk->pQuanMemBlk))->Energy_INDEX);

   return VCI_NO_ERROR;
}

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciSetInjectSilenceFlagVR12()                                    */
/* PURPOSE : Perform fade-in/fade-out and background noise injection during   */
/*           packet loss.                                                     */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciSetInjectSilenceFlagVR12(void *hCodecMemBlk)
{
   DEC_MEM_BLK *DecMemBlk = NULL;

   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(hCodecMemBlk))
      return VCI_NOT_DEC_HANDLE;

   DecMemBlk = (DEC_MEM_BLK *)((VCI_CODEC_BLOCK *)hCodecMemBlk)->hCodecMblk;

   ((FadeIOstruct*)(DecMemBlk->pFadeIOMemBlk))->CurrentFrameState=0;

   return VCI_NO_ERROR;
}

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciNumFramesInBufferVR12()                                       */
/* PURPOSE : Counts the number of frames in the Vox buffer of the given codec */
/*           I/O block.                                                       */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciNumFramesInBufferVR12(const void  *hCodecMemBlk, 
                                 const VCI_CODEC_IO_BLOCK *hvciCodecIOBlk,
                                 unsigned short *pwNumFrames,
                                 unsigned short *pwByteOffset,
                                 unsigned char  *pchBitOffset)
{
	unsigned short  BitsInFrame;

   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   /* Do parameter checking on IO block. */
   VCI_IO_BLK_PARAM_CHECK(hvciCodecIOBlk);

   if(!pwNumFrames)
      return VCI_NULL_ARG;

   if(!pwByteOffset)
      return VCI_NULL_ARG;

   if(!pchBitOffset)
      return VCI_NULL_ARG;

   framesInBufferVR12(hvciCodecIOBlk, pwNumFrames);
         seekForwardVR12(hvciCodecIOBlk->wVoxBufferReadByteOffset, 
            hvciCodecIOBlk->chVoxBufferReadBitOffset, 
            hvciCodecIOBlk->wVoxBufferSize, 
            hvciCodecIOBlk->pVoxBuffer,
            *pwNumFrames, 
            pwByteOffset, 
            pchBitOffset, 
            &BitsInFrame);
       
   return VCI_NO_ERROR;
}




/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciSeekForwardVR12()                                             */
/* PURPOSE : Seeks to a given frame offset.                                   */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciSeekForwardVR12(const void  *hCodecMemBlk, 
                           const VCI_CODEC_IO_BLOCK *hvciCodecIOBlk,
                           unsigned short  wNumFrames,
                           unsigned short *pwByteOffset,
                           unsigned char  *pchBitOffset)
{
	unsigned short  BitsInFrame;
   unsigned short  TempFrames;

   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   /* Do parameter checking on IO block. */
   VCI_IO_BLK_PARAM_CHECK(hvciCodecIOBlk);

   if(!pwByteOffset)
      return VCI_NULL_ARG;

   if(!pchBitOffset)
      return VCI_NULL_ARG;

	framesInBufferVR12(hvciCodecIOBlk, &TempFrames);
   if (TempFrames < wNumFrames)
      return VCI_INSUFFICIENT_VOX_DATA;

   seekForwardVR12(hvciCodecIOBlk->wVoxBufferReadByteOffset, 
      hvciCodecIOBlk->chVoxBufferReadBitOffset, 
      hvciCodecIOBlk->wVoxBufferSize, 
      hvciCodecIOBlk->pVoxBuffer,
      wNumFrames, 
      pwByteOffset, 
      pchBitOffset, 
      &BitsInFrame);

   /* An else would be required here to deal with variable rate codecs.       */
   return VCI_NO_ERROR;
}

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciGetPlusInfoVR12()                                               */
/* PURPOSE : Fills in the VCI plus info blk                                   */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciGetPlusInfoVR12(VCI_PLUS_INFO_BLOCK *pVciPlusInfoBlk)
{

   if(!pVciPlusInfoBlk)
      return VCI_NULL_ARG;

   pVciPlusInfoBlk->wPlusFlags=VCI_CODEC_HAS_WARPING; 
   pVciPlusInfoBlk->wPlusFlags|=VCI_CODEC_HAS_SILENCE_INJECTION;
   pVciPlusInfoBlk->wPlusFlags|=VCI_CODEC_HAS_GET_ENERGY;
   pVciPlusInfoBlk->wPlusFlags|=VCI_CODEC_HAS_FRAME_TRAVERSAL;
   pVciPlusInfoBlk->wPlusFlags|=VCI_PRIME_FLUSH;

   pVciPlusInfoBlk->dwMinWarp=MIN_WARP;
   pVciPlusInfoBlk->dwMaxWarp=MAX_WARP;

   return VCI_NO_ERROR;
}

VCI_RETCODE vciPlusInitVR12(const void *pCodecMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   if(!VCI_BLOCK_CHECK(pCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pCodecMemBlk;

   if(VOX_MEM_INIT(vciCodecMblk->pVciPlusMethodStruct,1,sizeof(VCI_PLUS_METHOD_STRUCT)))
      return VCI_FAILED_MEM_ALLOC;

   vciCodecMblk->pVciPlusMethodStruct->pfVciDecodeNeedsInput=vciDecodeNeedsInputVR12;
   vciCodecMblk->pVciPlusMethodStruct->pfVciSetWarpFactor=vciSetWarpFactorVR12;
   vciCodecMblk->pVciPlusMethodStruct->pfVciGetWarpFactor=vciGetWarpFactorVR12;

   vciCodecMblk->pVciPlusMethodStruct->pfVciSetInjectSilenceFlag=vciSetInjectSilenceFlagVR12;

   vciCodecMblk->pVciPlusMethodStruct->pfVciGetEnergy=vciGetEnergyVR12;   
  
   vciCodecMblk->pVciPlusMethodStruct->pfVciNumFramesInBuffer=vciNumFramesInBufferVR12;
   vciCodecMblk->pVciPlusMethodStruct->pfVciSeekForward=vciSeekForwardVR12;

   return VCI_NO_ERROR;
}

VCI_RETCODE vciPlusFreeVR12(const void *pCodecMemBlk)
{
   if(!VCI_BLOCK_CHECK(pCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   VOX_MEM_FREE(((VCI_CODEC_BLOCK *)pCodecMemBlk)->pVciPlusMethodStruct);

   return VCI_NO_ERROR;
}
