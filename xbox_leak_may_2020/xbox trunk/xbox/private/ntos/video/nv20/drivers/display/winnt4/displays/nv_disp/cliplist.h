#ifndef _CLIPLIST_H
#define _CLIPLIST_H
//******************************Module*Header***********************************
// Module Name: cliplist.h
//
// management functions for struct CLIPLIST
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

LONG lClipListCopy(
  OUT       RECTL    *prclOut,
  IN        LONG      cOutMax,
  IN  const struct _CLIPLIST *pClip);

BOOL bClipListAlloc(
  IN OUT struct _CLIPLIST *pClip, // structure to fill out
  IN     LONG      cMax); // number of rects to allocate

BOOL bClipListReAlloc(
  IN OUT struct _CLIPLIST *pClip,  // structure to fill out
  IN     LONG      cNewMax);  // number of rects to allocate

BOOL bClipListFree(
  IN OUT struct _CLIPLIST *pClip);

LONG lClipListPixelCount(struct _CLIPLIST *pClip);

VOID vClipListBoundsCalculate(struct _CLIPLIST *pClip);

BOOL bClipListCheck(
  IN  const struct _CLIPLIST *pClip);
#endif // _CLIPLIST_H
