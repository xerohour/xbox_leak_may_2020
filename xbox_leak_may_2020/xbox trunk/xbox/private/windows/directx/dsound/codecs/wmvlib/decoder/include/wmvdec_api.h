/*************************************************************************

Copyright (c) 2000 - 2001  Microsoft Corporation

Module Name:

	wmvdec_api.h

Abstract:

	Decoder API's for WMV.
        decodes WMV2, WMV1, MP43, MP42, MP4S

Author:


Revision History:

*************************************************************************/

#ifndef __WMVDEC_API_H_
#define __WMVDEC_API_H_

// basic types
typedef void* HWMVDecoder;

typedef void Void_WMV;
//typedef long I32_WMV;
typedef int I32_WMV;
typedef unsigned long U32_WMV;
typedef short I16_WMV;
typedef unsigned short U16_WMV;
typedef unsigned short U16Char_WMV;
typedef char I8_WMV;
typedef unsigned char U8_WMV;
typedef long Bool_WMV;
typedef double Double_WMV;
typedef float Float_WMV;

#define TRUE_WMV    1
#define FALSE_WMV   0
#define NULL_WMV   0

typedef enum tagWMVDecodeStatus
{
    WMV_Succeeded = 0,
    WMV_Failed,
    WMV_BadMemory,
    WMV_NoKeyFrameDecoded,
    WMV_CorruptedBits,
    WMV_UnSupportedOutputPixelFormat,
    WMV_UnSupportedCompressedFormat,
    WMV_InValidArguments,
    WMV_BadSource,

    WMV_NoMoreOutput,
    WMV_EndOfFrame,
    WMV_BrokenFrame
} tWMVDecodeStatus;


// GUID definiton
//// SHOULD MERGE WITH WMF_TYPES.H

#ifndef MAKEFOURCC_WMV
#define MAKEFOURCC_WMV(ch0, ch1, ch2, ch3) \
        ((U32_WMV)(U8_WMV)(ch0) | ((U32_WMV)(U8_WMV)(ch1) << 8) |   \
        ((U32_WMV)(U8_WMV)(ch2) << 16) | ((U32_WMV)(U8_WMV)(ch3) << 24 ))

#define mmioFOURCC_WMV(ch0, ch1, ch2, ch3)  MAKEFOURCC_WMV(ch0, ch1, ch2, ch3)
#endif


/******* video output type guids, in preference order  *****/
#define FOURCC_WMV2_WMV     mmioFOURCC_WMV('W','M','V','2')
#define FOURCC_wmv2_WMV     mmioFOURCC_WMV('w','m','v','2')
#define FOURCC_WMV1_WMV     mmioFOURCC_WMV('W','M','V','1')
#define FOURCC_wmv1_WMV     mmioFOURCC_WMV('w','m','v','1')
#define FOURCC_M4S2_WMV     mmioFOURCC_WMV('M','4','S','2')
#define FOURCC_m4s2_WMV     mmioFOURCC_WMV('m','4','s','2')
#define FOURCC_MP43_WMV     mmioFOURCC_WMV('M','P','4','3')
#define FOURCC_mp43_WMV     mmioFOURCC_WMV('m','p','4','3')
#define FOURCC_MP4S_WMV     mmioFOURCC_WMV('M','P','4','S')
#define FOURCC_mp4s_WMV     mmioFOURCC_WMV('m','p','4','s')
#define FOURCC_MP42_WMV     mmioFOURCC_WMV('M','P','4','2')
#define FOURCC_mp42_WMV     mmioFOURCC_WMV('m','p','4','2')

/***********************************************************/


/******* video intput type guids, in preference order  *****/

#define FOURCC_I420_WMV		        0x30323449
#define FOURCC_IYUV_WMV		        0x56555949
#define FOURCC_YV12_WMV		        0x32315659
#define FOURCC_YUY2_WMV		        0x32595559
#define FOURCC_UYVY_WMV		        0x59565955
#define FOURCC_YVYU_WMV		        0x55595659
#define FOURCC_YVU9_WMV		        0x39555659
#define FOURCC_BI_RGB_WMV 	        0x00000000
#define FOURCC_BI_BITFIELDS_WMV	    0x00000003

/***********************************************************/


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

tWMVDecodeStatus WMVideoDecInit (
    HWMVDecoder*    phWMVDecoder,
    U32_WMV         uiUserData,
    U32_WMV         uiFOURCCCompressed,
    Float_WMV       fltFrameRate,
    Float_WMV       fltBitRate,
    I32_WMV         iWidthSource,
    I32_WMV         iHeightSource,
    I32_WMV         iPostFilterLevel
);

tWMVDecodeStatus WMVideoDecClose (HWMVDecoder phWMVDecoder);

tWMVDecodeStatus WMVideoDecDecodeSequenceHeader (HWMVDecoder hWMVDecoder);

tWMVDecodeStatus WMVideoDecDecodeData (HWMVDecoder hWMVDecoder, U16_WMV* puiNumDecodedFrames);

tWMVDecodeStatus WMVideoDecGetOutput (
    HWMVDecoder     hWMVDecoder,
    U32_WMV         uiFOURCCOutput,
    U16_WMV         uiBitsPerPixelOutput,
    U8_WMV*         pucDecodedOutput
);

tWMVDecodeStatus WMVideoDecReset (HWMVDecoder hWMVDecoder);

extern tWMVDecodeStatus WMVDecCBGetData (
    U32_WMV     uiUserData,
    U32_WMV     uintPadBeginning,
    U8_WMV **ppchBuffer, 
    U32_WMV uintUserDataLength,
    U32_WMV* puintActualBufferLength,
    Bool_WMV*    pbNotEndOfFrame
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // __WMVDEC_API_H_
