/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:    vciSC3.c
*
* Purpose:     Defines the interface functions for the Voxware SC Quality codec
*
* Functions:   vciGetInfoSC3, vciEncodeSC3, vciDecodeSC3, 
*              vciInitEncodeSC3, vciInitDecodeSC3, vciFreeEncodeSC3,
*              vciFreeDecodeSC3, vciGetExtCompatibilitySC3
*
* Author/Date: Ilan Berci, Epiphany Vera 97/05/19
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   P:/r_and_d/archives/scaleable/vci/vciSC3.c_v   1.26   12 Oct 1998 10:23:50   johnp  $
******************************************************************************/

/** *********************************************************************** **/
/**                             Standard headers                            **/
/** *********************************************************************** **/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/** *********************************************************************** **/
/**                              Vci Headers                                **/
/** *********************************************************************** **/
#include "vci.h"
#include "vciError.h"
#include "vciPriv.h"
#include "vciPlus.h"
#include "vciPlsSC.h"

/** *********************************************************************** **/
/**                              Codec Headers                              **/
/** *********************************************************************** **/
#include "vLibPack.h"
#include "vem2Prm.h"
#include "VoxMem.h"
#include "vem2.h"
#include "codec.h"
#include "quansc36.h"
#include "DecSC.h"
#include "DecPlsSC.h"

#ifndef DECODE_ONLY
#include "EncSC.h"
#endif

#include "Coder.h"
#include "CodeStr.h"
#include "PackSC.h"
#include "vCodecID.h"
#include "FadeSC.h"
#include "rate.h"
#include "paramSC.h"
#include "vciSC3.h"
#include "DecPriSC.h"

#include "xvocver.h"

/** ************************************************************************ **/
/**                     Voxware Core Interface PreDefines                    **/
/** ************************************************************************ **/

#define MAX_STACK_WORD_SC3_ENC        2000
#define MAX_STACK_WORD_SC3_DEC        900

#define VEM_COMPATIBILITY_VFONT       20
#define VEM_COMPATIBILITY_VAD_AGC     20
#define VEM_COMPATIBILITY_PLUS        20
#define VEM_COMPATIBILITY_DEFAULT     0

#define SC_CODEC_VERSION              "SC.01.08.00"


/** ************************************************************************ **/
/**                            Private Prototypes                            **/
/** ************************************************************************ **/

static unsigned  short SC3CompatibilityFunc(unsigned short VemPlugin, 
                                            unsigned long *pVemCodID);

static unsigned short initMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct);
static unsigned short freeMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct);

                                     
/******************************************************************************
*
* Function:  vciGetInfoSC3()        
*
* Action:    Fill up information block for SC3
*
* Input:    pvciCodecInfoBlk -- pointer to the info. block
*
* Output:   pvciCodecInfoBlk -- filled up info. block
*
* Globals:   none
*
* Return:    VCI error code
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

VCI_RETCODE vciGetInfoSC3(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk)
{

   pvciCodecInfoBlk->wVCIVersion               = (unsigned short) VCI_VERSION;
   
   /* Variables for codec identification.     */
   strcpy(pvciCodecInfoBlk->sCodecVersion,       SC_CODEC_VERSION);
   pvciCodecInfoBlk->dwCodecClassID            = (unsigned long)VOXWARE_CODEC_SC_8K_3;      
   pvciCodecInfoBlk->dwCodecID                 = (unsigned long)VOXWARE_CODEC_ID_SC_8K_3_GENERIC;
   
   /* PCM Data formats.                       */
   pvciCodecInfoBlk->wSamplingRate             = (unsigned short)RATE8K;   
   pvciCodecInfoBlk->wNumBitsPerSample         = 16; 
   pvciCodecInfoBlk->wNumSamplesPerFrame       = (unsigned short)FRAME_LENGTH_SC;

   /* Bitstream information.                  */
   pvciCodecInfoBlk->dwBitRate                 = (unsigned long)(TOTAL_BITS_SC3*
                                                    (float)RATE8K/(float)FRAME_LENGTH_SC);
   pvciCodecInfoBlk->dwMinPacketBits           = (unsigned short) TOTAL_BITS_SC3;
   pvciCodecInfoBlk->dwAvgPacketBits           = (unsigned short) TOTAL_BITS_SC3;
   pvciCodecInfoBlk->dwMaxPacketBits           = (unsigned short) TOTAL_BITS_SC3;

   /* Prime Flush Information */
   /* encoder delay + decoder delay == ENC_DELAY_SIZE (145 == 80 + 65) */
   pvciCodecInfoBlk->wNumPrimeFlushFrames      = (unsigned short)ceil((float)ENC_DELAY_SIZE
                                                 /(float)FRAME_LENGTH_SC);
   pvciCodecInfoBlk->wNumPrimeFlushSamples     = 
              (unsigned short)(pvciCodecInfoBlk->wNumPrimeFlushFrames*FRAME_LENGTH_SC
                               -ENC_DELAY_SIZE);
   
   /* Multi-channel support.                  */
   pvciCodecInfoBlk->wNumChannels              = 1;
   pvciCodecInfoBlk->wInterleaveBlockSize      = 1;
   
   return(VCI_NO_ERROR);
}
/*===========================================================================*/


#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  vciEncodeSC3()     
*
* Action:    Encodes one frame of speech using SC3.
*
* Input:    pEncodeMemBlk -- encoder structure
*           pvciCodecIOBlk -- codec I/O structure
*
* Output:   pEncodeMemBlk -- updated strcture
*           pvciCodecIOBlk -- updated strcture
*
* Globals:   none
*
* Return:    VCI error code
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

VCI_RETCODE vciEncodeSC3(void *pEncodeMemBlk, 
                         VCI_CODEC_IO_BLOCK *pvciCodecIOBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   PARAM_SC    ParamMblk;  /* scratch memory */
   STACK_DECLARE

   /* check the validity of the VCI IO blk */
   VCI_IO_BLK_PARAM_CHECK(pvciCodecIOBlk);

   if(!VCI_BLOCK_CHECK(pEncodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_ENCODE(pEncodeMemBlk))
      return VCI_NOT_ENC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)pEncodeMemBlk;
   STACK_ASSIGN(vciCodecMblk->pScratchBlk)
   vciCodecMblk->pVciCodecIOBlock=pvciCodecIOBlk;

   /* check to make sure that codec was initialized at 3 or higher */
   if(!(voxGetEncodeInitRate(vciCodecMblk->hCodecMblk)&SC_RATE_3K))
      return VCI_CODEC_NOT_SUPPORTED; 

   VoxEncodeSC(STACK_S pvciCodecIOBlk->pPCMBuffer, vciCodecMblk->hCodecMblk,
               (void *)&ParamMblk);

   /* combine quantization and pack together to save memory */
   {
     int QuanIndex[TOTAL_SC3_QUAN_INDICES];
 
     VoxQuantSC3(STACK_S QuanIndex, vciCodecMblk->hCodecMblk,
                 (void *)&ParamMblk);

     VoxPackSC3(QuanIndex, pvciCodecIOBlk->pVoxBuffer, 
                &(pvciCodecIOBlk->wVoxBufferWriteByteOffset),
                &(pvciCodecIOBlk->chVoxBufferWriteBitOffset), 
                pvciCodecIOBlk->wVoxBufferSize,
                (unsigned short) END_OF_FRAME);
   }

   return(VCI_NO_ERROR);
}
/*===========================================================================*/
#endif

/******************************************************************************
*
* Function:  vciDecodeSC3()
*
* Action:    Decodes one frame of speech using SC3.
*
* Input:    pDecodeMemBlk -- decoder structure
*           pvciCodecIOBlk -- codec I/O structure
*
* Output:   pDecodeMemBlk -- updated strcture
*           pvciCodecIOBlk -- updated strcture
*
* Globals:   none
*
* Return:    VCI error code
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

VCI_RETCODE vciDecodeSC3(void *pDecodeMemBlk, 
                         VCI_CODEC_IO_BLOCK *pvciCodecIOBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   DEC_MEM_BLK     *pDecMemBlk;
   PARAM_SC        ParamMblk; 
   unsigned short  sLongerFrameFlag;
   short wSeek;
   STACK_DECLARE
   VCI_RETCODE VoxErrRet = VCI_NO_ERROR;
   unsigned short sByteOffset;
   unsigned char cBitOffset;
   unsigned char cDecReqDataFlagSC;

   /*----------------------------------------------------
     sanity check and decode check
   ----------------------------------------------------*/
   if(!VCI_BLOCK_CHECK(pDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(pDecodeMemBlk))     
      return VCI_NOT_DEC_HANDLE;
   
   /*----------------------------------------------------
     set some pointers and assign the VoxStack
   ----------------------------------------------------*/
   vciCodecMblk = (VCI_CODEC_BLOCK *)pDecodeMemBlk;

   pDecMemBlk = (DEC_MEM_BLK *)(vciCodecMblk->hCodecMblk);

   STACK_ASSIGN(vciCodecMblk->pScratchBlk);
   vciCodecMblk->pVciCodecIOBlock=pvciCodecIOBlk;

   /*----------------------------------------------------
     check the validity of the VCI IO Block.
   ----------------------------------------------------*/
   
   if (SILENCE != GetCurrentFrameStateSC(pDecMemBlk->pFadeIOMemBlk))
   {
	   VCI_IO_BLK_PARAM_CHECK(pvciCodecIOBlk);

	  /*----------------------------------------------------
		 If the decoder needs a new frame to decode then
		   get the new frame from pvciCodecIOBlk->pVoxBuffer
		   and put it in vciCodecMblk->pbSaveVoxBuffer.
	   ----------------------------------------------------*/
	   if (voxGetDecReqDataFlagSC((void *)pDecMemBlk))
		{

		/*------------------------------------------------------------
		  Check whether there are sufficient Vox data.
		  ------------------------------------------------------------*/
		  VoxErrRet = vciSeekForwardSC(pDecodeMemBlk, pvciCodecIOBlk,
									   1, &sByteOffset, &cBitOffset);

		  if (VoxErrRet != VCI_NO_ERROR)
			 return VoxErrRet;

		/*------------------------------------------------------------
		  No further test is needed at this point because the error
			condition has already been tested and there is a frame
			of data available in the circular buffer.
		  ------------------------------------------------------------*/
		  copyCBuf2LBuf(pvciCodecIOBlk->pVoxBuffer, 
						&(pvciCodecIOBlk->wVoxBufferReadByteOffset),
						&(pvciCodecIOBlk->chVoxBufferReadBitOffset), 
						pvciCodecIOBlk->wVoxBufferSize, 
						TOTAL_BITS_SC3, pDecMemBlk->pbSaveVoxBuffer);
		}

	   /*---------------------------------------------------------
		 Unpack and unquantize the frame in 
		   vciCodecMblk->pbSaveVoxBuffer.
	   ---------------------------------------------------------*/
	   {
		 int QuanIndex[TOTAL_SC3_QUAN_INDICES];
		 sByteOffset = 0;
		 cBitOffset = 0;
	   /*---------------------------------------------------------
		 This unpacking location is a momentary hack, must be changed 
		 to support time scaling ####
		 ---------------------------------------------------------*/
		 VoxUnPackSC3(QuanIndex,
					  pDecMemBlk->pbSaveVoxBuffer,
					  &sByteOffset,
					  &cBitOffset,
					  TOTAL_BYTES_SC3,
					  &sLongerFrameFlag);

		 VoxUnQuantSC3(STACK_S QuanIndex, (void *)pDecMemBlk, &ParamMblk);
	   }

	   /*---------------------------------------------------------
		 if sLongerFrameFlag==LONGER_FRAME then this is a 
		 SC_RATE_6K bitstream and we need to increment the byte
		 pointer in the circular buffer by ADDITIONAL_BYTES_SC6
		 ---------------------------------------------------------*/
	   if (voxGetDecReqDataFlagSC((void *)pDecMemBlk) 
		   && (sLongerFrameFlag==LONGER_FRAME))
		 {
		   wSeek=seekForward(pvciCodecIOBlk->pVoxBuffer,
							 pvciCodecIOBlk->wVoxBufferReadByteOffset,
							 pvciCodecIOBlk->wVoxBufferWriteByteOffset,
							 pvciCodecIOBlk->wVoxBufferSize,
							 1, NULL);

		   if(wSeek==-2) 
			  return VCI_BITSTREAM_CORRUPTED;

		   pvciCodecIOBlk->wVoxBufferReadByteOffset=(unsigned short)wSeek;
		 }
   }
   else
   {
	   VCI_SIL_IO_BLK_PARAM_CHECK_DEC(pvciCodecIOBlk);
   }

   /*----------------------------------------------------
     Set the decoder rate, it is always 3
   ----------------------------------------------------*/
   if(voxSetDecodeSC((void *)pDecMemBlk, SC_RATE_3K))
     return VCI_CODEC_NOT_SUPPORTED;
   
   /*---------------------------------------------------------
     Call the fade routine to modify parameters if there
       has benn a fade.
   ---------------------------------------------------------*/
   VoxFadeSC(pDecMemBlk->pFadeIOMemBlk, &ParamMblk);

   /*---------------------------------------------------------
     Decode the parameters.
   ---------------------------------------------------------*/
   VoxDecodeSC(STACK_S (void *)pDecMemBlk, &ParamMblk, pvciCodecIOBlk->pPCMBuffer, 
               &(pvciCodecIOBlk->wSamplesInPCMBuffer) );
 
   /*---------------------------------------------------------
     Update the fade block for the case when the decoder
       is not yet finished with the current frame.
   ---------------------------------------------------------*/
   cDecReqDataFlagSC = voxGetDecReqDataFlagSC((void *)pDecMemBlk);
   UpdateFadeStateSC( pDecMemBlk->pFadeIOMemBlk, cDecReqDataFlagSC );

   return(VCI_NO_ERROR);
}
/*===========================================================================*/


#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  vciInitEncodeSC3()
*
* Action:    To allocate memory for a Codec Memory Block and to initialize
*            it as an encoder.  
*
* Input:    hEncodeMemBlk -- the pointer to the encoder structure
*
* Output:   hEncodeMemBlk -- memory allocated and initialized encoder structure
*
* Globals:   none
*
* Return:    VCI error code
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

VCI_RETCODE vciInitEncodeSC3(void **hEncodeMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk=NULL;
   STACK_DECLARE

   if(!hEncodeMemBlk)
      return VCI_NULL_ARG;

   if(VOX_MEM_INIT(vciCodecMblk=*hEncodeMemBlk,1,sizeof(VCI_CODEC_BLOCK))) 
     return VCI_FAILED_MEM_ALLOC;

   vciCodecMblk->chEncOrDec = ENCODE_ID;  
   vciCodecMblk->dwVciChunkID = SANITY_CHECK;
   vciCodecMblk->dwCodecClassID = VOXWARE_CODEC_SC_8K_3;
   vciCodecMblk->dwCodecID = VOXWARE_CODEC_ID_SC_8K_3_GENERIC;

   if(STACK_SET(vciCodecMblk->pScratchBlk,(MAX_STACK_WORD_SC3_ENC*sizeof(int)))) {
      vciFreeEncodeSC3(hEncodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }

   if(!(vciCodecMblk->pvemMemBlk=vemInit())) {
#if VEM_DEFINED == 1
      vciFreeEncodeSC3(hEncodeMemBlk);
      return VCI_FAILED_MEM_ALLOC; 
#endif
   } else {
      vemRegisterConstShort(GET_VEM_MEMORY_HANDLE(vciCodecMblk), 
                            (short)FRAME_LENGTH_SC, "frame length");
      vemRegisterConstShort(GET_VEM_MEMORY_HANDLE(vciCodecMblk), 
                            (short)RATE8K, "rate");
   }

   if(VoxInitEncodeSC3(&vciCodecMblk->hCodecMblk, vciCodecMblk->pvemMemBlk)) {
      vciFreeEncodeSC3(hEncodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }

   if(initMethodStruct(&vciCodecMblk->pVciMethodStruct))
      return VCI_FAILED_MEM_ALLOC;

   return VCI_NO_ERROR;
}
/*===========================================================================*/
#endif


/******************************************************************************
*
* Function:  vciInitDecodeSC3()
*
* Action:    To allocate memory for a Codec Memory Block and to initialize
*            it as a decoder.
*
* Input:    hDecodeMemBlk -- the pointer to the decoder structure
*
* Output:   hDecodeMemBlk -- allocated  and initilized structure
*
* Globals:   none
*
* Return:    VCI error code
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

VCI_RETCODE vciInitDecodeSC3(void **hDecodeMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk=NULL;
   DEC_MEM_BLK *pDecMemBlk;

   STACK_DECLARE

   if(!hDecodeMemBlk)
      return VCI_NULL_ARG;

   if(VOX_MEM_INIT(vciCodecMblk=*hDecodeMemBlk,1,sizeof(VCI_CODEC_BLOCK))) 
      return VCI_FAILED_MEM_ALLOC;

   vciCodecMblk->chEncOrDec = DECODE_ID;
   vciCodecMblk->dwVciChunkID = SANITY_CHECK;

   if(STACK_SET(vciCodecMblk->pScratchBlk,MAX_STACK_WORD_SC3_DEC*sizeof(int))) {
      vciFreeDecodeSC3(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   } 

   if(!(vciCodecMblk->pvemMemBlk=vemInit())) {
#if VEM_DEFINED == 1
      vciFreeDecodeSC3(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
#endif
   }

   if(VoxInitDecodeSC3(&vciCodecMblk->hCodecMblk)) {
      vciFreeDecodeSC3(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   } 

   pDecMemBlk = (DEC_MEM_BLK *)(vciCodecMblk->hCodecMblk);

   if(VoxinitFadeSC(&pDecMemBlk->pFadeIOMemBlk)) {
      vciFreeDecodeSC3(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }

   if (VOX_MEM_INIT(pDecMemBlk->pbSaveVoxBuffer, 
                    TOTAL_BYTES_SC3,
                    sizeof(unsigned char *)))
     return VCI_FAILED_MEM_ALLOC;

   if(initMethodStruct(&vciCodecMblk->pVciMethodStruct))
      return VCI_FAILED_MEM_ALLOC;

   return VCI_NO_ERROR;
}
/*===========================================================================*/


#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  vciFreeEncodeSC3()
*
* Action:    Frees memory assigned to the encoder.
*
* Input:    hEncodeMemBlk -- the pointer to the encoder structure
*
* Output:   hEncodeMemBlk -- freed memory 
*
* Globals:   none
*
* Return:    VCI error code
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

VCI_RETCODE vciFreeEncodeSC3(void **hEncodeMemBlk)
{  
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   /* First make sure that the handle passed is the correct one. */
   if(!hEncodeMemBlk)
      return VCI_NULL_ARG;

   if(!VCI_BLOCK_CHECK(*hEncodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_ENCODE(*hEncodeMemBlk))
      return VCI_NOT_DEC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)*hEncodeMemBlk;
   
   vciCodecMblk->dwVciChunkID=0;

   VoxFreeEncodeSC3(&vciCodecMblk->hCodecMblk);

   vemFree(vciCodecMblk->pvemMemBlk);

   freeMethodStruct(&vciCodecMblk->pVciMethodStruct);

   STACK_UNSET(vciCodecMblk->pScratchBlk)

   VOX_MEM_FREE(vciCodecMblk);

   return VCI_NO_ERROR;
}
/*===========================================================================*/
#endif

/******************************************************************************
*
* Function:  vciFreeDecodeSC3()
*
* Action:    Frees memory assigned to the decoder.
*
* Input:    hDecodeMemBlk -- the pointer to the decoder structure
*
* Output:   hDecodeMemBlk -- freed memory 
*
* Globals:   none
*
* Return:    VCI error code
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

VCI_RETCODE vciFreeDecodeSC3(void **hDecodeMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   DEC_MEM_BLK *pDecMemBlk;

   /* First make sure that the handle passed is the correct one. */
   if(!hDecodeMemBlk)
      return VCI_NULL_ARG;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(*hDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(*hDecodeMemBlk))
      return VCI_NOT_DEC_HANDLE;
   
   vciCodecMblk = (VCI_CODEC_BLOCK *)*hDecodeMemBlk;
   pDecMemBlk = (DEC_MEM_BLK *)(vciCodecMblk->hCodecMblk);

   vciCodecMblk->dwVciChunkID=0;

   VoxfreeFadeSC(&pDecMemBlk->pFadeIOMemBlk);

   VOX_MEM_FREE(pDecMemBlk->pbSaveVoxBuffer);

   VoxFreeDecodeSC3(&vciCodecMblk->hCodecMblk);

   vemFree(vciCodecMblk->pvemMemBlk);

   freeMethodStruct(&vciCodecMblk->pVciMethodStruct);

   STACK_UNSET(vciCodecMblk->pScratchBlk)

   VOX_MEM_FREE(vciCodecMblk);

  return VCI_NO_ERROR;
}

/*===========================================================================*/


/******************************************************************************
*
* Function:  vciGetExtCompatibilitySC3()  
*
* Action:    Returns compatibility function 
*
* Input:    pCompatFunc -- the function pointer to the compatibility function
*
* Output:   pCompatFunc -- the compatibility function
*
* Globals:   none
*
* Return:    VCI return error
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

VCI_RETCODE vciGetExtCompatibilitySC3(void **pCompatFunc)
{
   *((COMPATIBILITY_FUNCTION **)pCompatFunc)=&SC3CompatibilityFunc;

   return VCI_NO_ERROR;
}


/******************************************************************************
*
* Function:  SC3CompatibilityFunc()
*
* Action:    Get the compatiliby ID
*
* Input:    VemPlugin -- the VEM-plug-in-ID
*           pVemCodID -- the pointer to the current codec ID
*
* Output:   pVemCodID -- the current codec ID
*
* Globals:   none
*
* Return:    vem-compatibility-ID 
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

static unsigned  short SC3CompatibilityFunc(unsigned short VemPlugin, 
                                            unsigned long *pVemCodID) 
{
   unsigned short vem_compatibility_ID;

   *pVemCodID=(unsigned long)VOXWARE_CODEC_SC_8K_3;
   
   switch(VemPlugin) {
      case COMPATIBILITY_VFONT:
        vem_compatibility_ID = (unsigned short)VEM_COMPATIBILITY_VFONT;
        break;

      case COMPATIBILITY_VAD_AGC:
        vem_compatibility_ID = (unsigned short)VEM_COMPATIBILITY_VAD_AGC;
        break;

      case COMPATIBILITY_PLUS: 
        vem_compatibility_ID = (unsigned short)VEM_COMPATIBILITY_PLUS;
        break;

      default:
        vem_compatibility_ID = (unsigned short)VEM_COMPATIBILITY_DEFAULT;
   }

   return vem_compatibility_ID;
}



/******************************************************************************
*
* Function:  initMethodStruct()
*
* Action:    initialization of the VCI method structure
*
* Input:    hVciMethodStruct -- the pointer to the VCI method structure
*
* Output:   hVciMethodStruct -- the memory allocated and initialized 
*                               VCI method structure
*
* Globals:   none
*
* Return:    0: no error , 1: memory allocation error
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

static unsigned short initMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct)
{
   if(VOX_MEM_INIT(*hVciMethodStruct,1,sizeof(VCI_METHOD_STRUCT)))
      return 1;

   (*hVciMethodStruct)->pfVciGetInfo=vciGetInfoSC3;

#ifndef DECODE_ONLY
   (*hVciMethodStruct)->pfVciEncode=vciEncodeSC3;
   (*hVciMethodStruct)->pfVciInitEncode=vciInitEncodeSC3;
   (*hVciMethodStruct)->pfVciFreeEncode=vciFreeEncodeSC3;
#endif

   (*hVciMethodStruct)->pfVciDecode=vciDecodeSC3;
   (*hVciMethodStruct)->pfVciInitDecode=vciInitDecodeSC3; 
   (*hVciMethodStruct)->pfVciFreeDecode=vciFreeDecodeSC3;

   return 0;
}

/******************************************************************************
*
* Function:  freeMethodStruct()
*
* Action:    free the memory for the VCI method structure
*
* Input:    hVciMethodStruct -- the pointer to the VCI method structure
*
* Output:   hVciMethodStruct -- memory freed structure
*
* Globals:   none
*
* Return:    0: no error
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

static unsigned short freeMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct)
{
   if(hVciMethodStruct)
      VOX_MEM_FREE(*hVciMethodStruct);

   return 0;
}
