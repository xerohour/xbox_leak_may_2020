#ifndef _WMADECRAW_API_H_
#define _WMADECRAW_API_H_


#ifndef _WMARESULT_DEFINED
#define _WMARESULT_DEFINED

typedef int WMARESULT;

// SUCCESS codes
static const WMARESULT WMA_OK               = 0x00000000;
static const WMARESULT WMA_S_FALSE          = 0x00000001;
static const WMARESULT WMA_S_BUFUNDERFLOW   = 0x00000002;
static const WMARESULT WMA_S_NEWPACKET      = 0x00000003;
static const WMARESULT WMA_S_NO_MORE_FRAME  = 0x00000004;
static const WMARESULT WMA_S_NO_MORE_SRCDATA= 0x00000005;

// ERROR codes
static const WMARESULT WMA_E_FAIL           = 0x80004005;
static const WMARESULT WMA_E_OUTOFMEMORY    = 0x8007000E;
static const WMARESULT WMA_E_INVALIDARG     = 0x80070057;
static const WMARESULT WMA_E_NOTSUPPORTED   = 0x80040000;
static const WMARESULT WMA_E_LOSTPACKET     = 0x80040001;
static const WMARESULT WMA_E_BROKEN_FRAME   = 0x80040002;
static const WMARESULT WMA_E_BUFFEROVERFLOW = 0x80040003;
static const WMARESULT WMA_E_ONHOLD         = 0x80040004; 
static const WMARESULT WMA_E_NOMOREINPUT    = 0x8004000A; // Streaming mode: absolutely no more data (returned via GetMoreData callback)

#define WMARAW_SUCCEEDED(Status) ((WMARESULT)(Status) >= 0)
#define WMARAW_FAILED(Status) ((WMARESULT)(Status)<0)

#endif // _WMARESULT_DEFINED


// define basic types
typedef unsigned long U32_WMARawDec;
typedef long I32_WMARawDec;
typedef unsigned short U16_WMARawDec;
typedef short I16_WMARawDec;
typedef unsigned char U8_WMARawDec;


typedef void* WMARawDecHandle;

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


WMARESULT WMARawDecInit (
    WMARawDecHandle* phWMARawDec,
    U32_WMARawDec dwUserData,
    U16_WMARawDec iVersionNumber,
    U16_WMARawDec cSamplesPerBlock, 
    U16_WMARawDec cSamplesPerSecond, 
    U16_WMARawDec cChannel, 
    U16_WMARawDec cBytePerSec, 
    U16_WMARawDec cPacketLength,
    U16_WMARawDec cEncodeOpt,
    U16_WMARawDec cPlayerOpt
);

// **************************************************************************
// Player Options
// **************************************************************************
#define PLAYOPT_DOWNSAMPLE32TO22 0x0001
#define PLAYOPT_HALFTRANSFORM    0x0002
#define PLAYOPT_UPSAMPLE         0x0004
#define PLAYOPT_PAD2XTRANSFORM   0x0008

    
WMARESULT WMARawDecClose (WMARawDecHandle* phWMARawDec);
WMARESULT WMARawDecReset (WMARawDecHandle hWMARawDec);
WMARESULT WMARawDecStatus (WMARawDecHandle hWMARawDec);
WMARESULT WMARawDecDecodeData (WMARawDecHandle hWMARawDec, U32_WMARawDec *pnDecodedSamples);
WMARESULT WMARawDecGetPCM (WMARawDecHandle hWMARawDec, U16_WMARawDec* pcSampleReady, U8_WMARawDec* pbDst, U32_WMARawDec cbDstBufferLength);

extern WMARESULT WMARawDecCBGetData (U8_WMARawDec **ppBuffer, U32_WMARawDec* pcbBuffer, U32_WMARawDec dwUserData);
#ifdef __cplusplus
}
#endif  // __cplusplus


#endif // _WMADECRAW_API_H_
