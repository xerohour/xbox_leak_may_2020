/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vci.h
*
* Purpose:   Defines the interface data structures needed for the Voxware
*            Core Interface (VCI).
*
* Structure: VCI_CODEC_IO_BLOCK, VCI_CODEC_INFO_BLOCK
*
* Author/Date:     Ilan Berci, Epiphany Vera
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/vci/vci.h_v   1.1   13 Feb 1998 10:42:36   weiwang  $
******************************************************************************/

#ifndef _VCI_H_
#define _VCI_H_

/* Donavan's VCI switch */
#define VCI_SEGMENTED_MODEL_SWITCH 0

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _VCI_RETCODE_
#define _VCI_RETCODE_
typedef unsigned short VCI_RETCODE; /* Voxware Core Interface error type.    */
                                    /* This variable is also defined in      */
                                    /* vciError.h.                           */
#endif  /** #ifndef _VCI_RETCODE_ **/


/* ************************************************************************* */
/*                                 VCI Defines                               */
/* ************************************************************************* */
#define VCI_VERSION 4

/* ************************************************************************* */
/*                                                                           */
/* STRUCTURE: VCI_CODEC_CONTROL_BLOCK                                        */
/* PURPOSE :  Used for I/O between the codec and the calling program.        */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
typedef struct 
{
  /* PCM Data buffer variables                                               */
                                  /* Buffer of signed 16 bit PCM samples.    */
  signed short   *pPCMBuffer;               
                                  /* Number of samples in PCMBuffer          */
  unsigned short wSamplesInPCMBuffer;

  /* Compressed bit-stream buffer variables                                  */
                                  /* Buffer of compressed bit-stream data    */
  unsigned char  *pVoxBuffer;     
                                  /* Size of VoxBuffer in bytes              */
  unsigned short wVoxBufferSize;
                                  /* Diagram of the VoxBuffer. The "D" repr- */
                                  /* esents byte with at least 1 bit of data */
                                  /* _______________________________________
                                    |                  DDDDDDDDDDDDDD       |
                                    |__________________^____________^_______|
                                    ^                  ^            ^       ^
                                    |<-ReadByteOffset->|            |       |
                                    |                               |       |
                                    |<------WriteByteOffset-------->|       |
                                    |                               |       |
                                    |<-------------BufferSize-------------->|
                                    |
                                    |<---Begining of buffer (i.e. pVoxBuffer)
                                                                             */
                                  /* The offset in bytes, from the beginning */
                                  /* of the buffer, of the first byte to     */
                                  /* start reading from.                     */
  unsigned short wVoxBufferReadByteOffset;
                                  /* The offset in bits, from the most sign- */
                                  /* ificant bit, to start reading from.     */
  unsigned char  chVoxBufferReadBitOffset;
                                  /* The offset in bytes, from the beginning */
                                  /* of the buffer, of the first byte to     */
                                  /* start writing to.                       */
  unsigned short wVoxBufferWriteByteOffset;
                                  /* The offset in bits, from the most sign- */
                                  /* ificant bit, to start writing to.       */
  unsigned char  chVoxBufferWriteBitOffset;
   
} VCI_CODEC_IO_BLOCK;



/* ************************************************************************* */
/*                                                                           */
/* STRUCTURE: VCI_CODEC_INFO_BLOCK                                           */
/* PURPOSE :  Used for getting information about the codec.                  */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
 typedef struct
 {
   /* VCI Codec API information.              */
   unsigned short wVCIVersion;
   
   /* Variables for codec identification.     */
   char           sCodecVersion[32];  
   unsigned long  dwCodecClassID;      
   unsigned long  dwCodecID;
   
   /* PCM Data formats.                       */
   unsigned short wSamplingRate;   
   unsigned short wNumBitsPerSample;
   unsigned short wNumSamplesPerFrame;
   
   /* Multi-channel support.                  */
   unsigned short wNumChannels;
   unsigned short wInterleaveBlockSize;
   
   /* Bitstream information.                  */
   unsigned long dwBitRate;
   unsigned long dwMinPacketBits;
   unsigned long dwAvgPacketBits;
   unsigned long dwMaxPacketBits;
      
   /* Prime Flush Information   */
   unsigned short wNumPrimeFlushFrames;
   unsigned short wNumPrimeFlushSamples;
 } VCI_CODEC_INFO_BLOCK;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /** #ifndef _VCI_H_ **/


