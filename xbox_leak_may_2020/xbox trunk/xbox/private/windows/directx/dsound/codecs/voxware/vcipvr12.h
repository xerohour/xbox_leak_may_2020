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
 * FILENAME:  vciPVR12.h                                                      *
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

#ifndef VCIPVR12_H
#define VCIPVR12_H

#ifdef __cplusplus
extern "C"
{
#endif

VCI_RETCODE vciDecodeNeedsInputVR12(void *hCodecMemBlk, 
                                    unsigned char *pUnPack);
VCI_RETCODE vciSetWarpFactorVR12(void *hCodecMemBlk, float wWarpFactor);
VCI_RETCODE vciGetWarpFactorVR12(void *hCodecMemBlk, float *pwWarpFactor);

VCI_RETCODE vciSetInjectSilenceFlagVR12(void *hCodecMemBlk);

VCI_RETCODE vciGetEnergyVR12(void *hCodecMemBlk, short *pwEnergyLevel);   
  
VCI_RETCODE vciNumFramesInBufferVR12(const void *hCodecMemBlk, 
                                 const VCI_CODEC_IO_BLOCK *hvciCodecIOBlk,
                                 unsigned short *pwNumFrames,
                                 unsigned short *pwByteOffset,
                                 unsigned char  *pchBitOffset);
VCI_RETCODE vciSeekForwardVR12(const void *hCodecMemBlk, 
                           const VCI_CODEC_IO_BLOCK *hvciCodecIOBlk,
                           unsigned short      wNumFrames,
                           unsigned short *pwByteOffset,
                           unsigned char  *pchBitOffset);

VCI_RETCODE vciGetPlusInfoVR12(VCI_PLUS_INFO_BLOCK *pVciPlusInfoBlk);

/* needed for all extensions that call out to vci(plus) calls */
VCI_RETCODE vciPlusInitVR12(const void *pCodecMemBlk);
VCI_RETCODE vciPlusFreeVR12(const void *pCodecMemBlk);

#ifdef __cplusplus
}
#endif /* __cplusplus */
                     
#endif /* VCIPVR15_H */

