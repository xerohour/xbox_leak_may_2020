/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vciPlsSC.h
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/vci/vciPlsSC.h_v   1.2   16 Mar 1998 17:13:06   weiwang  $
******************************************************************************/
#ifndef _VCIPLSSC
#define _VCIPLSSC

#ifdef __cplusplus
extern "C"
{
#endif

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetFramesSkippedSC()                                         */
/* PURPOSE : Tell the decoder that frames have been skipped/lost             */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSetFramesSkippedSC(void *pDecodeMemBlk, int iFramesSkipped);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetRepeatFrameFlagSC()                                       */
/* PURPOSE : Tell the decoder that the frame is a repeat.                    */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSetRepeatFrameFlagSC(void *pDecodeMemBlk);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetWarpFactorSC()                                            */
/* PURPOSE : Sets the warp factor for the decoder.                           */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSetWarpFactorSC(void *pDecodeMemBlk, float wWarpFactor);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetWarpFactor()                                              */
/* PURPOSE : Retrieves the warp factor setting in the  decoder.              */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetWarpFactorSC(void *pDecodeMemBlk, float *pwWarpFactor);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciDecodeNeedsInput()                                           */
/* PURPOSE : Checks whether the decoder needs input or not.                  */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciDecodeNeedsInputSC(void *pDecodeMemBlk, unsigned char *pUnPack);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetEnergyLevelSC()                                           */
/* PURPOSE : Gets the energy level.                                          */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetEnergyLevelSC(void *pDecodeMemBlk, short *pwEnergyLevel);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetInjectSilenceFlagSC()                                     */
/* PURPOSE : Perform fade-in/fade-out and background noise injection during  */
/*           packet loss.                                                    */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciSetInjectSilenceFlagSC(void *pDecodeMemBlk);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciNumFramesInBuffer()                                          */
/* PURPOSE : Counts the number of frames in the Vox buffer of the given      */
/*           codec I/O block.                                                */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciNumFramesInBufferSC(const void *pVciCodecMemBlk,
                                   const VCI_CODEC_IO_BLOCK *pvciCodecIOBlk,
                                   unsigned short *pwNumFrames,
                                   unsigned short *pwByteOffset,
                                   unsigned char  *pchBitOffset);

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
                             unsigned char  *pchBitOffset);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciExtractFromBitStreamSC()                                     */
/* PURPOSE : Extracts the specified embedded streams from the Vox Buffer of  */
/*           the codec I/O block                                             */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciExtractFromBitStreamSC(const void *pVciCodecMemBlk,
                                      const VCI_CODEC_IO_BLOCK *pvciInputIOBlk,
                                      VCI_CODEC_IO_BLOCK  *pvciOutputIOBlk,
                                      unsigned long dwBitField);

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
                                  unsigned long  ScCodecB);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetRatesInFrameSC()                                          */
/* PURPOSE : Returns the highest rate codec that is present in the           */
/*           bit-stream for the frame ponted to by the read offset pointer.  */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetRatesInFrameSC(const void *pVciCodecMemBlk,
                                 const VCI_CODEC_IO_BLOCK *pvciCodecIOBlk,
                                 unsigned long *ScCodec);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciSetEncoderRateSC()                                           */
/* PURPOSE : Switches the encoder rate on the fly (depends upon              */
/*           initialization)                                                 */
/*                                                                           */
/* NOTE    : looking for a rate or a valid stream!                           */
/* ************************************************************************* */
VCI_RETCODE vciSetEncoderRateSC(const void *pEncodeMemBlk, unsigned long wRate);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetEncoderRateSC()                                           */
/* PURPOSE : Returns the current encoder rate being used                     */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetEncoderRateSC(const void *pEncodeMemBlk, unsigned long *wRate); 

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciGetValideRateSC()                                            */
/* PURPOSE : Returns the current either encoder rate or decoder rate         */
/*           being used                                                      */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciGetValidRatesSC(const void *pCodecMemBlk, unsigned long *wRate);


/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciCalculateBitRate()                                           */
/* PURPOSE : Returns the bitrate calculated from the rate stream passed in   */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciCalculateRateSC(const void *pCodecMemBlk, unsigned short wBitRate, 
                                   unsigned long *wRate);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciCalculateBitField()                                          */
/* PURPOSE : Returns the bitrate calculated from the rate stream passed in   */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciCalculateBitRateSC(const void *pCodecMemBlk, unsigned long wRate, 
                                  unsigned short *pwBitRate);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciPlusInitSC()                                                 */
/* PURPOSE : Initialize pVciPlusMethodStruct with all the plus functions     */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciPlusInitSC(const void *pCodecMemBlk);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: vciPlusFreeSC()                                                 */
/* PURPOSE : Free pVciPlusMethodStruct                                       */
/*                                                                           */
/* ************************************************************************* */
VCI_RETCODE vciPlusFreeSC(const void *pCodecMemBlk);

/* ************************************************************************** */
/*                                                                            */
/* FUNCTION: vciGetPlusInfoSC()                                               */
/* PURPOSE : Fills in the VCI plus info blk                                   */
/*                                                                            */
/* ************************************************************************** */
VCI_RETCODE vciGetPlusInfoSC(VCI_PLUS_INFO_BLOCK *pVciPlusInfoBlk);

#ifdef __cplusplus
}
#endif

#endif /* _VCIPLSSC */


