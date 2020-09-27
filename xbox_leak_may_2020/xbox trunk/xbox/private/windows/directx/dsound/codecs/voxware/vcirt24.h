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
 * FILENAME:  vciRT24.h                                                       *
 * PURPOSE:   Defines the interface functions and defines for vciRT24.c.      *
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

#ifndef VCIRT24_H
#define VCIRT24_H
 
#ifdef __cpluplus
extern "C"
{
#endif
                                                         
VCI_RETCODE vciGetInfoRT24(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk);

VCI_RETCODE vciDecodeRT24(void *hCodecMemBlk, 
                          VCI_CODEC_IO_BLOCK *hvciDecodeIOBlk);
VCI_RETCODE vciEncodeRT24(void *hCodecMemBlk, 
                          VCI_CODEC_IO_BLOCK *hvciEncodeIOBlk);

VCI_RETCODE vciInitEncodeRT24(void **hCodecMemBlk);
VCI_RETCODE vciInitDecodeRT24(void **hCodecMemBlk); 
                                                      
VCI_RETCODE vciFreeEncodeRT24(void **hCodecMemBlk);                                                       
VCI_RETCODE vciFreeDecodeRT24(void **hCodecMemBlk);  

VCI_RETCODE vciGetExtCompatibilityRT24(void **ppvExtCompat);


#define MAX_FRAME_LENGTH_24    180
#define PACKET_BITS_RT24    54  /* The number of bits in a bit-stream packet  */
#define PACKET_BYTE_RT24     7  /* The minimum bytes required to store one    */
                                /* bit-stream packet.                         */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VCIRT24_H */





