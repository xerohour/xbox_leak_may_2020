/******************************************************************************
 *                                                                            *
 *                      Voxware Proprietary Material                          *
 *                      Copyright 1996, Voxware, Inc.                         *
 *                           All Rights Resrved                               *
 *                                                                            *
 *                     DISTRIBUTION PROHIBITED without                        *
 *                   written authorization from Voxware                       *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
 *                                                                            *
 * FILENAME:  vciVR12.c                                                       *
 * PURPOSE:   Defines the interface functions for Voxware's variable rate cdc *                                             *
 * AUTHOR:    Ilan Berci, Epiphany Vera   97/05/27                            *
 *                                                                            *
 *****************************************************************************/

/** ************************************************************************ **/
/**                              Standard headers                            **/
/** ************************************************************************ **/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>


/** ************************************************************************ **/
/**                               Codec Headers                              **/
/** ************************************************************************ **/
#include "VoxMem.h"
#include "model.h"
#include "quanvr12.h"
#include "vem2.h"

#include "Pack12.h"
#include "Quan12.h"
#include "EncodeRT.h"
#include "DecodeRT.h"
#include "FadeIO.h"
#include "QTable12.h" /* For Quantization types */
#include "vCodecID.h"

/** ************************************************************************ **/
/**                     Voxware Core Interface Headers                       **/
/** ************************************************************************ **/
#include "vciError.h"   /* VCI Specific errors.                               */
#include "vci.h"
#include "vciPriv.h"    /* Voxware Core Interface (VCI) private definations   */
#include "vciPlus.h"

#include "vemFrm12.h"
#include "vciVR12.h"

#include "xvocver.h"

/** ************************************************************************ **/
/**                     Voxware Core Interface Static Functions              **/
/** ************************************************************************ **/
static short DropFrames(float InWarp, float *OutWarp, short *DropLeft);
static unsigned short initMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct);
static unsigned short freeMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct);

/** ************************************************************************ **/
/**                     Voxware Core Interface PreDefines                    **/
/** ************************************************************************ **/
#define ON          1
#define OFF         0
#define STRETCH_IT  1
#define DROP_IT     0
/*============================================================================*/

#define PRIME_FLUSH_SAMP   90    /* delay: dec: 90 + end: 180 ==> 360-180-90 = 90 */
#define PRIME_FLUSH_FRAME  2     /* 360 samples total delay if we count prime flush samples*/
#define PACKET_BITS_VR12   64    /* The number of bits in a bit-stream packet  */
#define PACKET_BYTE_VR12   8     /* The minimum bytes required to store one    */
                                 /* bit-stream packet.                         */
                                 
/** ************************************************************************ **/
/**                            Private Prototypes                            **/
/** ************************************************************************ **/ 
                                
unsigned  short VR12CompatibilityFunc(unsigned short VemPlugin, 
                                      unsigned long * pVemCodID);
                                                                       
/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciGetInfoVR12()                                                 */
/* PURPOSE : Encodes one frame of speech.                                     */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciGetInfoVR12(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk)
{
   float avebits;

   if(!pvciCodecInfoBlk)
      return VCI_NULL_ARG;

   pvciCodecInfoBlk->wVCIVersion               = (unsigned short) VCI_VERSION;
   
   /* Variables for codec identification.     */
   strcpy(pvciCodecInfoBlk->sCodecVersion,     "RT.03.01.00");         
   pvciCodecInfoBlk->dwCodecClassID            = (unsigned long)  VOXWARE_CODEC_RT_8K_VR12;      
   pvciCodecInfoBlk->dwCodecID                 = (unsigned long)  VOXWARE_CODEC_ID_RT_8K_VR12;
   
   /* PCM Data formats.                       */
   pvciCodecInfoBlk->wSamplingRate             = (unsigned short) RATE8K;   
   pvciCodecInfoBlk->wNumBitsPerSample         = 16; 
   pvciCodecInfoBlk->wNumSamplesPerFrame       = (unsigned short) FRAME_LEN;
   
   /* Multi-channel support.                  */
   pvciCodecInfoBlk->wNumChannels              = 1;
   pvciCodecInfoBlk->wInterleaveBlockSize      = 1;

   /* Bitstream information.                  */
   avebits =  (TOTAL_BITS_VR12_SILENCE*RATE_S + TOTAL_BITS_VR12_UNVOICED*RATE_UV 
             + TOTAL_BITS_VR12_VOICED*RATE_V + TOTAL_BITS_VR12_MIXED*RATE_MIX);
   pvciCodecInfoBlk->dwBitRate                  = (unsigned long) (avebits*RATE8K/((float)FRAME_LEN));
   pvciCodecInfoBlk->dwMinPacketBits            = (unsigned long) TOTAL_BITS_VR12_SILENCE;
   pvciCodecInfoBlk->dwAvgPacketBits            = (unsigned long) avebits;
   pvciCodecInfoBlk->dwMaxPacketBits            = (unsigned long) TOTAL_BITS_VR12_MIXED;

   /* Prime Flush Information	*/
   pvciCodecInfoBlk->wNumPrimeFlushFrames      = (unsigned short) PRIME_FLUSH_FRAME;
   pvciCodecInfoBlk->wNumPrimeFlushSamples     = (unsigned short) PRIME_FLUSH_SAMP;    
   return(VCI_NO_ERROR);
}
/*============================================================================*/



/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciEncodeVR12()                                                  */
/* PURPOSE : Encodes one frame of speech.                                     */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciEncodeVR12(void *hCodecMemBlk, VCI_CODEC_IO_BLOCK 
                          *hvciEncodeIOBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   STACK_DECLARE
   
   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_ENCODE(hCodecMemBlk))
      return VCI_NOT_ENC_HANDLE;

   VCI_IO_BLK_PARAM_CHECK(hvciEncodeIOBlk);

   vciCodecMblk = (VCI_CODEC_BLOCK *)hCodecMemBlk;
   STACK_ASSIGN(vciCodecMblk->pScratchBlk)
   
   vciCodecMblk->pVciCodecIOBlock=hvciEncodeIOBlk;

   VoxEncodeRT(STACK_S hvciEncodeIOBlk->pPCMBuffer,vciCodecMblk->hCodecMblk);
   
   VoxQuanVR12(vciCodecMblk->pQuanMemBlk, 
             ((ENC_MEM_BLK *)(vciCodecMblk->hCodecMblk))->Param_mblk);

   VoxPackVR12(vciCodecMblk->pQuanMemBlk, hvciEncodeIOBlk->pVoxBuffer, 
      &(hvciEncodeIOBlk->wVoxBufferWriteByteOffset),
      &(hvciEncodeIOBlk->chVoxBufferWriteBitOffset), 
      hvciEncodeIOBlk->wVoxBufferSize);

   return(VCI_NO_ERROR);
}
/*============================================================================*/



/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciDecodeVR12()                                                  */
/* PURPOSE : Decodes one frame of speech.                                     */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciDecodeVR12(void *hCodecMemBlk, VCI_CODEC_IO_BLOCK 
                          *hvciDecodeIOBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   DEC_MEM_BLK     *DecMemBlk    = NULL;
   STACK_DECLARE
   FadeIOstruct    *pFadeBlk     = NULL;
   unsigned short  TempFrames;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(hCodecMemBlk))
      return VCI_NOT_DEC_HANDLE;
   
   VCI_IO_BLK_PARAM_CHECK(hvciDecodeIOBlk);

   vciCodecMblk = (VCI_CODEC_BLOCK *)hCodecMemBlk;
   DecMemBlk= ((DEC_MEM_BLK*)(vciCodecMblk->hCodecMblk));

   pFadeBlk = (FadeIOstruct *)(DecMemBlk->pFadeIOMemBlk);

   STACK_ASSIGN(vciCodecMblk->pScratchBlk)
   vciCodecMblk->pVciCodecIOBlock=hvciDecodeIOBlk;

  if((DecMemBlk->unpackFLAG==ON)&&(pFadeBlk->CurrentFrameState))
  {
      framesInBufferVR12(hvciDecodeIOBlk, &TempFrames);
      if (TempFrames == 0)
         return VCI_INSUFFICIENT_VOX_DATA;

       VoxUnPackVR12(vciCodecMblk->pQuanMemBlk, hvciDecodeIOBlk->pVoxBuffer, 
          &(hvciDecodeIOBlk->wVoxBufferReadByteOffset), 
          &(hvciDecodeIOBlk->chVoxBufferReadBitOffset), 
          hvciDecodeIOBlk->wVoxBufferSize);
  }

  if (!DropFrames(DecMemBlk->fWarpedLengthFactor,&DecMemBlk->Warp,&DecMemBlk->dropLeft)) {
     VoxUnQuanVR12(vciCodecMblk->pQuanMemBlk, DecMemBlk->Param_mblk);

     VoxFadeIO(pFadeBlk, DecMemBlk->Param_mblk);

     VoxDecodeRT(STACK_S hvciDecodeIOBlk->pPCMBuffer,DecMemBlk);

     if (!DecMemBlk->unpackFLAG) {
       if (pFadeBlk->LastFrameState == SILENCE)
	 pFadeBlk->CurrentFrameState = SILENCE;
     }

     if(DecMemBlk->writeFLAG) 
        hvciDecodeIOBlk->wSamplesInPCMBuffer = FRAME_LEN;
     else  
        hvciDecodeIOBlk->wSamplesInPCMBuffer = 0;
      
  } else {
     DecMemBlk->writeFLAG=0;
     hvciDecodeIOBlk->wSamplesInPCMBuffer = 0;
  } 

  return(VCI_NO_ERROR);
}
/*============================================================================*/



/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciInitEncodeVR12()                                              */
/* PURPOSE : To allocate memory for a Codec Memory Block and to initialize it */
/*           as an encoder.                                                   */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciInitEncodeVR12(void **hCodecMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk;
   STACK_DECLARE

   if(!hCodecMemBlk)
      return VCI_NULL_ARG;

   if(VOX_MEM_INIT(vciCodecMblk=*hCodecMemBlk,1,sizeof(VCI_CODEC_BLOCK))) 
     return VCI_FAILED_MEM_ALLOC;

   vciCodecMblk->chEncOrDec = ENCODE_ID;
   vciCodecMblk->dwVciChunkID = SANITY_CHECK;
   vciCodecMblk->dwCodecClassID = VOXWARE_CODEC_RT_8K_VR12;  
   vciCodecMblk->dwCodecID = VOXWARE_CODEC_ID_RT_8K_VR12; 

   if(!(vciCodecMblk->pvemMemBlk=vemInit())) {
#if VEM_DEFINED == 1
      return VCI_FAILED_MEM_ALLOC;
#endif
   }

   if(VoxInitEncodeRT(&vciCodecMblk->hCodecMblk,vciCodecMblk->pvemMemBlk)) {
      vciFreeEncodeVR12(hCodecMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }

   if(VoxInitQuanVR12(&vciCodecMblk->pQuanMemBlk, vciCodecMblk->pvemMemBlk)) {
      vciFreeEncodeVR12(hCodecMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }

   if(STACK_SET(vciCodecMblk->pScratchBlk,10240)) {
      vciFreeEncodeVR12(hCodecMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }

   if(initMethodStruct(&vciCodecMblk->pVciMethodStruct))
      return VCI_FAILED_MEM_ALLOC;

   return VCI_NO_ERROR;
}
/*============================================================================*/



/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciInitDecodeVR12()                                              */
/* PURPOSE : To allocate memory for a Codec Memory Block and to initialize it */
/*           as a decoder.                                                    */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciInitDecodeVR12(void **hCodecMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk;
   DEC_MEM_BLK     *DecMemBlk = NULL;

   STACK_DECLARE

   if(!hCodecMemBlk)
      return VCI_NULL_ARG;

   if(VOX_MEM_INIT(vciCodecMblk=*hCodecMemBlk,1,sizeof(VCI_CODEC_BLOCK))) 
      return VCI_FAILED_MEM_ALLOC;

   vciCodecMblk->chEncOrDec = DECODE_ID;
   vciCodecMblk->dwVciChunkID = SANITY_CHECK;
   vciCodecMblk->dwCodecClassID = VOXWARE_CODEC_RT_8K_VR12;  
   vciCodecMblk->dwCodecID = VOXWARE_CODEC_ID_RT_8K_VR12; 

   if(!(vciCodecMblk->pvemMemBlk=vemInit())) {
#if VEM_DEFINED == 1
      return VCI_FAILED_MEM_ALLOC;
#endif
   }

   if(VoxInitDecodeRT(&vciCodecMblk->hCodecMblk,vciCodecMblk->pvemMemBlk)) {
      vciFreeDecodeVR12(hCodecMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   } 

   if(VoxInitQuanVR12(&vciCodecMblk->pQuanMemBlk, vciCodecMblk->pvemMemBlk)) {     
      vciFreeDecodeVR12(hCodecMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   } 

   DecMemBlk = (DEC_MEM_BLK *)vciCodecMblk->hCodecMblk;
   if (VoxinitFadeIO(&DecMemBlk->pFadeIOMemBlk)) {
      vciFreeDecodeVR12(hCodecMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }

   if(STACK_SET(vciCodecMblk->pScratchBlk,4096)) {
      vciFreeDecodeVR12(hCodecMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   } 

   if(initMethodStruct(&vciCodecMblk->pVciMethodStruct))
      return VCI_FAILED_MEM_ALLOC;

   ((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->fWarpedLengthFactor=1.0F;
   ((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->Warp=1.0F;
   ((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->dropRate=1;
   ((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->writeFLAG=1;
   ((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->unpackFLAG=1;
   ((DEC_MEM_BLK*)vciCodecMblk->hCodecMblk)->dropLeft=0;

   return VCI_NO_ERROR;
}
/*============================================================================*/




/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciFreeEncodeVR12()                                              */
/* PURPOSE : Frees memory assigned to the encoder.                            */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciFreeEncodeVR12(void **hCodecMemBlk)
{  
   VCI_CODEC_BLOCK *vciCodecMblk   = NULL;

   if(!hCodecMemBlk)
      return VCI_NULL_ARG;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(*hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_ENCODE(*hCodecMemBlk))
      return VCI_NOT_ENC_HANDLE;
   
   vciCodecMblk = (VCI_CODEC_BLOCK *)*hCodecMemBlk;
   STACK_UNSET(vciCodecMblk->pScratchBlk)

   vciCodecMblk->dwVciChunkID=0;

   vemFree(vciCodecMblk->pvemMemBlk);

   if(vciCodecMblk->hCodecMblk) {
      VoxFreeEncodeRT(&vciCodecMblk->hCodecMblk);
      VoxFreeQuanVR12(&vciCodecMblk->pQuanMemBlk);
   }

   VOX_MEM_FREE(vciCodecMblk->hCodecMblk);

   freeMethodStruct(&vciCodecMblk->pVciMethodStruct);

   VOX_MEM_FREE(*hCodecMemBlk);

   return VCI_NO_ERROR;
}
/*============================================================================*/




/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciFreeDecodeVR12()                                              */
/* PURPOSE : Frees memory assigned to the decoder.                            */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciFreeDecodeVR12(void **hCodecMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   DEC_MEM_BLK *DecMemBlk;

   if(!hCodecMemBlk)
      return VCI_NULL_ARG;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(*hCodecMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(*hCodecMemBlk))
      return VCI_NOT_DEC_HANDLE;
   
   vciCodecMblk = (VCI_CODEC_BLOCK *)*hCodecMemBlk;
   STACK_UNSET(vciCodecMblk->pScratchBlk)

   vciCodecMblk->dwVciChunkID=0;

   vemFree(vciCodecMblk->pvemMemBlk);

   if(vciCodecMblk->hCodecMblk) {
      DecMemBlk = (DEC_MEM_BLK *)vciCodecMblk->hCodecMblk;
      VoxfreeFadeIO(&DecMemBlk->pFadeIOMemBlk);
      VoxFreeDecodeRT(&vciCodecMblk->hCodecMblk);
      VoxFreeQuanVR12(&vciCodecMblk->pQuanMemBlk);
   }

   freeMethodStruct(&vciCodecMblk->pVciMethodStruct);

   VOX_MEM_FREE(*hCodecMemBlk);

  return VCI_NO_ERROR;
}

/*============================================================================*/

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: DropFrames()                                                     */
/* PURPOSE : Decides on whether to drop a frame                               */
/*                                                                            */
/* ************************************************************************** */
short DropFrames(float InWarp, float *OutWarp, short *DropLeft)
{
   short dropit;

   if (*DropLeft==0) {
      dropit = 0;
      if (InWarp >=1.0)
         *DropLeft = 0;
      else 
         *DropLeft = ((short)floor(1.0/InWarp))-1;

     *OutWarp = InWarp*((*DropLeft)+1);
   } else  {
      dropit=1;
     (*DropLeft)--;
   }

   return(dropit);
}

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciGetExtCompatibilityVR12()                                     */
/* PURPOSE : Returns function pointer to the function vemIsCompatible         */
/*                                                                            */
/* ************************************************************************** */
unsigned  short VR12CompatibilityFunc(unsigned short VemPlugin, 
                                      unsigned long * pVemCodID) 
{
   *pVemCodID=(unsigned long)VOXWARE_CODEC_RT_8K_VR12;
   
   switch(VemPlugin) {
      case COMPATIBILITY_VFONT:
         return (unsigned short)20;

      case COMPATIBILITY_VAD_AGC:
         return (unsigned short)20;

      case COMPATIBILITY_PLUS: 
         return (unsigned short)20;
   }
   return 0;
}

VCI_RETCODE vciGetExtCompatibilityVR12(void **pCompatFunc)
{
   if(!pCompatFunc)
      return VCI_NULL_ARG;

   *((COMPATIBILITY_FUNCTION **)pCompatFunc)=&VR12CompatibilityFunc;

   return VCI_NO_ERROR;
}

static unsigned short initMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct)
{
   if(VOX_MEM_INIT(*hVciMethodStruct,1,sizeof(VCI_METHOD_STRUCT)))
      return 1;

   (*hVciMethodStruct)->pfVciGetInfo=vciGetInfoVR12;

   (*hVciMethodStruct)->pfVciEncode=vciEncodeVR12;
   (*hVciMethodStruct)->pfVciDecode=vciDecodeVR12;

   (*hVciMethodStruct)->pfVciInitEncode=vciInitEncodeVR12;
   (*hVciMethodStruct)->pfVciInitDecode=vciInitDecodeVR12; 
                                                      
   (*hVciMethodStruct)->pfVciFreeEncode=vciFreeEncodeVR12;                                                       
   (*hVciMethodStruct)->pfVciFreeDecode=vciFreeDecodeVR12;  

   return 0;
}

static unsigned short freeMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct)
{
   if(hVciMethodStruct)
      VOX_MEM_FREE(*hVciMethodStruct);

   return 0;
}

