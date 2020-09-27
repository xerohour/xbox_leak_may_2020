//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       WMADEC_api.cpp
//
//--------------------------------------------------------------------------

// WMADEC_api.cpp

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#include <stdlib.h>  
//#ifdef HITACHI
//typedef long I32;
//typedef unsigned long I32;
//#endif
#include "..\decoder\msaudiodec.h"
#include "..\decoder\wmadec_api.h"
#include "..\..\..\dsound\dsndver.h"


// ===========================================================================
// struct for the handle

#define WMADECHANDLE_MAGIC 0xdcfb8a8e

typedef struct _tCWMADECHANDLE
{
    // magic is just to check for a validity
    unsigned long magic;

    // needed later (acquired at init time)
    long nFrameLength;

    // the actual object
    CAudioObjectDecoder *pDecoder;

} CWMADECHANDLE;

long
WMADEChIsValid(CWMADECHANDLE *pHandle)
{
    return pHandle != NULL &&
           pHandle->magic == WMADECHANDLE_MAGIC &&
           pHandle->pDecoder != NULL;
}

CWMADECHANDLE *
WMADEChNew()
{
    CWMADECHANDLE *pHandle = (CWMADECHANDLE *)malloc(sizeof(CWMADECHANDLE));
    if(pHandle == NULL)
        return NULL;

    pHandle->magic        = WMADECHANDLE_MAGIC;
    pHandle->nFrameLength = 0;
    pHandle->pDecoder     = NULL;

    return pHandle;
}

void
WMADEChDelete(CWMADECHANDLE *pHandle)
{
    if(pHandle)
    {
        if(pHandle->pDecoder)
        {
            audecDelete(pHandle->pDecoder);
            pHandle->pDecoder = NULL;
        }
        free(pHandle);
    }
}


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
//        nPlayerOpt       : decoding mode, see defines in the .h file.
//
// Returns NULL if not successful.

WMADECHANDLE
WMADECCreate(long nVersion,
           long nSamplesPerSec,
           long nChannels,
           long nBytesPerSec,
           long nSamplesPerBlock,
           long nFrameLength,
           long nEncodeOpt,
           long nPlayerOpt)
{
    WMARESULT hr;
    CWMADECHANDLE *pHandle = NULL;

    // create the handle
    pHandle = WMADEChNew();
    if(pHandle == NULL)
        goto lerror;

    // create the object
    pHandle->pDecoder = audecNew(NULL, 0);
    if(pHandle->pDecoder == NULL)
        goto lerror;

    // store this for later
    pHandle->nFrameLength = nFrameLength;

    // initialize the object
    hr = audecInit(pHandle->pDecoder,
                   nVersion,
                   nSamplesPerBlock,
                   nSamplesPerSec,
                   (U16)nChannels,
                   nBytesPerSec,
                   nFrameLength,
                   (U16)nEncodeOpt, 
                   (U16)nPlayerOpt,
                   NULL, (U32)0, NULL);
    if(hr != WMADECERR_NOERROR)
        goto lerror;


    return (WMADECHANDLE)pHandle;

lerror:
    if(pHandle)
        WMADEChDelete(pHandle);
    return NULL;
}


// ===========================================================================
// WMADECDestroy()
//
// Destroys the given Microsoft Audio decoder handle.

WMADECERROR
WMADECDestroy(WMADECHANDLE hDecoder)
{
    CWMADECHANDLE *pHandle = (CWMADECHANDLE *)hDecoder;

    // check for a valid handle
    if(!WMADEChIsValid(pHandle))
        return WMADECERR_INVALIDHANDLE;

    // destroy the object and the handle
    audecDelete(pHandle->pDecoder);
    pHandle->pDecoder = NULL;
    WMADEChDelete(pHandle);

    return WMADECERR_NOERROR;
}


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

WMADECERROR
WMADECDecode(WMADECHANDLE hDecoder,
           unsigned char *pSrc,
           unsigned long nSrcLen,
           unsigned long *pnSrcUsed,
           unsigned char *pDst,
           unsigned long nDstLen,
           unsigned long *pnDstUsed,
           unsigned long *pnSamplesFromBefore
)
{
    WMARESULT hr;
    //NQF+
    //long nSrcUsed, nDstUsed;
    Int nSrcUsed, nDstUsed;    
    //NQF-
    CWMADECHANDLE *pHandle = (CWMADECHANDLE *)hDecoder;

    // check for a valid handle
    if(!WMADEChIsValid(pHandle))
        return WMADECERR_INVALIDHANDLE;

    // check for the memory pointers
    if(pSrc == NULL || pDst == NULL)
        return WMADECERR_INVALIDPOINTER;

    // decode
    hr = audecDecode(pHandle->pDecoder,
                     pSrc, nSrcLen, &nSrcUsed,
                     pDst, nDstLen, &nDstUsed,
                     NULL, 0, NULL, (long *)pnSamplesFromBefore);
	if (hr == 4)
		hr = WMADECERR_NOERROR; 
	if(hr != WMADECERR_NOERROR)
        return WMADECERR_FAIL;

    // update the out arguments
    if(pnSrcUsed)
        *pnSrcUsed = nSrcUsed;
    if(pnDstUsed)
        *pnDstUsed = nDstUsed;

    return WMADECERR_NOERROR;
}


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
          unsigned long *pnDstUsed)
{
    WMARESULT hr;
    //NQF+
    //long nDstUsed;
    Int nDstUsed;
    //NQF-
    
    CWMADECHANDLE *pHandle = (CWMADECHANDLE *)hDecoder;

    // check for a valid handle
    if(!WMADEChIsValid(pHandle))
        return WMADECERR_INVALIDHANDLE;

    // check for the memory pointers
    if(pDst == NULL)
        return WMADECERR_INVALIDPOINTER;
    
    // flush
    hr = audecFlush(pHandle->pDecoder,
                    pDst, nDstLen, &nDstUsed,
                    NULL, 0, NULL, NULL);
    if(hr != WMADECERR_NOERROR)
        return WMADECERR_FAIL;

    // update the out argument
    if(pnDstUsed)
        *pnDstUsed = nDstUsed;

    return WMADECERR_NOERROR;
}


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

unsigned long WMADECOutputBufferSize(long nVersion, 
                                     long nSamplingRate, 
                                     long nChannels, 
                                     long nBytesPerSec, 
                                     long nEncodeOpt)
{
    return (unsigned long)audecOutputBufferSize( (Int)nVersion, (Int)nSamplingRate, (Int)nChannels, 
                                                 (Int)nBytesPerSec, (U16)nEncodeOpt);
}

