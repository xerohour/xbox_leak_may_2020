/*************************************************************************

Copyright (c) 1996 -- 2000  Microsoft Corporation

Module Name:

	spatialPredictor.hpp

Abstract:

	Include file for spatial prediction

Revision History:

    Sridhar Srinivasan: 7/20/00

*************************************************************************/

typedef struct t_SpatialPredictor {
    const  U8_WMV   *m_pClampTable;

#ifdef _XBOX
    static const Bool_WMV m_bIsMMX = TRUE;
#else
    static Bool_WMV m_bIsMMX;
#endif

    const PixelC    *m_pRef;
    I32_WMV         m_iRefStride;
    Bool_WMV        m_bFlat;
    I32_WMV         m_iDcValue;
    PixelC          *m_pLeft, *m_pTop;
    PixelC          *m_pNeighbors;
    Bool_WMV        m_bInitZeroPredictor;
    U16_WMV         *m_pSums;
    U16_WMV         *m_pLeftSum;
    U16_WMV         *m_pTopSum;
    I16_WMV         *m_pBuffer16; // buffer for setupZeroPredictor speedup
} t_CSpatialPredictor;

t_SpatialPredictor *t_SpatialPredictorConstruct ();
Void_WMV    t_SpatialPredictorDestruct (t_SpatialPredictor *pSp);
Void_WMV    predictDec (t_SpatialPredictor *pSp, const I32_WMV iOrient,
                 PixelC *pRef, const I32_WMV iRefStride, PixelI *pDelta,
                 Bool_WMV bClear);
Void_WMV    predict_0 (t_SpatialPredictor *pSp, const I32_WMV iOrient,
                 PixelC *pRef, const I32_WMV iRefStride);

#ifdef _WMV_TARGET_X86_ //_M_IX86
Void_WMV    predictDec_MMX (t_SpatialPredictor *pSp, const I32_WMV iOrient,
                 PixelC *pRef, const I32_WMV iRefStride, PixelI *pDelta,
                 Bool_WMV bClear);
Void_WMV    predict_0_MMX (U16_WMV *pTSum, U16_WMV *pLSum, U16_WMV *pWts,
                        PixelC *pRef, I32_WMV iRefStride);
Void_WMV    SetupZeroPredictor_MMX (t_SpatialPredictor *pSp);
Void_WMV    GetRange (U8_WMV *pTop, U8_WMV *pLeft, I32_WMV &iRange,
                      I32_WMV &iDC);
#endif // _M_IX86

/****************************************************************
  Class CContext: local store of contextual information
  Used by encoder and decoder for consistent context generation
****************************************************************/

typedef struct t_ContextWMV {
    I32_WMV m_iRow;
    I32_WMV m_iCol;
    I32_WMV m_iColGt2;
    U8_WMV *m_pData; // one copy enough?
} t_CContextWMV;

t_ContextWMV *t_ContextWMVConstruct (I32_WMV iCol, I32_WMV iRow);
Void_WMV    t_ContextWMVDestruct (t_ContextWMV *pContext);
Void_WMV    t_ContextGetDec (t_ContextWMV *pContext, I32_WMV iX, I32_WMV iY,
              I32_WMV iStepSize, I32_WMV *iPredOrient, I32_WMV *iEstRun);
Void_WMV    t_ContextPutDec (t_ContextWMV *pContext, I32_WMV iX, I32_WMV iY,
                             I32_WMV iOrientContext, I32_WMV iRunContext);
I32_WMV     t_ContextGetChromaDec (t_ContextWMV *pContext, I32_WMV blockX,
                                   I32_WMV blockY);// chroma context
