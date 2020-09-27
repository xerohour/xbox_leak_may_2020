/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:         vciPriv.h 
*
* Purpose:     Defines the interface data structures and constants that are
*              private to the Voxware Core Interface (VCI). 

* Author/Date: John Puterbaugh, Ilan Berci & Epiphany Vera  (Nov 25, '96)
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/common/vci/vciPriv.h_v   1.5   22 Jun 1998 14:29:28   weiwang  $
******************************************************************************/
#ifndef VCIPRIV_H
#define VCIPRIV_H

#ifndef _VCI_RETCODE_
#define _VCI_RETCODE_
typedef unsigned short VCI_RETCODE; /* Voxware Core Interface error type.    */
                                    /* This variable is also defined in      */
                                    /* vciError.h.                           */
#endif  /** #ifndef _VCI_RETCODE_ **/


/* ************************************************************************** */
/*                                                                            */
/* STRUCTURE: VCI_PLUS_METHOD_STRUCT                                          */
/* PURPOSE :  definies the basic functions for VCI                            */
/*                                                                            */
/* ************************************************************************** */
typedef struct tagVCI_METHOD_STRUCT {

   VCI_RETCODE (*pfVciGetInfo)(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk);

   VCI_RETCODE (*pfVciEncode)(void *hCodecMemBlk, VCI_CODEC_IO_BLOCK *hvciEncodeIOBlk);
   VCI_RETCODE (*pfVciDecode)(void *hCodecMemBlk, VCI_CODEC_IO_BLOCK *hvciDecodeIOBlk);

   VCI_RETCODE (*pfVciInitEncode)(void **hCodecMemBlk);
   VCI_RETCODE (*pfVciInitDecode)(void **hCodecMemBlk); 
                                                      
   VCI_RETCODE (*pfVciFreeEncode)(void **hCodecMemBlk);                                                       
   VCI_RETCODE (*pfVciFreeDecode)(void **hCodecMemBlk);  

} VCI_METHOD_STRUCT;


/* ************************************************************************** */
/*                                                                            */
/* STRUCTURE: VCI_PLUS_METHOD_STRUCT                                          */
/* PURPOSE :  definies the plus functionalities for VCI                       */
/*                                                                            */
/* ************************************************************************** */
typedef struct tagVCI_PLUS_METHOD_STRUCT {
   VCI_RETCODE (*pfVciDecodeNeedsInput)(void *hCodecMemBlk, unsigned char *pUnPack);
   VCI_RETCODE (*pfVciSetWarpFactor)(void *hCodecMemBlk, float wWarpFactor);
   VCI_RETCODE (*pfVciGetWarpFactor)(void *hCodecMemBlk, float *pwWarpFactor);

   VCI_RETCODE (*pfVciSetInjectSilenceFlag)(void *hCodecMemBlk);

   VCI_RETCODE (*pfVciGetEnergy)(void *hCodecMemBlk, short *pwEnergyLevel);   
  
   VCI_RETCODE (*pfVciNumFramesInBuffer)(const void     *hCodecMemBlk, 
                                    const VCI_CODEC_IO_BLOCK  *hvciCodecIOBlk,
                                    unsigned short      *pwNumFrames,
                                    unsigned short      *pwByteOffset,
                                    unsigned char       *pchBitOffset);
   VCI_RETCODE (*pfVciSeekForward)(const void     *hCodecMemBlk, 
                              const VCI_CODEC_IO_BLOCK  *hvciCodecIOBlk,
                              unsigned short      wNumFrames,
                              unsigned short      *pwByteOffset,
                              unsigned char       *pchBitOffset);

   VCI_RETCODE (*pfVciSetFramesSkipped)(void *pDecodeMemBlk, int iFramesSkipped);
   VCI_RETCODE (*pfVciSetRepeatFrameFlag)(void *pDecodeMemBlk);


   VCI_RETCODE (*pfVciExtractFromBitStream)(const void * pVciCodecBlk,
                                         const VCI_CODEC_IO_BLOCK  *pvciInputIOBlk,
                                         VCI_CODEC_IO_BLOCK  *pvciOutputIOBlk,
                                         unsigned long      ScCodec);

   VCI_RETCODE (*pfVciCombineBitStream)(const void * pVciCodecBlk,
                                     const VCI_CODEC_IO_BLOCK *pvciInputAIOBlk,
                                     const VCI_CODEC_IO_BLOCK *pvciInputBIOBlk,
                                     VCI_CODEC_IO_BLOCK *pvciOutputIOBlk,
                                     unsigned long  ScCodecA,
                                     unsigned long  ScCodecB);

   VCI_RETCODE (*pfVciGetRatesInFrame)(const void *pVciCodecBlk,
                                     const VCI_CODEC_IO_BLOCK  *pvciCodecIOBlk,
                                     unsigned long   *ScCodec);

   VCI_RETCODE (*pfVciSetEncoderRate)(const void *pEncodeMemBlk, unsigned long wBitField);
   VCI_RETCODE (*pfVciGetEncoderRate)(const void *pEncodeMemBlk, unsigned long *wBitField); 

   VCI_RETCODE (*pfVciGetValidRates)(const void *pCodecMemBlk, unsigned long *wBitField);

   VCI_RETCODE (*pfVciCalculateBitRate)(const void *pCodecMemBlk, unsigned long wBitField, 
                                        unsigned short *wBitRate);
   VCI_RETCODE (*pfVciCalculateRateSC)(const void *pCodecMemBlk, unsigned short wBitRate, 
                                        unsigned long *wBitField);

   unsigned short *wStreamTable;

} VCI_PLUS_METHOD_STRUCT;



/* ************************************************************************** */
/*                                                                            */
/* STRUCTURE: VCI_CODEC_BLOCK                                                 */
/* PURPOSE :  This a wrapper put around the codec's encode or decode by VCI   */
/*            It is used by VCI to identify the memory block.                 */
/*                                                                            */
/* ************************************************************************** */
typedef struct tagVciCodecBlock
{
/* Warning Warning Warning Warning Warning Warning Warning Warning Warning */
/* DO NOT MOVE THE FIRST ELEMENT OF THIS SRUCT!!!!!!!!                     */
/* Warning Warning Warning Warning Warning Warning Warning Warning Warning */
   unsigned long  dwVciChunkID;  /* Number used to identify a VCI memory   */
                                    /* block.                                 */
   unsigned long  dwCodecClassID;   /* Codec class ID                         */   
   unsigned long  dwCodecID;        /* Specific ID                            */

   unsigned char  chEncOrDec;       /* Identifies wether the memory block po- */
                                    /* inted to by hCodecMblk is an encode or */
                                    /* decode memory block.                   */
   void           *hCodecMblk;      /* Points to an encode or decode memory   */
                                    /* block.                                 */
   void           *pQuanMemBlk;     /* Points to a Quantize memory block      */
   void           *pvemMemBlk;      /* Points to the extension manager memory */

   void           *pVciCodecIOBlock;/* Attached VCI Codec IO Block            */
   void           *pScratchBlk;     /* Points to Scratch Memory block         */

   VCI_METHOD_STRUCT *pVciMethodStruct; /* needed for extensions that call VCI*/
   VCI_PLUS_METHOD_STRUCT *pVciPlusMethodStruct; /* as bove for vci plus funcs*/

} VCI_CODEC_BLOCK;

/* ************************************************************************** */
/*                              hCodecMblk TYPES                              */
/* ************************************************************************** */
/* Defines the types of codec memory blocks. These are the values that are    */
/* assigned to chEncOrDec.                                                    */
#define ENCODE_ID   0xA
#define DECODE_ID   0xB

#ifndef SANITY_CHECK
#define SANITY_CHECK 0xFACE
#endif

#define VCI_BLOCK_CHECK(ptr)    (ptr&&(((VCI_CODEC_BLOCK*)ptr)->dwVciChunkID==SANITY_CHECK))
#define VCI_GET_VEM_HANDLE(ptr) (((VCI_CODEC_BLOCK*)ptr)->pvemMemBlk)

#define VCI_IS_ENCODE(ptr)      (((VCI_CODEC_BLOCK*)ptr)->chEncOrDec==ENCODE_ID)
#define VCI_IS_DECODE(ptr)      (((VCI_CODEC_BLOCK*)ptr)->chEncOrDec==DECODE_ID)

#define VCI_GET_CLASS_ID(ptr)   (((VCI_CODEC_BLOCK*)ptr)->dwCodecClassID)
#define VCI_GET_CODEC_ID(ptr)   (((VCI_CODEC_BLOCK*)ptr)->dwCodecID)

/* functions for initializing the method struct */
unsigned short vciInitMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct);
unsigned short vciFreeMethodStruct(VCI_METHOD_STRUCT **hVciMethodStruct);

#endif /* VCIPRIV_H */




