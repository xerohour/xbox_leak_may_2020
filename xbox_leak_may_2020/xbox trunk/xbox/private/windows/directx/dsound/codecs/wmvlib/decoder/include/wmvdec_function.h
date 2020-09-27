#include "wmvdec_api.h"
#include "wmvdec_member.h"

#ifndef __WMVDEC_FUNCTION_H_
#define __WMVDEC_FUNCTION_H_


// ===========================================================================
// private functions
// ===========================================================================

// inits
tWMVDecodeStatus WMVInternalMemberInit (
    tWMVDecInternalMember *pWMVDec,
    U32_WMV uiUserData,
    U32_WMV uiFOURCCCompressed,
    Float_WMV fltFrameRate,
    Float_WMV fltBitRate,
    I32_WMV iWidthSource,
    I32_WMV iHeightSource,
    I32_WMV iPostFilterLevel
);

// destroys
tWMVDecodeStatus WMVInternalMemberDestroy (tWMVDecInternalMember *pWMVDec);

// Sequence level decoding
tWMVDecodeStatus setSliceCode (tWMVDecInternalMember *pWMVDec, I32_WMV iSliceCode);
tWMVDecodeStatus decodeVOLHead (tWMVDecInternalMember *pWMVDec);

// frame level decoding

Void_WMV WMVideoDecAssignMotionCompRoutines (tWMVDecInternalMember *pWMVDec);
Void_WMV WMVideoDecSwapCurrAndRef (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeFrameHead (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeClipInfo (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus WMVideoDecDecodeI_X8 (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeI (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeP (tWMVDecInternalMember *pWMVDec);
#ifdef _SUPPORT_POST_FILTERS_
tWMVDecodeStatus WMVideoDecDecodeIDeblock (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodePDeblock (tWMVDecInternalMember *pWMVDec);
#endif

// BMP/color conversion routines

tWMVDecodeStatus WMVideoDecInitBMPInfo (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecColorConvert (tWMVDecInternalMember *pWMVDec, U8_WMV* pucDecodedOutput);

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_GRAY4) || defined (OUTPUT_RGB8) || defined (OUTPUT_RGB12) ||defined (OUTPUT_RGB16) || defined (OUTPUT_RGB24) || defined (OUTPUT_RGB32)
tWMVDecodeStatus g_InitDstTables_WMV (tWMVDecInternalMember *pWMVDec);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8) || defined (OUTPUT_RGB16) || defined (OUTPUT_RGB24) || defined (OUTPUT_RGB32)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_GRAY4)
Void_WMV WMVideoDecUpdateDstPartialMBGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
Void_WMV g_InitDstDitherMap_WMV ();
Void_WMV WMVideoDecUpdateDstPartialMBRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB12)
Void_WMV WMVideoDecUpdateDstPartialMBRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
#   ifndef OPT_DECOLORCONV_ARM
Void_WMV WMVideoDecUpdateDstMBRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#else
extern "C" Void_WMV WMVideoDecUpdateDstMBRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
extern "C" Void_WMV WMVideoDecUpdateDstBlkRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)
Void_WMV WMVideoDecUpdateDstPartialMBRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
#   ifndef OPT_DECOLORCONV_ARM
    Void_WMV WMVideoDecUpdateDstMBRGB16 (
        tWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
    Void_WMV WMVideoDecUpdateDstBlkRGB16 (
        tWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
#   else
    extern "C" Void_WMV WMVideoDecUpdateDstMBRGB16 (
        tWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
    extern "C" Void_WMV WMVideoDecUpdateDstBlkRGB16 (
        tWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
#   endif //OPT_DECOLORCONV_ARM
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB24)
Void_WMV WMVideoDecUpdateDstPartialMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);

#   if	!defined(OPT_DECOLORCONV_SH4)
Void_WMV WMVideoDecUpdateDstMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV *pBits,
    const U8_WMV *pucCurrYMB, const U8_WMV *pucCurrUMB, const U8_WMV *pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);

#   else
    extern "C" Void_WMV WMVideoDecUpdateDstMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
    extern "C" Void_WMV WMVideoDecUpdateDstBlkRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);


//	extern "C" Void_WMV WMVVideoDecInitDecolorSH4 (float *matrix); This is defined if we use a vector transformation for decolor

#endif //OPT_DECOLORCONV_SH4
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB24)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)
Void_WMV WMVideoDecUpdateDstPartialMBRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)

// packed YUV formats

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)
Void_WMV WMVideoDecUpdateDstPartialMBYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)
Void_WMV WMVideoDecUpdateDstPartialMBUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YVYU)
Void_WMV WMVideoDecUpdateDstPartialMBYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);

Void_WMV WMVideoDecUpdateDstMBYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)

//Init routines
tWMVDecodeStatus VodecConstruct (
    tWMVDecInternalMember *pWMVDec, 
    U32_WMV               dwFOURCC,
    I32_WMV                 iFrameRate,
    I32_WMV                 iWidthSource, 
    I32_WMV                 iHeightSource,
    CInputBitStream_WMV     *pInputBitstream//,
);
tWMVDecodeStatus  VodecDestruct (tWMVDecInternalMember *pWMVDec);
Void_WMV    m_InitFncPtrAndZigzag(tWMVDecInternalMember *pWMVDec);
Void_WMV    setRefreshPeriod(tWMVDecInternalMember *pWMVDec);
Void_WMV    decideMMXRoutines (tWMVDecInternalMember *pWMVDec);
I32_WMV     assignVODataMembers (tWMVDecInternalMember *pWMVDec);
Void_WMV    g_InitDecGlobalVars ();
tWMVDecodeStatus InitDataMembers (
    tWMVDecInternalMember *pWMVDec,     
    I32_WMV                iWidthSource, 
    I32_WMV                iHeightSource
);

Void_WMV DecodeSkipBit(tWMVDecInternalMember *pWMVDec);
Void_WMV decodeVOPHead_WMV2 (tWMVDecInternalMember *pWMVDec);
Void_WMV UpdateDCStepSize(tWMVDecInternalMember *pWMVDec, I32_WMV iStepSize);

// Loopfilter
#if defined(_ARM_ASM_LOOPFILTER_OPT_) || defined(_SH4_ASM_LOOPFILTER_OPT_)
extern "C" Void_WMV g_FilterHorizontalEdge_WMV(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
extern "C" Void_WMV g_FilterVerticalEdge_WMV(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#else
Void_WMV g_FilterHorizontalEdge_WMV(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdge_WMV(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#endif //_ARM_ASM_LOOPFILTER_OPT_
#ifdef _WMV_TARGET_X86_
Void_WMV g_FilterHorizontalEdge_MMX(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdge_MMX(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#endif //_WMV_TARGET_X86_
Void_WMV DeblockSLFrame (
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    Bool_WMV bMultiThread, U32_WMV iThreadID, I32_WMV iMBStartX, I32_WMV iMBEndX, I32_WMV iMBStartY, I32_WMV iMBEndY
); 

//Deblocking 
Void_WMV DeblockIFrame (
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY,
    U8_WMV* ppxliU,
    U8_WMV* ppxliV,
    U32_WMV iMBStartY, U32_WMV iMBEndY
    );
Void_WMV DeblockPFrame (
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY,
    U8_WMV* ppxliU,
    U8_WMV* ppxliV,
    U32_WMV iMBStartY, U32_WMV iMBEndY
    );
Void_WMV AssignDeblokRoutines (tWMVDecInternalMember *pWMVDec);
#if defined(_SUPPORT_POST_FILTERS_) && defined(_WMV_TARGET_X86_)
tWMVDecodeStatus WMVDecideDeblokLevel (tWMVDecInternalMember *pWMVDec, I32_WMV iWidth, I32_WMV iHeight);
#endif

// Block based routines
tWMVDecodeStatus DecodeIMBAcPred (
    tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode*        pmbmd, 
    U8_WMV*  ppxliTextureQMBY, 
    U8_WMV*  ppxliTextureQMBU, 
    U8_WMV*  ppxliTextureQMBV, 
    I16_WMV*            piQuanCoefACPred, 
    I16_WMV**           piQuanCoefACPredTable, 
    Bool_WMV            bLeftMB, 
    Bool_WMV            bTopMB, 
    Bool_WMV            bLeftTopMB
);

tWMVDecodeStatus DecodeIMBAcPred_PFrame (
    tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode*        pmbmd, 
    U8_WMV*  ppxliTextureQMBY, 
    U8_WMV*  ppxliTextureQMBU, 
    U8_WMV*  ppxliTextureQMBV, 
    I16_WMV*            piQuanCoefACPred, 
    I16_WMV**           piQuanCoefACPredTable, 
    Bool_WMV            bLeftMB, 
    Bool_WMV            bTopMB, 
    Bool_WMV            bLeftTopMB
);

#ifndef _EMB_WMV2_
tWMVDecodeStatus DecodePMB (
    tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode*        pmbmd,
	U8_WMV* ppxlcCurrQYMB,
	U8_WMV* ppxlcCurrQUMB,
	U8_WMV* ppxlcCurrQVMB,
	CoordI xRefY, CoordI yRefY,
	CoordI xRefUV, CoordI yRefUV
);
#endif //_EMB_WMV2_

// decode MB header 
tWMVDecodeStatus decodeMBOverheadOfIVOP_MSV (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);
tWMVDecodeStatus decodeMBOverheadOfPVOP_MSV (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);
// decode MV
Bool_WMV decideHybridMVOn (tWMVDecInternalMember *pWMVDec, CoordI x, Bool_WMV bNot1stRowInSlice, const CVector* pmv);
Void_WMV find16x16MVpred (
    tWMVDecInternalMember *pWMVDec, 
    CVector& vecPredHalfPel, 
    const CVector* pmv,
    const CWMVMBMode* pmbmd,
    Bool_WMV bLeftBndry, 
    Bool_WMV bRightBndry, 
    Bool_WMV bTopBndry
);
tWMVDecodeStatus decodeMVMSV (tWMVDecInternalMember *pWMVDec, CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry, Huffman_WMV* pHufMVDec, U8_WMV* uXMvFromIndex, U8_WMV* uYMvFromIndex);

// Decode Coefs
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode);
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MMX (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode);
tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred_MPEG4 (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, Bool_WMV CBP, I16_WMV* pDct, U8_WMV* piZigzagInv);
tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred (tWMVDecInternalMember *pWMVDec, 
	CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, 
	Bool_WMV CBP, 
	I16_WMV* pDct, 
	U8_WMV* piZigzagInv
);

tWMVDecodeStatus decodeIntraBlockAcPred (tWMVDecInternalMember *pWMVDec, 
	CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, 
	CWMVMBMode*        pmbmd,
    U8_WMV   *ppxliTextureQMB,        //place to set the pixels
    I32_WMV             iOffsetToNextRowForDCT,
	I32_WMV             ib, 
    I16_WMV*            piQuanCoef1stRowColAC, 
    I16_WMV*            pRef, 
    Bool_WMV            bPredPattern
);
Void_WMV decodeBitsOfESCCode (tWMVDecInternalMember *pWMVDec);


// Motion Compensation
#ifndef _EMB_WMV2_
Void_WMV MotionCompMixed (MOTION_COMP_ARGS);
Void_WMV MotionCompMixedAndAddError (MOTION_COMP_ADD_ERROR_ARGS);
#endif //_EMB_WMV2_

Void_WMV MotionCompMixed_MMX (MOTION_COMP_ARGS);
Void_WMV MotionCompMixedAndAddError_MMX (MOTION_COMP_ADD_ERROR_ARGS);

Void_WMV MotionCompMixed_KNI (MOTION_COMP_ARGS);
Void_WMV MotionCompMixedAndAddError_KNI (MOTION_COMP_ADD_ERROR_ARGS);


// AC_DC Pred    
I16_WMV* decodeDiffDCTCoef(
    tWMVDecInternalMember *pWMVDec, 
    I16_WMV**   piQuanCoefACPredTable,
    Bool_WMV    bLeftMB,
    Bool_WMV    bTopMB,
    Bool_WMV    bLeftTopMB, 
    Bool_WMV&   bPredPattern
);
I16_WMV* decodeDiffDCTCoef_WMV(
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliTextureQMB,
    I16_WMV**   piQuanCoefACPredTable,
    Bool_WMV    bLeftMB,
    Bool_WMV    bTopMB,
    Bool_WMV    bLeftTopMB, 
    Bool_WMV    bLeftMBAndInter,
    Bool_WMV    bTopMBAndInter,
    Bool_WMV    bLeftTopMBAndInter, 
    Bool_WMV&   bPredPattern,
    I32_WMV     iWidthPrev,
    I32_WMV     iStepSize
);
I16_WMV* decodeDiffDCTCoef_WMV_Y23(
    I16_WMV** piQuanCoefACPredTable, 
    Bool_WMV bLeftMB, 
    Bool_WMV bTopMB, 
    Bool_WMV bLeftTopMB, 
    Bool_WMV& bPredPattern
);
Void_WMV decodeIntraDCAcPredMSV (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE);
I32_WMV BlkAvgX8(const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize);
I32_WMV BlkAvgX8_MMX(const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize);


// Routines to support MP4X's routines.
//#ifdef _WMVONLY_
tWMVDecodeStatus DecodeIMBAcPred_MP4X(tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, U8_WMV* ppxliTextureQMBY, U8_WMV* ppxliTextureQMBU, U8_WMV* ppxliTextureQMBV, 
    I16_WMV* piQuanCoefACPred, I16_WMV** piQuanCoefACPredTable, Bool_WMV bLeftMB, Bool_WMV bTopMB, Bool_WMV bLeftTopMB);
Void_WMV decodeIntraDCAcPred (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, U8_WMV DecodeCodeTableIntraDC[], I32_WMV iMaxCodeWordLngIntraDC);
tWMVDecodeStatus decodeMBOverheadOfIVOP_New (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);
tWMVDecodeStatus decodeMBOverheadOfPVOP_New (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);
tWMVDecodeStatus decodeMV (tWMVDecInternalMember *pWMVDec, CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry);
I32_WMV checkResyncMarker(tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus decodeMBOverheadOfIVOP_MPEG4 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);
tWMVDecodeStatus decodeMBOverheadOfPVOP_MPEG4 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);
Void_WMV decodeIntraDCAcPred_MPEG4 (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, U8_WMV DecodeCodeTableIntraDC[], I32_WMV iMaxCodeWordLngIntraDC);
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MPEG4(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode);
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MMX_MPEG4(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode);
tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred_MPEG4 (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, Bool_WMV CBP, I16_WMV* pDct, U8_WMV* piZigzagInv);
tWMVDecodeStatus decodeVOLHeadMP4S(tWMVDecInternalMember *pWMVDec);
I32_WMV	decodeVideoPacketHeader(tWMVDecInternalMember *pWMVDec, I32_WMV& iCurrentQP);
Void_WMV getDiffMV (tWMVDecInternalMember *pWMVDec, CVector& vctDiffMV);
Void_WMV fitMvInRange (CVector& vctSrc, I32_WMV iMvRange);

#ifdef _EMB_WMV2_
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MPEG4_EMB(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode);

tWMVDecodeStatus DecodePMB_EMB (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
//    const Bool* rgCBP, 
    U8_WMV* ppxlcCurrQYMB,
    U8_WMV* ppxlcCurrQUMB,
    U8_WMV* ppxlcCurrQVMB,
    CoordI xRefY, CoordI yRefY,
    CoordI xRefUV, CoordI yRefUV
);

Void_WMV InitEMB_FuncTable(tWMVDecInternalMember *pWMVDec);

extern "C"  Void_WMV MotionCompMixed011(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed010(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed110(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed001(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed101(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed000(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed100(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed111(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompWAddError01(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern "C" Void_WMV MotionCompWAddError10(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern "C" Void_WMV MotionCompWAddError00(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern "C" Void_WMV MotionCompMixedAlignBlock(U8_WMV*   pBlock, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev);
extern "C" Void_WMV MotionCompMixedHQHI(U8_WMV* ppxlcPredMB,  const U8_WMV* ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * ppxliErrorBuf, U8_WMV* alignedBlock);

extern "C" Void_WMV MotionCompMixed011Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed010Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed110Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed001Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed101Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed000Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed100Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompMixed111Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern "C" Void_WMV MotionCompWAddError01Complete(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern "C" Void_WMV MotionCompWAddError10Complete(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern "C" Void_WMV MotionCompWAddError00Complete(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern "C" Void_WMV MotionCompMixedAlignBlockComplete(U8_WMV*   pBlock, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev);
extern "C" Void_WMV MotionCompMixedHQHIComplete(U8_WMV* ppxlcPredMB, const U8_WMV* ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * ppxliErrorBuf, U8_WMV* alignedBlock);

 tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCodeEMB (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV* pZigzag, EMB_DecodeP_ShortCut *);

#ifdef OPT_IDCT_ARM
extern "C" Void_WMV g_IDCTDec_WMV2_16bit (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
extern "C" Void_WMV g_8x4IDCTDec_WMV2_16bit (Buffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags);
extern "C" Void_WMV g_4x8IDCTDec_WMV2_16bit (Buffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags);
#else
Void_WMV g_IDCTDec_WMV2_16bit (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
Void_WMV g_8x4IDCTDec_WMV2_16bit (Buffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags);
Void_WMV g_4x8IDCTDec_WMV2_16bit (Buffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags);
#endif //OPT_IDCT_ARM

Void_WMV InitEMB_DecodePShortcut(tWMVDecInternalMember *pWMVDec);
#endif

#ifdef COMBINE_LOOPFILTER_RENDERING
tWMVDecodeStatus combine_loopfilter_rendering ( tWMVDecInternalMember *pWMVDec,  U8_WMV* pucDecodedOutput);  // Frame-based deblocking
    //#endif
#endif

Void_WMV Huffman_WMV_construct(Huffman_WMV * pThis);
Void_WMV Huffman_WMV_destruct(Huffman_WMV * pThis);
VResult Huffman_WMV_init (Huffman_WMV * pThis, U32_WMV uiUserData, tPackHuffmanCode_WMV huffArray[], I32_WMV *maxBits, I32_WMV iMAX_STAGES);

#ifdef OPT_HUFFMAN_GET_WMV

//I32_WMV getHuffman(CInputBitStream_WMV *bs,void * data);
U32_WMV getHuffman(CInputBitStream_WMV *bs,TableInfo *tableInfo, HuffDecInfo *decTable);

inline I32_WMV Huffman_WMV_get(Huffman_WMV * pThis, CInputBitStream_WMV *bs)
  {
	  //return getHuffman(bs, pThis);
      return getHuffman(bs, pThis->m_tableInfo, pThis->m_decInfo);
	  
  }
#else
I32_WMV Huffman_WMV_get(Huffman_WMV * pThis, CInputBitStream_WMV *bs);
#endif

Void_WMV BS_construct (CInputBitStream_WMV * pThis, U32_WMV uiUserData);
Void_WMV
BS_reset(
    CInputBitStream_WMV * pThis,
    U8_WMV          *pBuffer,
    U32_WMV        dwBuflen,
    Bool_WMV        bNotEndOfFrame
    );

#ifdef DYNAMIC_EDGEPAD
Void_WMV edgePadInit(tWMVDecInternalMember *pWMVDec, tWMVEdgePad * pEdgePad );
Void_WMV edgePadDestroy(tWMVEdgePad * pEdgePad );
Void_WMV edgePadSetup(tWMVDecInternalMember *pWMVDec, tWMVEdgePad * pEdgePad );
#endif

#if defined(_6BIT_COLORCONV_OPT_) || defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB12) || defined (OUTPUT_GRAY4)
Void_WMV g_EndDstTables_WMV(tWMVDecInternalMember *pWMVDec);
#endif

#endif // __WMVDEC_FUNCTION_H_