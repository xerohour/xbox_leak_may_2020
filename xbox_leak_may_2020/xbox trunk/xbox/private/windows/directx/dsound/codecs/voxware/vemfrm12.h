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
 * FILENAME:  vemFram12.h                                                      *
 * PURPOSE:   Defines the interface functions for vmFrames.c                  *
 * AURTHOR:   Epiphany Vera                                                   *
 *                                                                            *
 *****************************************************************************/

/******************************************************************************
 *                                                                            *
 *                          DETAILED REVISION HISTORY                         *
 *                                                                            *
 * (March 24, '97)                                                            *
 *    Created                                                                 *
 *                                                                            *
 *****************************************************************************/

void framesInBufferVR12(const VCI_CODEC_IO_BLOCK  *hvciCodecIOBlk,
                         unsigned short      *pwNumFrames);
void seekForwardVR12(unsigned short ReadByteOffset,
                      unsigned char  ReadBitOffset,
                      unsigned short BufferSize,
                      unsigned char  *pBuffer,
                      unsigned short wNumFrames,
                      unsigned short *pwByteOffset,
                      unsigned char  *pchBitOffset,
                      unsigned short *BitsInFrames);



