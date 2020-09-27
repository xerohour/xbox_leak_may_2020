#ifndef _OGLCLIP_H
#define _OGLCLIP_H
//******************************Module*Header***********************************
// Module Name: oglclip.h
//
// OpenGL server side clip management
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

BOOL bOglNegativeClipListCreate(PPDEV ppdev);
BOOL bOglNegativeClipListDestroy(PPDEV ppdev);
BOOL bOglNegativeClipListUpdate(PPDEV ppdev, struct _WNDOBJ *pwo);
BOOL bOglClientIntersectingWithOtherClients(struct _NV_OPENGL_CLIENT_INFO *clientInfo);
void OglClipRectToScreen(RECTL *rect, LONG cxScreen, LONG cyScreen);
void OglCopyClipListToDrawableInfo(PDEV *ppdev, WNDOBJ *pwo, FLONG fl, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);
BOOL bOglClipRectAgainstClipobj(
    IN  CLIPOBJ *pco, 
    IN  RECTL   *prcl);
BOOL bOglClipBoundsUpdate(struct _OGLSERVER *pOglServer);
void CombineRectLists(RECTL *rectList1, ULONG count1, 
                      RECTL *rectList2, ULONG count2,
                      LONG  leftList2, LONG topList2,
                      RECTL *resultList, ULONG *resultCount);

#define COPY_RECT(_DST, _SRC) \
    _DST.left = _SRC.left; \
    _DST.right = _SRC.right; \
    _DST.top = _SRC.top; \
    _DST.bottom = _SRC.bottom;


#endif // _OGLCLIP_H
