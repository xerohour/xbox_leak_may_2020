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
 * FILENAME:  vciRT29.h                                                       *
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


#ifndef VCIRT28_H
#define VCIRT28_H
 
#ifdef __cplusplus
extern "C"
{
#endif
                                                         
VCI_RETCODE vciGetInfoRT28(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk);

VCI_RETCODE vciDecodeRT28(void *hCodecMemBlk, 
                          VCI_CODEC_IO_BLOCK *hvciDecodeIOBlk);
VCI_RETCODE vciEncodeRT28(void *hCodecMemBlk, 
                          VCI_CODEC_IO_BLOCK *hvciEncodeIOBlk);

VCI_RETCODE vciInitEncodeRT28(void **hCodecMemBlk);
VCI_RETCODE vciInitDecodeRT28(void **hCodecMemBlk); 
                                                      
VCI_RETCODE vciFreeEncodeRT28(void **hCodecMemBlk);                                                       
VCI_RETCODE vciFreeDecodeRT28(void **hCodecMemBlk);  

VCI_RETCODE vciGetExtCompatibilityRT28(void **ppvExtCompat);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VCIRT29_H */





