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
 * FILENAME:  vciVR15.h                                                       *
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


#ifndef VCIVR15_H
#define VCIVR15_H
 
#ifdef __cplusplus
extern "C"
{
#endif
                                                         
VCI_RETCODE vciGetInfoVR15(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk);

VCI_RETCODE vciDecodeVR15(void *hCodecMemBlk, VCI_CODEC_IO_BLOCK *hvciDecodeIOBlk);
VCI_RETCODE vciEncodeVR15(void *hCodecMemBlk, VCI_CODEC_IO_BLOCK *hvciEncodeIOBlk);

VCI_RETCODE vciInitEncodeVR15(void **hCodecMemBlk);
VCI_RETCODE vciInitDecodeVR15(void **hCodecMemBlk); 
                                                      
VCI_RETCODE vciFreeEncodeVR15(void **hCodecMemBlk);                                                       
VCI_RETCODE vciFreeDecodeVR15(void **hCodecMemBlk);  

VCI_RETCODE vciGetExtCompatibilityVR15(void **ppvExtCompat);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VCIVR15_H */





