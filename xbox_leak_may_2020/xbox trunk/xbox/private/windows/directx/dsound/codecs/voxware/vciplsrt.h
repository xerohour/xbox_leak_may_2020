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
 * FILENAME:  vciPlsRT.h                                                      *
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

#ifndef VCIPLSRT_H
#define VCIPLSRT_H

#ifdef __cplusplus
extern "C"
{
#endif

VCI_RETCODE vciDecodeNeedsInputRT(void *hCodecMemBlk, unsigned char *pUnPack);
VCI_RETCODE vciSetWarpFactorRT(void *hCodecMemBlk, float wWarpFactor);
VCI_RETCODE vciGetWarpFactorRT(void *hCodecMemBlk, float *pwWarpFactor);

VCI_RETCODE vciSetInjectSilenceFlagRT(void *hCodecMemBlk);

VCI_RETCODE vciGetEnergyRT(void *hCodecMemBlk, short *pwEnergyLevel);   
  
VCI_RETCODE vciNumFramesInBufferRT(const void *hCodecMemBlk, 
                                 const VCI_CODEC_IO_BLOCK *hvciCodecIOBlk,
                                 unsigned short *pwNumFrames,
                                 unsigned short *pwByteOffset,
                                 unsigned char  *pchBitOffset);
VCI_RETCODE vciSeekForwardRT(const void *hCodecMemBlk, 
                           const VCI_CODEC_IO_BLOCK *hvciCodecIOBlk,
                           unsigned short      wNumFrames,
                           unsigned short *pwByteOffset,
                           unsigned char  *pchBitOffset);

VCI_RETCODE vciGetPlusInfoRT(VCI_PLUS_INFO_BLOCK *pVciPlusInfoBlk);

/* needed for all extensions that call out to vci(plus) calls */
VCI_RETCODE vciPlusInit(const void *pCodecMemBlk);
VCI_RETCODE vciPlusFree(const void *pCodecMemBlk);

#ifdef __cplusplus
}
#endif /* __cplusplus */
                     
#endif /* VCIPLSRT_H */

