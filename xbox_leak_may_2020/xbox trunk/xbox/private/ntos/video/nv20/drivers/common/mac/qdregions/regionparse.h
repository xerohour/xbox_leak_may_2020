/******************************************************************************
 *
 *       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
 *
 *     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
 *     international laws.  Users and possessors of this source code are
 *     hereby granted a nonexclusive,  royalty-free copyright license to
 *     use this code in individual and commercial software.
 *
 *     Any use of this source code must include,  in the user documenta-
 *     tion and  internal comments to the code,  notices to the end user
 *     as follows:
 *
 *       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
 *
 *     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
 *     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
 *     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
 *     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
 *     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
 *     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
 *     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
 *     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
 *     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
 *     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
 *     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
 *
 *     U.S. Government  End  Users.   This source code  is a "commercial
 *     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
 *     consisting  of "commercial  computer  software"  and  "commercial
 *     computer  software  documentation,"  as such  terms  are  used in
 *     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
 *     ment only as  a commercial end item.   Consistent with  48 C.F.R.
 *     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
 *     all U.S. Government End Users  acquire the source code  with only
 *     those rights set forth herein.
 *
 ******************************************************************************
 *
 * MODULE:  regionparse.h
 *
 * PURPOSE: Contains definitions for region-parsing code.
 *
 ******************************************************************************/

#ifndef __QUICKDRAW__
#include <QuickDraw.h>
#endif

//=====================================================================================================
//
// Definitions
//
//=====================================================================================================

// Flag values for the region parser (parse_rgns and parse_rgns_callback)
#define RIGHT_TO_LEFT	1	// Emit rectangles in right-to-left order (default is left-to-right)
#define BOTTOM_TO_TOP	2	// Emit rectangles in bottom-to-top order (default is top-to-bottom)
#define USE_GLRECT_FMT	4	// Return rectangles in GLRect format (default is QuickDraw rects)

typedef struct {			// Rectangle list information
	int		maxRectCount;		// Maximum allocated rectangles in the list
	int		rectCount;			// How many rectangles were parsed
	void	*rectListPtr;		// Points to QuickDraw or OpenGL rectangles
} RectListInfo, *RectListInfoPtr;

typedef void (*RectCallbackProcPtr)(const void *rectPtr, void *dataPtr);	// dataPtr was passed into parse_rgns_callback

//=====================================================================================================
//
// Prototypes
//
//=====================================================================================================

extern int parse_rgns(void *startcliprectp,	// Points to QuickDraw or OpenGL rectangle, or is nil
						RgnHandle rh1, RgnHandle rh2, RgnHandle rh3,	// Set unused handles to nil
						RectListInfoPtr rectlistinfop, int flags);

extern int parse_rgns_callback(void *startcliprectp,	// Points to QuickDraw or OpenGL rectangle, or is nil
						RgnHandle rh1, RgnHandle rh2, RgnHandle rh3,	// Set unused handles to nil
						RectCallbackProcPtr callbackProcPtr, void *dataPtr,	// dataPtr is passed to callback proc
						int flags);
