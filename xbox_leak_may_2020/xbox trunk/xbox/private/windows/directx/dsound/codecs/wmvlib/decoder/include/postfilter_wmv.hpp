//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       postfilter.hpp
//
//--------------------------------------------------------------------------

#ifndef __POSTFILTER_H__
#define __POSTFILTER_H__

#include "wmvdec_api.h"

#define DEBLOCKMB_ARGS      tWMVDecInternalMember *pWMVDec, \
                            PixelC __huge        *ppxliY,           \
                            PixelC __huge        *ppxliU,           \
                            PixelC __huge        *ppxliV,           \
                            Bool_WMV                  bDoLeft,          \
                            Bool_WMV                  bDoRightHalfEdge, \
                            Bool_WMV                  bDoTop,           \
                            Bool_WMV                  bDoBottomHalfEdge,\
                            Bool_WMV                  bDoMiddle,        \
                            I32_WMV                   iStepSize,        \
                            I32_WMV                   iWidthPrevY,      \
                            I32_WMV                   iWidthPrevUV

#define APPLYSMOOTHING_ARGS PixelC                *pixel,           \
                            I32_WMV                   width,            \
                            I32_WMV                   max_diff,         \
                            I32_WMV                   thr

#define DETERMINETHR_ARGS   PixelC                *ptr,             \
                            I32_WMV                   *thr,             \
                            I32_WMV                   *range,           \
                            I32_WMV                   width

#define DERINGMB_ARGS       PixelC __huge        *ppxlcY,           \
                            PixelC __huge        *ppxlcU,           \
                            PixelC __huge        *ppxlcV,           \
                            I32_WMV                  iStepSize,         \
                            I32_WMV                  iWidthPrevY,       \
                            I32_WMV                  iWidthPrevUV


extern Void_WMV (*g_pDeblockMB)(DEBLOCKMB_ARGS);
extern Void_WMV (*g_pApplySmoothing)(APPLYSMOOTHING_ARGS);
extern Void_WMV (*g_pDetermineThreshold)(DETERMINETHR_ARGS);
extern Void_WMV (*g_pDeringMB)(DERINGMB_ARGS);

Void_WMV DeblockMB(DEBLOCKMB_ARGS);
Void_WMV DeblockMB_MMX(DEBLOCKMB_ARGS);
Void_WMV DeblockMB_FASTEST_MMX(DEBLOCKMB_ARGS);
Void_WMV DeblockMB_KNI(DEBLOCKMB_ARGS);
Void_WMV DeringMB(DERINGMB_ARGS);
Void_WMV ApplySmoothing(APPLYSMOOTHING_ARGS);
Void_WMV ApplySmoothing_MMX(APPLYSMOOTHING_ARGS);
Void_WMV DetermineThreshold(DETERMINETHR_ARGS);
Void_WMV DetermineThreshold_MMX(DETERMINETHR_ARGS);
Void_WMV DetermineThreshold_KNI(DETERMINETHR_ARGS);
Void_WMV DetermineThreshold_MMX(U8_WMV *srcptr, I32_WMV *thr, I32_WMV *range, I32_WMV width);

Void_WMV g_InitPostFilter (Bool_WMV bFastDeblock);
#endif
