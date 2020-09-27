/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:    vciSC6.c
*
* Purpose:     Defines the interface functions for the Voxware SC Quality codec
*
* Functions:   vciGetInfoSC6, vciEncodeSC6, vciDecodeSC6, 
*              vciInitEncodeSC6, vciInitDecodeSC3, vciFreeEncodeSC6,
*              vciFreeDecodeSC6, vciGetExtCompatibilitySC6
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
* $Header:   P:/r_and_d/archives/scaleable/vci/vciSC6.c_v   1.31   12 Oct 1998 10:23:52   johnp  $
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
#include "VoxMem.h"
#include "vem2.h"
#include "vem2Prm.h"
#include "vciPlsSC.h"

/** *********************************************************************** **/
/**                              Codec Headers                              **/
/** *********************************************************************** **/
#include "vLibPack.h"
#include "codec.h"

#include "DecSC.h"
#include "DecPlsSC.h"

#ifndef DECODE_ONLY
#include "EncSC.h"
#endif

#include "Coder.h"
#include "CodeStr.h"
#include "PackSC.h"
#include "vCodecID.h"
#include "quansc36.h"
#include "FadeSC.h"
#include "rate.h"
#include "paramSC.h"
#include "vciSC3.h"
#include "vciSC6.h"
#include "DecPriSC.h"

#include "xvocver.h"

/** ************************************************************************ **/
/**                     Voxware Core Interface PreDefines                    **/
/** ************************************************************************ **/

#define MAX_STACK_WORD_SC6_ENC       2000
#define MAX_STACK_WORD_SC6_DEC        900

#define VEM_COMPATIBILITY_VFONT       20
#define VEM_COMPATIBILITY_VAD_AGC     20
#define VEM_COMPATIBILITY_PLUS        20
#define VEM_COMPATIBILITY_DEFAULT     0

#define SC_CODEC_VERSION              "SC.01.08.00"


/** ************************************************************************ **/
/**                            Private Prototypes                            **/
/** ************************************************************************ **/

static unsigned  short SC6CompatibilityFunc(unsigned short VemPlugin, 
                                            unsigned long *pVemCodID) ;
static unsigned short initMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct);
static unsigned short freeMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct);

/******************************************************************************
*
* Function:  vciGetInfoSC6() 
*
* Action:    Fill up information block for SC6
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

VCI_RETCODE vciGetInfoSC6(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk)
{

   pvciCodecInfoBlk->wVCIVersion               = (unsigned short) VCI_VERSION;
   
   /* Variables for codec identification.     */
   strcpy(pvciCodecInfoBlk->sCodecVersion,       SC_CODEC_VERSION);
   pvciCodecInfoBlk->dwCodecClassID            = (unsigned long)VOXWARE_CODEC_SC_8K_6;      
   /* generic codec is SC3 */
   pvciCodecInfoBlk->dwCodecID                 = (unsigned long)VOXWARE_CODEC_ID_SC_8K_3_GENERIC;  
   
   /* PCM Data formats.                       */
   pvciCodecInfoBlk->wSamplingRate             = (unsigned short)RATE8K;   
   pvciCodecInfoBlk->wNumBitsPerSample         = 16; 
   pvciCodecInfoBlk->wNumSamplesPerFrame       = (unsigned short)FRAME_LENGTH_SC;

   pvciCodecInfoBlk->dwBitRate                 = (unsigned long)((TOTAL_BITS_SC3+
                                                    ADDITIONAL_BITS_SC6)*
                                                    (float)RATE8K/(float)FRAME_LENGTH_SC);

   /* Prime Flush Information */
   /* encoder delay + decoder delay == ENC_DELAY_SIZE (145 == 80 + 65) */
   pvciCodecInfoBlk->wNumPrimeFlushFrames      = (unsigned short)ceil((float)ENC_DELAY_SIZE
                                                  /(float)FRAME_LENGTH_SC);
   pvciCodecInfoBlk->wNumPrimeFlushSamples     = 
              (unsigned short)(pvciCodecInfoBlk->wNumPrimeFlushFrames*FRAME_LENGTH_SC
                              -ENC_DELAY_SIZE);
   
   /* Bitstream information (for fix rate codec). */
   pvciCodecInfoBlk->dwMinPacketBits           = (unsigned short) (TOTAL_BITS_SC3+
                                                    ADDITIONAL_BITS_SC6);
   pvciCodecInfoBlk->dwAvgPacketBits           = pvciCodecInfoBlk->dwMinPacketBits;
   pvciCodecInfoBlk->dwMaxPacketBits           = pvciCodecInfoBlk->dwMinPacketBits;
   
   /* Multi-channel support.                  */
   pvciCodecInfoBlk->wNumChannels              = 1;
   pvciCodecInfoBlk->wInterleaveBlockSize      = 1;

   return(VCI_NO_ERROR);
}
/*===========================================================================*/


#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  vciEncodeSC6()     
*
* Action:    Encodes one frame of speech using SC6
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

VCI_RETCODE vciEncodeSC6( void               *pEncodeMemBlk, 
                          VCI_CODEC_IO_BLOCK *pvciCodecIOBlk
                        )
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   PARAM_SC ParamMblk; /* scratch memory */
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

   /* check to make sure that codec was initialized at 6 or higher */
   if(!(voxGetEncodeInitRate(vciCodecMblk->hCodecMblk)&SC_RATE_6K))
      return VCI_CODEC_NOT_SUPPORTED; 

   /*--------------------------------------------------------------------
     Encode speech at SC_RATE_6K
   --------------------------------------------------------------------*/
   VoxEncodeSC(STACK_S pvciCodecIOBlk->pPCMBuffer, vciCodecMblk->hCodecMblk,
               (void *)&ParamMblk);

   {
     int QuanSC3Index[TOTAL_SC3_QUAN_INDICES];
     int QuanSC6Index[EXTRA_SC6_QUAN_INDICES];

     /*--------------------------------------------------------------------
      Quantize 3k parameters and then quantize 6k parameters
     --------------------------------------------------------------------*/
     VoxQuantSC3(STACK_S QuanSC3Index, vciCodecMblk->hCodecMblk,
                 (void *)&ParamMblk);

     if(voxGetEncodeRate(vciCodecMblk->hCodecMblk)!=SC_RATE_3K)
        VoxQuantSC6(QuanSC6Index, (void *)&ParamMblk);

     /*--------------------------------------------------------------------
       Pack 3k parameters and then pack 6k parameters
     --------------------------------------------------------------------*/
     if(voxGetEncodeRate(vciCodecMblk->hCodecMblk)==SC_RATE_3K) {
        VoxPackSC3(QuanSC3Index,
                   pvciCodecIOBlk->pVoxBuffer,
                   &(pvciCodecIOBlk->wVoxBufferWriteByteOffset),
                   &(pvciCodecIOBlk->chVoxBufferWriteBitOffset),
                   pvciCodecIOBlk->wVoxBufferSize,
                   (unsigned short) END_OF_FRAME);

     } else {
        VoxPackSC3(QuanSC3Index,
                   pvciCodecIOBlk->pVoxBuffer,
                   &(pvciCodecIOBlk->wVoxBufferWriteByteOffset),
                   &(pvciCodecIOBlk->chVoxBufferWriteBitOffset),
                   pvciCodecIOBlk->wVoxBufferSize,
                   (unsigned short) LONGER_FRAME);

        VoxPackSC6(QuanSC6Index,
                   pvciCodecIOBlk->pVoxBuffer,
                   &(pvciCodecIOBlk->wVoxBufferWriteByteOffset),
                   &(pvciCodecIOBlk->chVoxBufferWriteBitOffset),
                   pvciCodecIOBlk->wVoxBufferSize,
                   (unsigned short) END_OF_FRAME);
     }
   }

   return(VCI_NO_ERROR);
}
/*===========================================================================*/
#endif


/******************************************************************************
*
* Function:  vciDecodeSC6()
*
* Action:    Decodes one frame of speech using SC6
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

VCI_RETCODE vciDecodeSC6( void               *pDecodeMemBlk, 
                          VCI_CODEC_IO_BLOCK *pvciCodecIOBlk
                        )
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   DEC_MEM_BLK     *pDecMemBlk;
   PARAM_SC        ParamMblk;
   STACK_DECLARE
   unsigned short  sLongerFrameFlag;
   VCI_RETCODE VoxErrRet = VCI_NO_ERROR;
   unsigned short sByteOffset=0;
   unsigned char  cBitOffset=0;
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
   STACK_ASSIGN(vciCodecMblk->pScratchBlk);

   pDecMemBlk= (DEC_MEM_BLK *)vciCodecMblk->hCodecMblk;

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
			  Check whether there are sufficient Vox data. If there is not,
				return an error.  In the code below we will assume there
				is enough data.
			  ------------------------------------------------------------*/
			  VoxErrRet = vciSeekForwardSC(pDecodeMemBlk, pvciCodecIOBlk,
										   1, &sByteOffset, &cBitOffset);

			  if (VoxErrRet != VCI_NO_ERROR)
				 return VoxErrRet;
		   }

		   /*------------------------------------------------------------
			 Unpack and unquantize the frame.  QuanMemBlk is declared
			   only in this local region to save memory.
		   ------------------------------------------------------------*/
		   {
			 int QuanSC3Index[TOTAL_SC3_QUAN_INDICES];
			 int QuanSC6Index[EXTRA_SC6_QUAN_INDICES];
			 sByteOffset = 0;
			 cBitOffset = 0;

			 /*---------------------------------------------------------
			   Copy the SC3 bits from the circular buffer to the 
				 linear buffer.
			 ---------------------------------------------------------*/
			 if (voxGetDecReqDataFlagSC((void *)pDecMemBlk))
			   copyCBuf2LBuf(pvciCodecIOBlk->pVoxBuffer, 
							 &(pvciCodecIOBlk->wVoxBufferReadByteOffset),
							 &(pvciCodecIOBlk->chVoxBufferReadBitOffset), 
							 pvciCodecIOBlk->wVoxBufferSize, 
							 TOTAL_BITS_SC3,
							 pDecMemBlk->pbSaveVoxBuffer);

			 /*---------------------------------------------------------
			   Unpack and unquantize SC3 data.
			 ---------------------------------------------------------*/
			 VoxUnPackSC3(QuanSC3Index,
						  pDecMemBlk->pbSaveVoxBuffer,
						  &sByteOffset,
						  &cBitOffset,
						  (TOTAL_BYTES_SC3+ADDITIONAL_BYTES_SC6),
						  &sLongerFrameFlag);

			 VoxUnQuantSC3(STACK_S QuanSC3Index, (void *)pDecMemBlk, &ParamMblk);

			 /*---------------------------------------------------------
			   if sLongerFrameFlag==END_OF_FRAME then decode using
				 SC_RATE_3K, otherwise continue unpacking.
			 ---------------------------------------------------------*/
			 if (sLongerFrameFlag==END_OF_FRAME)
			 {
				if(voxSetDecodeSC((void *)pDecMemBlk, SC_RATE_3K)) /* set decoder rate to SC3 */
				   return VCI_CODEC_NOT_SUPPORTED;
			 } else {
				if(voxSetDecodeSC((void *)pDecMemBlk, SC_RATE_6K)) /* set decoder rate to SC6 */
				   return VCI_CODEC_NOT_SUPPORTED;

				/*---------------------------------------------------------
				  Copy the additional SC6 bits from the circular buffer 
					to the linear buffer.  

				  NOTE: We had to unpack the SC3 bits before we could find 
					out if there are additional SC6 bits.
				---------------------------------------------------------*/
				if (voxGetDecReqDataFlagSC((void *)pDecMemBlk))
				   copyCBuf2LBuf(pvciCodecIOBlk->pVoxBuffer, 
								 &(pvciCodecIOBlk->wVoxBufferReadByteOffset),
								 &(pvciCodecIOBlk->chVoxBufferReadBitOffset), 
								 pvciCodecIOBlk->wVoxBufferSize, ADDITIONAL_BITS_SC6,
								 &(pDecMemBlk->pbSaveVoxBuffer[(TOTAL_BITS_SC3)>>3]));

				/*---------------------------------------------------------
				  Unpack and unquantize SC6 data.
				---------------------------------------------------------*/
				VoxUnPackSC6(QuanSC6Index,
							 pDecMemBlk->pbSaveVoxBuffer,
							 &sByteOffset,
							 &cBitOffset,
							 (TOTAL_BYTES_SC3+ADDITIONAL_BYTES_SC6),
							 &sLongerFrameFlag);

			   VoxUnQuantSC6(QuanSC6Index, &ParamMblk);
			 }
		   }
   }
   else
   {
	   VCI_SIL_IO_BLK_PARAM_CHECK_DEC(pvciCodecIOBlk);
   }

   
   /*---------------------------------------------------------
     Call the fade routine to modify parameters if there
       has benn a fade.
   ---------------------------------------------------------*/
   VoxFadeSC(pDecMemBlk->pFadeIOMemBlk, &ParamMblk);

   /*---------------------------------------------------------
     If the Fade state is SILENCE then set the decoder to
       SC3 because the fade routine does not generate
       measured phase parameters.
   ---------------------------------------------------------*/
   if (GetCurrentFrameStateSC(pDecMemBlk->pFadeIOMemBlk) == SILENCE)
     {
       if(voxSetDecodeSC((void *)pDecMemBlk, SC_RATE_3K))
          return VCI_CODEC_NOT_SUPPORTED;
     }

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
* Function:  vciInitEncodeSC6()
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

VCI_RETCODE vciInitEncodeSC6(void **hEncodeMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk=NULL;
   STACK_DECLARE

   if(!hEncodeMemBlk)
      return VCI_NULL_ARG;

   if(VOX_MEM_INIT(vciCodecMblk=*hEncodeMemBlk,1,sizeof(VCI_CODEC_BLOCK))) 
     return VCI_FAILED_MEM_ALLOC;

   vciCodecMblk->chEncOrDec = ENCODE_ID;
   vciCodecMblk->dwVciChunkID = SANITY_CHECK;

   if(STACK_SET(vciCodecMblk->pScratchBlk,20480)) {
      vciFreeEncodeSC6(hEncodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }

   if(!(vciCodecMblk->pvemMemBlk=vemInit())) {
#if VEM_DEFINED == 1
      vciFreeEncodeSC6(hEncodeMemBlk);
      return VCI_FAILED_MEM_ALLOC; 
#endif
   } else {
      vemRegisterConstShort(GET_VEM_MEMORY_HANDLE(vciCodecMblk), (short)FRAME_LENGTH_SC, "frame length");
      vemRegisterConstShort(GET_VEM_MEMORY_HANDLE(vciCodecMblk), (short)RATE8K, "rate");
   }

   if(VoxInitEncodeSC3(&vciCodecMblk->hCodecMblk,vciCodecMblk->pvemMemBlk)) {
      vciFreeEncodeSC6(hEncodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }
 
   if(VoxInitEncodeSC6(&vciCodecMblk->hCodecMblk)) {
      vciFreeEncodeSC6(hEncodeMemBlk);
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
* Function:  vciInitDecodeSC6()
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

VCI_RETCODE vciInitDecodeSC6(void **hDecodeMemBlk)
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

   if(STACK_SET(vciCodecMblk->pScratchBlk,20240)) {
      vciFreeDecodeSC6(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   } 

   if(!(vciCodecMblk->pvemMemBlk=vemInit())) {
#if VEM_DEFINED == 1
      vciFreeDecodeSC6(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
#endif
   }

   if(VoxInitDecodeSC3(&vciCodecMblk->hCodecMblk)) {
      vciFreeDecodeSC6(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   } 
 
   if(VoxInitDecodeSC6(&vciCodecMblk->hCodecMblk)) {
      vciFreeDecodeSC6(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   } 
   
   pDecMemBlk = (DEC_MEM_BLK *)(vciCodecMblk->hCodecMblk);

   if(VoxinitFadeSC(&pDecMemBlk->pFadeIOMemBlk)) {
      vciFreeDecodeSC6(hDecodeMemBlk);
      return VCI_FAILED_MEM_ALLOC;
   }
   
   if (VOX_MEM_INIT(pDecMemBlk->pbSaveVoxBuffer, 
                    TOTAL_BYTES_SC3+ADDITIONAL_BYTES_SC6,
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
* Function:  vciFreeEncodeSC6()
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

VCI_RETCODE vciFreeEncodeSC6(void **hEncodeMemBlk)
{  
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;

   if(!hEncodeMemBlk)
      return VCI_NULL_ARG;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(*hEncodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_ENCODE(*hEncodeMemBlk))
      return VCI_NOT_ENC_HANDLE;
   
   vciCodecMblk = (VCI_CODEC_BLOCK *)*hEncodeMemBlk;

   vciCodecMblk->dwVciChunkID=0;

   /*--------------------------------------------
     Must free SC6 before the free for SC3.
       (The free for SC3 assumes that SC6 was already freed)
   --------------------------------------------*/
   VoxFreeEncodeSC6(&vciCodecMblk->hCodecMblk);
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
* Function:  vciFreeDecodeSC6()
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

VCI_RETCODE vciFreeDecodeSC6(void **hDecodeMemBlk)
{
   VCI_CODEC_BLOCK *vciCodecMblk = NULL;
   DEC_MEM_BLK *pDecMemBlk;

   if(!hDecodeMemBlk)
      return VCI_NULL_ARG;

   /* First make sure that the handle passed is the correct one. */
   if(!VCI_BLOCK_CHECK(*hDecodeMemBlk))
      return VCI_NOT_VCI_HANDLE;

   if(!VCI_IS_DECODE(*hDecodeMemBlk))
      return VCI_NOT_DEC_HANDLE;

   vciCodecMblk = (VCI_CODEC_BLOCK *)*hDecodeMemBlk;

   vciCodecMblk->dwVciChunkID=0;

   pDecMemBlk = (DEC_MEM_BLK *)(vciCodecMblk->hCodecMblk);

   VoxfreeFadeSC(&pDecMemBlk->pFadeIOMemBlk);

   VOX_MEM_FREE(pDecMemBlk->pbSaveVoxBuffer);

   /*--------------------------------------------
     Must free SC6 before the free for SC3.
       (The free for SC3 assumes that SC6 was already freed)
   --------------------------------------------*/
   VoxFreeDecodeSC6(&vciCodecMblk->hCodecMblk);
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
* Function:  vciGetExtCompatibilitySC6()  
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

VCI_RETCODE vciGetExtCompatibilitySC6(void **pCompatFunc)
{
   *((COMPATIBILITY_FUNCTION **)pCompatFunc)=&SC6CompatibilityFunc;

   return VCI_NO_ERROR;
}


/******************************************************************************
*
* Function:  SC6CompatibilityFunc()
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

unsigned  short SC6CompatibilityFunc(unsigned short VemPlugin, 
                                     unsigned long *pVemCodID) 
{
   unsigned short vem_compatibility_ID;
   *pVemCodID=(unsigned long)VOXWARE_CODEC_SC_8K_6;
   
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

   (*hVciMethodStruct)->pfVciGetInfo=vciGetInfoSC6;

#ifndef DECODE_ONLY
   (*hVciMethodStruct)->pfVciEncode=vciEncodeSC6;
   (*hVciMethodStruct)->pfVciInitEncode=vciInitEncodeSC6;
   (*hVciMethodStruct)->pfVciFreeEncode=vciFreeEncodeSC6;
#endif

   (*hVciMethodStruct)->pfVciDecode=vciDecodeSC6;
   (*hVciMethodStruct)->pfVciInitDecode=vciInitDecodeSC6; 
   (*hVciMethodStruct)->pfVciFreeDecode=vciFreeDecodeSC6;

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

