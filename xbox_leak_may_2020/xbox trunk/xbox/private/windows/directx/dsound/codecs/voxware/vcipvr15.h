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
 * FILENAME:  vciPVR15.h                                                      *
 * PURPOSE:   Defines the interface functions for vciPlsRT.c.                 *
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

#ifndef VCIPVR15_H
#define VCIPVR15_H

#ifdef __cplusplus
extern "C"
{
#endif

VCI_RETCODE vciDecodeNeedsInputVR15(void *hCodecMemBlk, 
                                    unsigned char *pUnPack);
VCI_RETCODE vciSetWarpFactorVR15(void *hCodecMemBlk, float wWarpFactor);
VCI_RETCODE vciGetWarpFactorVR15(void *hCodecMemBlk, float *pwWarpFactor);

VCI_RETCODE vciSetInjectSilenceFlagVR15(void *hCodecMemBlk);

VCI_RETCODE vciGetEnergyVR15(void *hCodecMemBlk, short *pwEnergyLevel);   
  
VCI_RETCODE vciNumFramesInBufferVR15(const void *hCodecMemBlk, 
                                 const VCI_CODEC_IO_BLOCK *hvciCodecIOBlk,
                                 unsigned short *pwNumFrames,
                                 unsigned short *pwByteOffset,
                                 unsigned char  *pchBitOffset);
VCI_RETCODE vciSeekForwardVR15(const void       *hCodecMemBlk, 
                           const VCI_CODEC_IO_BLOCK *hvciCodecIOBlk,
                           unsigned short      wNumFrames,
                           unsigned short *pwByteOffset,
                           unsigned char  *pchBitOffset);

VCI_RETCODE vciGetPlusInfoVR15(VCI_PLUS_INFO_BLOCK *pVciPlusInfoBlk);

/* needed for all extensions that call out to vci(plus) calls */
VCI_RETCODE vciPlusInitVR15(const void *pCodecMemBlk);
VCI_RETCODE vciPlusFreeVR15(const void *pCodecMemBlk);

#ifdef __cplusplus
}
#endif /* __cplusplus */
                     
#endif /* VCIPVR15_H */

