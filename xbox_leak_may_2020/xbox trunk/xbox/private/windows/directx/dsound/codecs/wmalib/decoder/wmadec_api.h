//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       wmadec_api.h
//
//--------------------------------------------------------------------------


#ifndef _WMADEC_API_H_
#define _WMADEC_API_H_


typedef enum {
    WMADECERR_NOERROR = 0,     // no error, success

    WMADECERR_FAIL,            // (un-reasoned, generic) failure
    WMADECERR_OUTOFMEMORY,     // out of memory
    WMADECERR_INVALIDHANDLE,   // invalid handle
    WMADECERR_INVALIDPOINTER,  // invalid memory pointer

    WMADECERR_EOF,             // end of file
    WMADECERR_FILEREAD,        // failed in a file read operation

    WMADECERR_BUFTOOSMALL,     // buffer is too small for the operation
    WMADECERR_BUFTOOLARGE,     // buffer is too large for the operation

    WMADECERR_MAX              // guarding max barrier (not a valid error code)
} WMADECERROR;

// handle to be used for moving an instance
typedef void * WMADECHANDLE;

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


/***************   Begin: NORMAL, NON-STREAMING MODE  *******************************/
// ===========================================================================
// WMADECCreate()
//
// Creates a Microsoft Audio decoder object and returns it.
//
// Input: nVersion         : version of the WMA
//        nSamplesPerSec   : sampling rate in Hz
//        nChannels        : number of channels (1 for mono and 2 for stereo)
//        nBytesPerSec     : bits per second times 8
//        nSamplesPerBlock : decoded samples per block (powers of 2)
//        nFrameLength     : encoded frame length in bytes
//        nEncodeOpt       : encoding mode option (0 or 1)
//        nPlayerOpt       : decoding mode, see defines below.
//
// Returns NULL if not successful.

WMADECHANDLE
WMADECCreate (long nVersion,
           long nSamplesPerSec,
           long nChannels,
           long nBytesPerSec,
           long nSamplesPerBlock,
           long nFrameLength,
           long nEncodeOpt,
           long nPlayerOpt);

// ===========================================================================
// Important notes on Player Options 
// These options change the sampling rate of the PCM data relative to the input bit stream sampling rate.
// 1. Downsample 32kHz to 22kHz only works if the bit stream sampling rate is 32kHz.
//    The PCM data returned will be at 22kHz.
// 2. Half Transform cuts the sampling rate in half so the PCM data will be at half the bit stream rate.
//    Normally use this to lower CPU requirements when playing in the background.
// 3. Upsample doubles the sampling rate and is used in conjunction with Downsample 32kHz to 22kHz
//    so DOWNSAMPLE32_22 | UPSAMPLE will result in the PCM data being at 44kHz.
//    Remember that your PCM buffer should be longer with this option.
// 4. Pad2x Transform doubles the sampling rate.  So a 22kHz bit stream will return PCM data at 44kHz.
//    Remember that your PCM buffer should be twice as long with this option.

#define PLAYOPT_DOWNSAMPLE32TO22 0x0001
#define PLAYOPT_HALFTRANSFORM    0x0002
#define PLAYOPT_UPSAMPLE         0x0004
#define PLAYOPT_PAD2XTRANSFORM   0x0008


// ===========================================================================
// WMADECDestroy()
//
// Destroys the given Microsoft Audio decoder handle.

WMADECERROR
WMADECDestroy(WMADECHANDLE hDecoder);


// ===========================================================================
// WMADECDecode()
//
// Decodes the memory block with the previous created handle.
//
// Input: pSrc      : pointer to the source buffer
//        nSrcLen   : size of the source buffer in bytes
//        pnSrcUsed : pointer to receive the actual number of source bytes
//                    used
//        pDst      : pointer to the destination buffer
//        nDstLen   : size of the destination buffer in bytes
//        pnDstUsed : pointer to receive the actual number of destination
//                    bytes used
//        pnSamplesFromBefore: number of samples used before the current packet

WMADECERROR WMADECDecode (
    WMADECHANDLE hDecoder,
    unsigned char *pSrc,
    unsigned long nSrcLen,
    unsigned long *pnSrcUsed,
    unsigned char *pDst,
    unsigned long nDstLen,
    unsigned long *pnDstUsed,
    unsigned long *pnSamplesFromBefore
);


// ===========================================================================
// WMADECFlush()
//
// Flushes out the internally buffered data, if any.
//
// Input: pDst      : pointer to the destination buffer
//        nDstLen   : size of the destination buffer in bytes
//        pnDstUsed : pointer to receive the actual number of destination
//                    bytes used

WMADECERROR
WMADECFlush(WMADECHANDLE hDecoder,
          unsigned char *pDst,
          unsigned long nDstLen,
          unsigned long *pnDstUsed);


// ===========================================================================
// WMADECOutputBufferSize()
//
// Returns the minimum Output Buffer Size for WMADECDecode.
//
// Input: nVersion         : version of the WMA
//        nSamplesPerSec   : sampling rate in Hz
//        nChannels        : number of channels (1 for mono and 2 for stereo)
//        nBytesPerSec     : bits per second times 8
//        nEncodeOpt       : encoding mode option (0 or 1)
//                    bytes used

unsigned long WMADECOutputBufferSize(long nVersion, 
                                     long nSamplingRate, 
                                     long nChannels, 
                                     long nBytesPerSec, 
                                     long nEncodeOpt);


/***************   End: NORMAL, NON-STREAMING MODE  *******************************/

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // _MSAD_API_H_
