/*************************************************************************

Copyright (C) 1996 -- 1998  Microsoft Corporation

Module Name:

	idctdec_wmv2.h

Abstract:

	Inverse DCT routines used by WMV2 decoder

Author:

	Marc Holder (marchold@microsoft.com)    05/98
    Bruce Lin (blin@microsoft.com)          06/98
    Ming-Chieh Lee (mingcl@microsoft.com)   06/98

Revision History:

*************************************************************************/

#ifndef __IDCTDEC_WMV2_H_
#define __IDCTDEC_WMV2_H_

#include "wmvdec_api.h"

///////////////////////////////////////////////////////////////////////////
//
// Intra IDCT Functions
//
///////////////////////////////////////////////////////////////////////////
#ifndef OPT_IDCT_ARM
Void_WMV g_IDCTDec_WMV2 (PixelC __huge* piDst, I32_WMV  iOffsetToNextRowForDCT, const PixelI32 __huge* rgiCoefRecon);
#else
extern "C" Void_WMV g_IDCTDec_WMV2 (PixelC __huge* piDst, I32_WMV  iOffsetToNextRowForDCT, const PixelI32 __huge* rgiCoefRecon);
#endif

#ifdef _WMV_TARGET_X86_
Void_WMV g_IDCTDecMMX_WMV2 (PixelC __huge* piDst, I32_WMV  iOffsetToNextRowForDCT, const PixelI32 __huge* rgiCoefRecon);
Void_WMV g_IDCTDecP5_WMV2 (PixelC __huge* piDst, I32_WMV  iOffsetToNextRowForDCT, const PixelI32 __huge* rgiCoefRecon);
#endif //_WMV_TARGET_X86_

///////////////////////////////////////////////////////////////////////////
//
// Inter IDCT Functions
//
///////////////////////////////////////////////////////////////////////////

#ifndef OPT_IDCT_ARM
Void_WMV g_IDCTDec16_WMV2 (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV  iDCTHorzFlags);
#else
extern "C" Void_WMV g_IDCTDec16_WMV2 (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV  iDCTHorzFlags);
#endif

#ifndef _EMB_WMV2_
Void_WMV g_IDCTDec_WMV2 (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV  iDCTHorzFlags);
Void_WMV g_8x4IDCTDec_WMV2 (Buffer __huge* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV  iHalf);
Void_WMV g_4x8IDCTDec_WMV2 (Buffer __huge* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV  iHalf);
Void_WMV g_4x4IDCTDec_WMV2 (Buffer __huge* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV  iQuadrant);
#endif //_EMB_WMV2_

Void_WMV g_4x4IDCTDecMMX_WMV2 (Buffer __huge* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV  iQuadrant);
#ifdef _WMV_TARGET_X86_
Void_WMV g_IDCTDecMMX_WMV2 (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV  iDCTHorzFlags);
Void_WMV g_IDCTDecP5_WMV2 (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV  iDCTHorzFlags);
#endif //_WMV_TARGET_X86_

#if defined(_WMV_TARGET_X86_) || defined(_Embedded_x86)
Void_WMV g_8x4IDCTDecMMX_WMV2 (Buffer __huge* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV  iHalf);
Void_WMV g_4x8IDCTDecMMX_WMV2 (Buffer __huge* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV  iHalf);
#endif //_WMV_TARGET_X86_

Void_WMV g_InitIDCTTablesDec (); 

#endif // __IDCTDEC_H_
