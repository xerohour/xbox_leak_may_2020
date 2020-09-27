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
 * FILENAME:  vmFrames.h                                                      *
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

void framesInBufferFixed(const VCI_CODEC_IO_BLOCK  *hvciCodecIOBlk,
                         short               PacketBits,
                         unsigned short      *pwNumFrames);
void seekForwardFixed(unsigned short ReadByteOffset,
                      unsigned char  ReadBitOffset,
                      unsigned short BufferSize,   unsigned short PacketBits,
                      unsigned short wNumFrames,   unsigned short *pwByteOffset,
                      unsigned char  *pchBitOffset);




