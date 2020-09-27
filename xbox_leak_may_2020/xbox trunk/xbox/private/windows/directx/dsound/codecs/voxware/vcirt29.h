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


#ifndef VCIRT29_H
#define VCIRT29_H
 
#ifdef __cplusplus
extern "C"
{
#endif
                                                         
VCI_RETCODE vciGetInfoRT29(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk);

VCI_RETCODE vciDecodeRT29(void*hCodecMemBlk, 
                          VCI_CODEC_IO_BLOCK *hvciDecodeIOBlk);
VCI_RETCODE vciEncodeRT29(void *hCodecMemBlk, 
                          VCI_CODEC_IO_BLOCK *hvciEncodeIOBlk);

VCI_RETCODE vciInitEncodeRT29(void **hCodecMemBlk);
VCI_RETCODE vciInitDecodeRT29(void **hCodecMemBlk); 
                                                      
VCI_RETCODE vciFreeEncodeRT29(void **hCodecMemBlk);                                                       
VCI_RETCODE vciFreeDecodeRT29(void **hCodecMemBlk);  

VCI_RETCODE vciGetExtCompatibilityRT29(void **ppvExtCompat);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VCIRT29_H */





