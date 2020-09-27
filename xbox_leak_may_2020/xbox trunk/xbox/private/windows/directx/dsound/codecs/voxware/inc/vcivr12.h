/******************************************************************************
 *                                                                            *
 *		                  Voxware Proprietary Material                        *
 *		                  Copyright 1996, Voxware, Inc.                       *
 *		                       All Rights Resrved                             *
 *                                                                            *
 *		                 DISTRIBUTION PROHIBITED without                      *
 *		               written authorization from Voxware                     *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
 *                                                                            *
 * FILENAME:  vciVR12.h                                                       *
 * PURPOSE:   Defines the interface functions and defines for vciRT29.c.      *
 *                                                                            *
 *****************************************************************************/

/******************************************************************************
 *                                                                            *
 *                          DETAILED REVISION HISTORY                         *
 *                                                                            *
 * (Sept 25, '96)                                                             *
 *    Created                                                                 *
 *                                                                            *
 *****************************************************************************/


#ifndef VCIVR12_H
#define VCIVR12_H
 
#ifdef __cplusplus
extern "C"
{
#endif
                                                         
VCI_RETCODE vciGetInfoVR12(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk);

VCI_RETCODE vciDecodeVR12(void *hCodecMemBlk, 
                          VCI_CODEC_IO_BLOCK *hvciDecodeIOBlk);
VCI_RETCODE vciEncodeVR12(void *hCodecMemBlk, 
                          VCI_CODEC_IO_BLOCK *hvciEncodeIOBlk);

VCI_RETCODE vciInitEncodeVR12(void **hCodecMemBlk);
VCI_RETCODE vciInitDecodeVR12(void **hCodecMemBlk); 
                                                      
VCI_RETCODE vciFreeEncodeVR12(void **hCodecMemBlk);                                                       
VCI_RETCODE vciFreeDecodeVR12(void **hCodecMemBlk);  

VCI_RETCODE vciGetExtCompatibilityVR12(void **ppvExtCompat);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VCIRT29_H */





