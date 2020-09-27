/***********************************************************

Copyright (c) 1987, 1988, 1989  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987, 1988, 1989 by
Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XConsortium: miregion.c,v 1.60 94/04/17 20:27:49 dpw Exp $ */


#ifndef __NVREGIONSHARED_H
#define __NVREGIONSHARED_H

#if !defined(XFree86LOADER)
typedef struct _Box {
    short x1, y1, x2, y2;
} BoxRec;

typedef struct _Box *BoxPtr;
#else
# include <miscstruct.h>
#endif /* XFree86LOADER */

/* 
 *   clip region
 */

typedef struct _RegData {
    int    size;
    int    numRects;
/*  BoxRec rects[size];   in memory but not explicitly declared */
} RegDataRec, *RegDataPtr;

typedef struct _Region {
    BoxRec  extents;
    RegDataPtr  data;
} RegionRec, *RegionPtr;


#define REGION_NIL(reg) ((reg)->data && !(reg)->data->numRects)
#define REGION_NUM_RECTS(reg) ((reg)->data ? (reg)->data->numRects : 1)
#define REGION_SIZE(reg) ((reg)->data ? (reg)->data->size : 0)
#define REGION_RECTS(reg) ((reg)->data ? (BoxPtr)((reg)->data + 1) \
                           : &(reg)->extents)
#define REGION_BOXPTR(reg) ((BoxPtr)((reg)->data + 1))
#define REGION_BOX(reg,i) (&REGION_BOXPTR(reg)[i])
#define REGION_TOP(reg) REGION_BOX(reg, (reg)->data->numRects)
#define REGION_END(reg) REGION_BOX(reg, (reg)->data->numRects - 1)
#define REGION_SZOF(n) (sizeof(RegDataRec) + ((n) * sizeof(BoxRec)))

#define good(a)

extern void * NV_STDCALL oglXAlloc(size_t size);
extern void  NV_STDCALL oglXFree(void *ptr);
extern void * NV_STDCALL oglXRealloc(void *oldPtr, size_t oldSize, size_t newSize);

extern RegDataPtr  NV_STDCALL oglXAllocData(int n);
extern void  NV_STDCALL oglXFreeData(RegionPtr reg);

extern int  NV_STDCALL nvInverse(RegionPtr newReg, RegionPtr reg1, BoxPtr invRect);
extern int  NV_STDCALL nvIntersect(RegionPtr newReg, RegionPtr reg1, RegionPtr reg2);
extern void  NV_STDCALL nvOptimizeRegion(RegionPtr nvRegion);
   
#endif // __NVREGIONSHARED_H
