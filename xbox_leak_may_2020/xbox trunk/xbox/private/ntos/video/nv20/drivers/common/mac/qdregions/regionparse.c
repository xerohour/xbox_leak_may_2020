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
 * MODULE:  regionparse.c
 *
 * PURPOSE: Contains the code to decompose regions into a list of rectangles.
 *
 ******************************************************************************/

#include <Quickdraw.h>
#include <Memory.h>
#include <stdio.h>

#include "regionparse.h"
#include "debug.h"


//=====================================================================================================
//
// Definitions
//
//=====================================================================================================

#define ENDMARKER		32767	// Marks 'end of line' and 'end of region'
#define UNINITIALIZEDV	65536	// Will never match valid coordinates (-32767 to 32767)
#define MAXCOORDS		3000	// Maximum allowable complexity per line


//=====================================================================================================
//
// Declarations
// This is duplicated from OpenGL Header files
// This is present in source file instead of header to avoid duplicate
// definition of the same structure while compiling OpenGL source files
//
//=====================================================================================================

typedef struct {			// OpenGL rectangle
    int		x0;  
    int		y0;
    int		x1; 
    int		y1; 
} __GLregionRect, *__GLregionRectPtr;

//=====================================================================================================
//
// Globals
//
//=====================================================================================================

static SInt16 gXcoord[MAXCOORDS];	// Horizontal coordinates for the line's region change information
static SInt8 gRgnID[MAXCOORDS];	// Identifies which region(s) each coordinate belongs to (max 8 regions)

//=====================================================================================================
//
// Prototypes for local procedures
//
//=====================================================================================================

static int parse1rgn(Rect *mincliprectp, RgnHandle rh1, RectListInfoPtr rectlistinfop,
						RectCallbackProcPtr callbackProcPtr, void *dataPtr, int flags);
static int parseNrgns(Rect *mincliprectp, int rgncount, RgnHandle *rhap, RectListInfoPtr rectlistinfop,
						RectCallbackProcPtr callbackProcPtr, void *dataPtr, int flags);


//=====================================================================================================
//
// parse_rgns()
//
//	This is the exported procedure that intersects and clips 1-3 input regions, and outputs a list
//	of hardware-efficient rectangles that make up the resulting region.  It does some parameter
//	checking and clipping, then dispatches to the subroutines that handle the one or N-way
//	intersecting region cases.
//
//=====================================================================================================
int parse_rgns(void *startcliprectp,
				RgnHandle rh1, RgnHandle rh2, RgnHandle rh3,
				RectListInfoPtr rectlistinfop, int flags)
{
	int cliptop, clipleft, clipbottom, clipright;	// Minimum rectangle we have to clip to
	int top, left, bottom, right;
	int i, rgncount, rectcount;
	RgnPtr rgnp;
	RgnHandle rh;
	RgnHandle inrgnh[3];	// Up to 3 input regions
	RgnHandle outrgnh[3];	// List of (output) regions we actually have to parse
	void *rectp;			// Generic rectangle pointer cast to the appropriate type
	Rect mergedcliprect;	// Rect resulting from merged cliprect and the region bounding boxes
#if DEBUG_BUILD
	SInt8 hstate[3];		// Hold the prior handle state for the regions
#endif

	if (startcliprectp == nil) {	// No clip rectangle, so start with wide-open bounds
		cliptop = -32767;
		clipleft = -32767;
		clipbottom = 32767;
		clipright = 32767;
	} else if (flags & USE_GLRECT_FMT) {	// GLRect format
		clipleft = ((__GLregionRectPtr)startcliprectp)->x0;
		cliptop = ((__GLregionRectPtr)startcliprectp)->y0;
		clipright = ((__GLregionRectPtr)startcliprectp)->x1;
		clipbottom = ((__GLregionRectPtr)startcliprectp)->y1;
	} else {	// QuickDraw rect format
		cliptop = ((Rect *)startcliprectp)->top;
		clipleft = ((Rect *)startcliprectp)->left;
		clipbottom = ((Rect *)startcliprectp)->bottom;
		clipright = ((Rect *)startcliprectp)->right;
	}

	inrgnh[0] = rh1;
	inrgnh[1] = rh2;
	inrgnh[2] = rh3;
	rgncount = 0;
	rectcount = 0;

	// Check each region, updating the clip rect and collecting non-rectangular regions

	for (i = 0; i < 3; i++) {
		rh = inrgnh[i];
		if (rh == nil)	// Make sure the region handle is good
			continue;
		rgnp = *rh;
		top = rgnp->rgnBBox.top;
		left = rgnp->rgnBBox.left;
		bottom = rgnp->rgnBBox.bottom;
		right = rgnp->rgnBBox.right;
		if (top >= bottom || left >= right)	// Empty region, nothing gets drawn
			goto done;
		if (rgnp->rgnSize > 10)	// A non-rectangular region we have to parse
			outrgnh[rgncount++] = rh;
		// Update the minimum clipping rectangle if the new bounds are smaller
		if (top > cliptop)
			cliptop = top;
		if (left > clipleft)
			clipleft = left;
		if (bottom < clipbottom)
			clipbottom = bottom;
		if (right < clipright)
			clipright = right;
	}

	// If none of the regions were non-rectangular, just return the cliprect
	if (rgncount == 0) {
		rectp = rectlistinfop->rectListPtr;
		if (flags & USE_GLRECT_FMT) {	// GLRect format
			((__GLregionRectPtr)rectp)->x0 = clipleft;
			((__GLregionRectPtr)rectp)->y0 = cliptop;
			((__GLregionRectPtr)rectp)->x1 = clipright;
			((__GLregionRectPtr)rectp)->y1 = clipbottom;
		} else {	// QuickDraw rect format
			((Rect *)rectp)->top = cliptop;
			((Rect *)rectp)->left = clipleft;
			((Rect *)rectp)->bottom = clipbottom;
			((Rect *)rectp)->right = clipright;
		}
		rectlistinfop->rectCount = 1;
		return(1);
	}

	mergedcliprect.top = cliptop;
	mergedcliprect.left = clipleft;
	mergedcliprect.bottom = clipbottom;
	mergedcliprect.right = clipright;

#if DEBUG_BUILD	// Lock down the region so they don't slide if we're using GUI debuggers that can move memory
	for (i = 0; i < rgncount; i++) {
		hstate[i] = HGetState((Handle)outrgnh[i]);
		HLock((Handle)outrgnh[i]);
	}
#endif

	if (rgncount == 1)	// Use a faster routine for the one-region case
		rectcount = parse1rgn(&mergedcliprect, outrgnh[0], rectlistinfop, nil, nil, flags);
	else
		rectcount = parseNrgns(&mergedcliprect, rgncount, outrgnh, rectlistinfop, nil, nil, flags);

#if DEBUG_BUILD
	for (i = 0; i < rgncount; i++) {
		HSetState((Handle)outrgnh[i], hstate[i]);
	}
#endif

done:
	return(rectcount);
}

//=====================================================================================================
//
// parse_rgns_callback()
//
//	This is the exported procedure that intersects and clips 1-3 input regions, and calls back
//	(using the supplied procptr) a routine that gets passed each rectangle discovered in the
//	region decomposition.  (The non-callback version passes back a list of all the rectangles.)
//
//=====================================================================================================
int parse_rgns_callback(void *startcliprectp,	// Points to QuickDraw or OpenGL rectangle, or is nil
						RgnHandle rh1, RgnHandle rh2, RgnHandle rh3,	// Set unused handles to nil
						RectCallbackProcPtr callbackProcPtr, void *dataPtr,	// dataPtr is passed to callback proc
						int flags)
{
	int cliptop, clipleft, clipbottom, clipright;	// Minimum rectangle we have to clip to
	int top, left, bottom, right;
	int i, rgncount, rectcount;
	RgnPtr rgnp;
	RgnHandle rh;
	RgnHandle inrgnh[3];	// Up to 3 input regions
	RgnHandle outrgnh[3];	// List of (output) regions we actually have to parse
	void *rectp;			// Generic rectangle pointer cast to the appropriate type
	Rect mergedcliprect;	// Rect resulting from merged cliprect and the region bounding boxes
	__GLregionRect mergedcliprect_gl;	// OpenGL rect version of merged cliprect
#if DEBUG_BUILD
	SInt8 hstate[3];		// Hold the prior handle state for the regions
#endif

	if (startcliprectp == nil) {	// No clip rectangle, so start with wide-open bounds
		cliptop = -32767;
		clipleft = -32767;
		clipbottom = 32767;
		clipright = 32767;
	} else if (flags & USE_GLRECT_FMT) {	// GLRect format
		clipleft = ((__GLregionRectPtr)startcliprectp)->x0;
		cliptop = ((__GLregionRectPtr)startcliprectp)->y0;
		clipright = ((__GLregionRectPtr)startcliprectp)->x1;
		clipbottom = ((__GLregionRectPtr)startcliprectp)->y1;
	} else {	// QuickDraw rect format
		cliptop = ((Rect *)startcliprectp)->top;
		clipleft = ((Rect *)startcliprectp)->left;
		clipbottom = ((Rect *)startcliprectp)->bottom;
		clipright = ((Rect *)startcliprectp)->right;
	}

	inrgnh[0] = rh1;
	inrgnh[1] = rh2;
	inrgnh[2] = rh3;
	rgncount = 0;
	rectcount = 0;

	// Check each region, updating the clip rect and collecting non-rectangular regions

	for (i = 0; i < 3; i++) {
		rh = inrgnh[i];
		if (rh == nil)	// Make sure the region handle is good
			continue;
		rgnp = *rh;
		top = rgnp->rgnBBox.top;
		left = rgnp->rgnBBox.left;
		bottom = rgnp->rgnBBox.bottom;
		right = rgnp->rgnBBox.right;
		if (top >= bottom || left >= right)	// Empty region, nothing gets drawn
			goto done;
		if (rgnp->rgnSize > 10)	// A non-rectangular region we have to parse
			outrgnh[rgncount++] = rh;
		// Update the minimum clipping rectangle if the new bounds are smaller
		if (top > cliptop)
			cliptop = top;
		if (left > clipleft)
			clipleft = left;
		if (bottom < clipbottom)
			clipbottom = bottom;
		if (right < clipright)
			clipright = right;
	}

	// If none of the regions were non-rectangular, just return the cliprect
	if (rgncount == 0) {
		if (flags & USE_GLRECT_FMT) {	// GLRect format
			mergedcliprect_gl.x0 = clipleft;
			mergedcliprect_gl.y0 = cliptop;
			mergedcliprect_gl.x1 = clipright;
			mergedcliprect_gl.y1 = clipbottom;
			rectp = (void *)&mergedcliprect_gl;
		} else {	// QuickDraw rect format
			mergedcliprect.top = cliptop;
			mergedcliprect.left = clipleft;
			mergedcliprect.bottom = clipbottom;
			mergedcliprect.right = clipright;
			rectp = (void *)&mergedcliprect;
		}
		(*callbackProcPtr)(rectp, dataPtr);	// Do the callback for the cliprect
		return(1);
	}

	mergedcliprect.top = cliptop;
	mergedcliprect.left = clipleft;
	mergedcliprect.bottom = clipbottom;
	mergedcliprect.right = clipright;

#if DEBUG_BUILD	// Lock down the region so they don't slide if we're using GUI debuggers that can move memory
	for (i = 0; i < rgncount; i++) {
		hstate[i] = HGetState((Handle)outrgnh[i]);
		HLock((Handle)outrgnh[i]);
	}
#endif

	if (rgncount == 1)	// Use a faster routine for the one-region case
		rectcount = parse1rgn(&mergedcliprect, outrgnh[0], nil, callbackProcPtr, dataPtr, flags);
	else
		rectcount = parseNrgns(&mergedcliprect, rgncount, outrgnh, nil, callbackProcPtr, dataPtr, flags);

#if DEBUG_BUILD
	for (i = 0; i < rgncount; i++) {
		HSetState((Handle)outrgnh[i], hstate[i]);
	}
#endif

done:
	return(rectcount);
}

static int parse1rgn(Rect *mincliprectp, RgnHandle rh1, RectListInfoPtr rectlistinfop,
						RectCallbackProcPtr callbackProcPtr, void *dataPtr, int flags)
{
	int i, j, start, stop, increment, searchstart;
	int rectcount, maxrectcount;
	int rectopenv, rectclosev;
	int recttop, rectbottom, rectleft, rectright;
	int cliptop, clipleft, clipbottom, clipright;
	int horizcoord, coord;
	int xcoordcount, coordcntparity;
	void *rectp;
	RgnPtr rgnp;
	SInt16 *rgnstartp, *rgnlinep, *rgndatap;
	Rect newrect;
	__GLregionRect newrect_gl;	// OpenGL version of newrect

	rgnp = *rh1;
	rgnstartp = (SInt16 *)((Ptr)rgnp + sizeof(Region));	// Skip over the region header

	if (flags & BOTTOM_TO_TOP) {	// Scan the region lines bottom-to-top
		// Skip to the end of the region, and back up to the last two 0x7FFFs
		rgnlinep = (SInt16 *)((Ptr)rgnp + rgnp->rgnSize - 2*sizeof(SInt16));
		// Back up and find the end of the previous line's data
		do {
			// We always back up by two shorts at a time.  The region change points always come
			//	in pairs, and the line vertical coordinate is always preceded by 0x7FFF (pairs
			//	again), except for the very first line in a region.  Since non-rectangular
			//	regions must always have at least two lines of data, we're guaranteed the last
			//	line is preceded by a previous line's 0x7FFF end marker.
			rgnlinep -= 2;
		} while (*rgnlinep != ENDMARKER);
		rgnlinep++;	// rgnlinep now points to the beginning of the line's data (Y coord)
	} else {	// Top to bottom
		rgnlinep = rgnstartp;
	}

	xcoordcount = 0;
	rgndatap = rgnlinep;
	rectopenv = *rgndatap++;	// First data on the line is the vertical coordinate
	// Pre-load the first line of X coordinates into the gXcoord array
	coordcntparity = 0;
	for (horizcoord = *rgndatap++; horizcoord != ENDMARKER || coordcntparity != 0; horizcoord = *rgndatap++) {
		gXcoord[xcoordcount++] = horizcoord;
		coordcntparity ^= 1;
	}

	cliptop = mincliprectp->top;
	clipleft = mincliprectp->left;
	clipbottom = mincliprectp->bottom;
	clipright = mincliprectp->right;

	rectcount = 0;
	if (rectlistinfop != nil) {	// We're building a rectangle list
		maxrectcount = rectlistinfop->maxRectCount;
		rectp = rectlistinfop->rectListPtr;
	}

	do {	// Keep processing lines of the region definition until we're done

		if (flags & BOTTOM_TO_TOP) {	// Scan the region lines bottom-to-top
			if (rgnlinep <= rgnstartp)
				break;
			rgnlinep--;	// Point to the even-earlier line's 0x7FFF end marker
			// Back up and find the end of the previous line's data.
			do {
				// We always back up by two shorts at a time.  The region change points always come
				//	in pairs, and the line vertical coordinate is always preceded by 0x7FFF (pairs
				//	again), except for the very first line in a region, which we also check for.
				rgnlinep -= 2;
			} while (*rgnlinep != ENDMARKER && rgnlinep > rgnstartp);
			rgnlinep++;	// rgnlinep now points to the beginning of the current line's data (Y coord)
			rgndatap = rgnlinep;
			rectclosev = *rgndatap++;
			recttop = rectclosev;	// Going bottom to top, the top Y coordinate is the latest one
			rectbottom = rectopenv;
		} else {	// Doing top to bottom
			rectclosev = *rgndatap++;
			if (rectclosev == ENDMARKER)
				break;
			recttop = rectopenv;	// Going top to bottom, the top Y coordinate was the earlier one
			rectbottom = rectclosev;
		}
		// Clip top and bottom to our clipping vertical bounds
		if (recttop < cliptop)
			recttop = cliptop;
		if (rectbottom > clipbottom)
			rectbottom = clipbottom;

		if (recttop < rectbottom) {	// Don't output rectangles clipped to zero height

			// Each new region definition line will flush out the old rectangles

			if (flags & RIGHT_TO_LEFT) {	// Do the reverse (right-to-left) horizontal rectangle order
				start = xcoordcount - 2;
				stop = -2;
				increment = -2;
			} else {			// Do the normal (left-to-right) horizontal rectangle order
				start = 0;
				stop = xcoordcount;
				increment = 2;
			}
			for (i = start; i != stop; i += increment) {
				rectleft = gXcoord[i];
				rectright = gXcoord[i+1];
				// Eliminate any rectangles horizontally outside the clipping rectangle
				if (rectleft >= clipright || rectright <= clipleft)
					continue;	// Completely skip this rectangle
				if (rectlistinfop != nil) {	// Doing the rectangle list
					if (rectcount < maxrectcount) {	// Don't overflow the rectangle list
						// Clip the horizontal coordinates to the clipping rectangle
						if (rectleft < clipleft)	// Partial or full clipping
							rectleft = clipleft;
						if (rectright > clipright)
							rectright = clipright;
						if (flags & USE_GLRECT_FMT) {
							((__GLregionRectPtr)rectp)[rectcount].x0 = rectleft;
							((__GLregionRectPtr)rectp)[rectcount].y0 = recttop;
							((__GLregionRectPtr)rectp)[rectcount].x1 = rectright;
							((__GLregionRectPtr)rectp)[rectcount].y1 = rectbottom;
						} else {
							((Rect *)rectp)[rectcount].top = recttop;
							((Rect *)rectp)[rectcount].left = rectleft;
							((Rect *)rectp)[rectcount].bottom = rectbottom;
							((Rect *)rectp)[rectcount].right = rectright;
						}
						rectlistinfop->rectCount = rectcount + 1;
					}
				} else {	// Doing the rectangle callback
					// Clip the horizontal coordinates to the clipping rectangle
					if (rectleft < clipleft)	// Partial or full clipping
						rectleft = clipleft;
					if (rectright > clipright)
						rectright = clipright;
					if (flags & USE_GLRECT_FMT) {
						newrect_gl.x0 = rectleft;
						newrect_gl.y0 = recttop;
						newrect_gl.x1 = rectright;
						newrect_gl.y1 = rectbottom;
						rectp = (void *)&newrect_gl;
					} else {	// QuickDraw rect format
						newrect.top = recttop;
						newrect.left = rectleft;
						newrect.bottom = rectbottom;
						newrect.right = rectright;
						rectp = (void *)&newrect;
					}
					(*callbackProcPtr)(rectp, dataPtr);	// Do the callback for newrect
				}
				rectcount++;	// Update the count even if we didn't store the rectangle
			}
		}

		// Update with the new coordinate information

		searchstart = 0;
		coordcntparity = 0;
		for (horizcoord = *rgndatap++; horizcoord != ENDMARKER || coordcntparity != 0; horizcoord = *rgndatap++) {
			coordcntparity ^= 1;
			// See whether the new point needs to be inserted into the coordinate
			//	array, or if it 'cancels' a point already in there.  Since the region's
			//	horizontal coordinates are sorted in increasing order, we know that the
			//	next coordinate to add to or delete from our array won't be any earlier
			//	than we've already searched.  So we save the array index where we did the
			//	last insert or delete, so we don't have to unnecessarily compare the
			//	array's previous entries.  ('searchstart' is used for this.)
			for (i = searchstart; i < xcoordcount; i++) {
				coord = gXcoord[i];
				if (horizcoord > coord)	// New point comes after this one
					continue;	// Keep looking for where to put it
				if (horizcoord < coord) {	// Insert the new point here
					if (xcoordcount == MAXCOORDS)	// Array is already full
						break;	// Prevent overflow
					// Shuffle each point up one slot in the array
					for (j = xcoordcount; j > i; j--)
						gXcoord[j] = gXcoord[j-1];
					goto newpoint;
				} else {	// horizcoord == coord, 'cancel' out this point
					// Shuffle each point down one slot in the array
					for (j = i+1; j < xcoordcount; j++)
						gXcoord[j-1] = gXcoord[j];
					xcoordcount--;
					goto savesearchstart;
				}
			}
			// If we reached here, we add the point at the end of the array
newpoint:
			gXcoord[i] = horizcoord;	// Put the new coordinate here
			xcoordcount++;
savesearchstart:
			searchstart = i;	// Optimize the start of our next search
		}

		rectopenv = rectclosev;	// New rect shares a border with the old one

	} while(1);	// Repeat until we 'break' from the loop

	return(rectcount);
}

#if 0	// Was #if DEBUG_BUILD
static void DUMPRGN(RgnHandle rgnh, int rgnnum)
{
	int newline, i, coordcntparity;
	UInt32 size, offset;
	RgnPtr rgnp;
	Ptr p;

	rgnp = *rgnh;
	size = rgnp->rgnSize;
	LOGPRINTF(("Region %d @ 0x%X: size = %d bytes\n", rgnnum, rgnp, size));
	LOGPRINTF(("Bounding rect (TLBR): %d, %d, %d, %d",
			rgnp->rgnBBox.top, rgnp->rgnBBox.left,
			rgnp->rgnBBox.bottom, rgnp->rgnBBox.right));
	p = (Ptr)rgnp;
	newline = true;
	for (offset = sizeof(Region); offset < size; offset += sizeof(SInt16)) {
		i = *(SInt16 *)(p + offset);
		if (newline) {
			LOGPRINTF(("\n%5d:\t", i));
			newline = false;
			coordcntparity = 0;
		} else {
			LOGPRINTF((" %6d", i));
			if (i == ENDMARKER && coordcntparity == 0)
				newline = true;
			coordcntparity ^= 1;
		}
	}
	LOGPRINTF(("\n"));
}
#else
#define DUMPRGN(rgnh, rgnnum)
#endif

static int parseNrgns(Rect *mincliprectp, int rgncount, RgnHandle *rhap, RectListInfoPtr rectlistinfop,
						RectCallbackProcPtr callbackProcPtr, void *dataPtr, int flags)
{
#define MAXRGNS	3
	int rgniter, i, j, searchstart;
	int rectcount, maxrectcount, intersectstartv, newv;
	int recttop, rectbottom, rectleft, rectright;
	int cliptop, clipleft, clipbottom, clipright;
	int horizcoord, coord;
	int xcoordcount, coordcntparity;
	UInt32 morergndatamask, mask, allrgnmask, rgnID;
	void *rectp;
	RgnPtr rgnp;
	SInt16 *rgndata1p, *rgndata2p;
	SInt16 *rgnlinestartp[MAXRGNS], *rgncoordp[MAXRGNS], *rgnendp[MAXRGNS];
	SInt16 *linestartp, *coordp, *endp;
	Rect newrect;
	__GLregionRect newrect_gl;	// OpenGL version of newrect

#if DEBUG_BUILD
	if (rgncount > MAXRGNS) {
		DBUGPRINTF(("parseNrgns: region count %d exceeds max %d\n", rgncount, MAXRGNS));
		return(0);
	}
#endif

	for (rgniter = 0; rgniter < rgncount; rgniter++) {
		rgnp = *rhap[rgniter];
		rgndata1p = (SInt16 *)((Ptr)rgnp + sizeof(Region));	// Skip over the region header
		rgndata2p = (SInt16 *)((Ptr)rgnp + rgnp->rgnSize - sizeof(SInt16));
		if (flags & BOTTOM_TO_TOP) {	// Scan the region lines bottom-to-top
			rgnlinestartp[rgniter] = rgndata2p;	// Points to the last 0x7FFF marker
			rgncoordp[rgniter] = rgndata2p;		// Also points to the last 0x7FFF marker
			rgnendp[rgniter] = rgndata1p;	// Top line is the last in bottom-to-top order
		} else {	// Top to bottom
			rgnlinestartp[rgniter] = rgndata1p;	// Top line is the first
			rgncoordp[rgniter] = rgndata1p;		// Also points to the first line
			rgnendp[rgniter] = rgndata2p;	// Bottom line is the last
		}
		LOGPRINTF(("parseNrgns(%d): rgn %d at 0x%08X, size = %d bytes\n", rgncount, rgniter, rgnp, rgnp->rgnSize));
//		DUMPRGN(rhap[rgniter], rgniter);
	}

	xcoordcount = 0;
	intersectstartv = UNINITIALIZEDV;	// Means we don't have a region intersection started yet
	cliptop = mincliprectp->top;
	clipleft = mincliprectp->left;
	clipbottom = mincliprectp->bottom;
	clipright = mincliprectp->right;
	allrgnmask = (1 << rgncount) - 1;	// Mask with 'rgncount' bits set

	rectcount = 0;
	if (rectlistinfop != nil) {	// We're building a rectangle list
		maxrectcount = rectlistinfop->maxRectCount;
		rectp = rectlistinfop->rectListPtr;
	}

	goto dowehavergndata;	// Kick off the loop (test the enormous loop condition)

	do {

		if (intersectstartv == UNINITIALIZEDV)	// Don't output rectangles if we haven't intersected yet
			goto mergergndata;

		if (flags & BOTTOM_TO_TOP) {
			recttop = newv;
			rectbottom = intersectstartv;
		} else {	// Top to bottom
			recttop = intersectstartv;
			rectbottom = newv;
		}
		// Clip top and bottom to our clipping vertical bounds
		if (recttop < cliptop)
			recttop = cliptop;
		if (rectbottom > clipbottom)
			rectbottom = clipbottom;
		if (recttop >= rectbottom)	// Don't output rectangles clipped to zero height
			goto mergergndata;

		rgnID = 0;	// Initialize for 'no regions active'

		/* The region ID operations are the key to making sure we only output
			rectangles that are in the intersection of ALL the regions.  We keep
			XORing in each coordinate's saved regionID(s), until the accumulated
			ID mask has all the bits set, indicating this coordinate was included
			in all the regions.  This is one of the rectangle sides, and the other
			is the very next coordinate in scanning order (L-R or R-L). */

		if (flags & RIGHT_TO_LEFT) {	// Do the reverse (right-to-left) horizontal rectangle order
			for (i = xcoordcount-1; i > 0; i--) {
				rgnID ^= gRgnID[i];
				if (rgnID != allrgnmask)
					continue;
				rectleft = gXcoord[i-1];
				rectright = gXcoord[i];
				// Eliminate any rectangles horizontally outside the clipping rectangle
				if (rectleft >= clipright || rectright <= clipleft)
					continue;	// Completely skip this rectangle
				if (rectlistinfop != nil) {	// Doing the rectangle list
					if (rectcount < maxrectcount) {	// Don't overflow the rectangle list
						// Clip the horizontal coordinates to the clipping rectangle
						if (rectleft < clipleft)	// Partial or full clipping
							rectleft = clipleft;
						if (rectright > clipright)
							rectright = clipright;
						LOGPRINTF(("\t\tGot a rect (TLBR): %3d, %3d, %3d, %3d\n", recttop, rectleft, rectbottom, rectright));
						if (flags & USE_GLRECT_FMT) {
							((__GLregionRectPtr)rectp)[rectcount].x0 = rectleft;
							((__GLregionRectPtr)rectp)[rectcount].y0 = recttop;
							((__GLregionRectPtr)rectp)[rectcount].x1 = rectright;
							((__GLregionRectPtr)rectp)[rectcount].y1 = rectbottom;
						} else {
							((Rect *)rectp)[rectcount].top = recttop;
							((Rect *)rectp)[rectcount].left = rectleft;
							((Rect *)rectp)[rectcount].bottom = rectbottom;
							((Rect *)rectp)[rectcount].right = rectright;
						}
						rectlistinfop->rectCount = rectcount + 1;
					}
				} else {	// Doing the rectangle callback
					// Clip the horizontal coordinates to the clipping rectangle
					if (rectleft < clipleft)	// Partial or full clipping
						rectleft = clipleft;
					if (rectright > clipright)
						rectright = clipright;
					if (flags & USE_GLRECT_FMT) {
						newrect_gl.x0 = rectleft;
						newrect_gl.y0 = recttop;
						newrect_gl.x1 = rectright;
						newrect_gl.y1 = rectbottom;
						rectp = (void *)&newrect_gl;
					} else {	// QuickDraw rect format
						newrect.top = recttop;
						newrect.left = rectleft;
						newrect.bottom = rectbottom;
						newrect.right = rectright;
						rectp = (void *)&newrect;
					}
					LOGPRINTF(("\t\tGot a rect (TLBR): %3d, %3d, %3d, %3d\n", recttop, rectleft, rectbottom, rectright));
					(*callbackProcPtr)(rectp, dataPtr);	// Do the callback for newrect
				}
				rectcount++;	// Update the count even if we didn't store the rectangle
			}
		} else {	// Do the normal (left-to-right) horizontal rectangle order
			for (i = 0; i < xcoordcount-1; i++) {
				rgnID ^= gRgnID[i];
				if (rgnID != allrgnmask)
					continue;
				rectleft = gXcoord[i];
				rectright = gXcoord[i+1];
				// Eliminate any rectangles horizontally outside the clipping rectangle
				if (rectleft >= clipright || rectright <= clipleft)
					continue;	// Completely skip this rectangle
				if (rectlistinfop != nil) {	// Doing the rectangle list
					if (rectcount < maxrectcount) {	// Don't overflow the rectangle list
						// Clip the horizontal coordinates to the clipping rectangle
						if (rectleft < clipleft)	// Partial or full clipping
							rectleft = clipleft;
						if (rectright > clipright)
							rectright = clipright;
						LOGPRINTF(("\t\tGot a rect (TLBR): %3d, %3d, %3d, %3d\n", recttop, rectleft, rectbottom, rectright));
						if (flags & USE_GLRECT_FMT) {
							((__GLregionRectPtr)rectp)[rectcount].x0 = rectleft;
							((__GLregionRectPtr)rectp)[rectcount].y0 = recttop;
							((__GLregionRectPtr)rectp)[rectcount].x1 = rectright;
							((__GLregionRectPtr)rectp)[rectcount].y1 = rectbottom;
						} else {
							((Rect *)rectp)[rectcount].top = recttop;
							((Rect *)rectp)[rectcount].left = rectleft;
							((Rect *)rectp)[rectcount].bottom = rectbottom;
							((Rect *)rectp)[rectcount].right = rectright;
						}
						rectlistinfop->rectCount = rectcount + 1;
					}
				} else {	// Doing the rectangle callback
					// Clip the horizontal coordinates to the clipping rectangle
					if (rectleft < clipleft)	// Partial or full clipping
						rectleft = clipleft;
					if (rectright > clipright)
						rectright = clipright;
					LOGPRINTF(("\t\tGot a rect (TLBR): %3d, %3d, %3d, %3d\n", recttop, rectleft, rectbottom, rectright));
					if (flags & USE_GLRECT_FMT) {
						newrect_gl.x0 = rectleft;
						newrect_gl.y0 = recttop;
						newrect_gl.x1 = rectright;
						newrect_gl.y1 = rectbottom;
						rectp = (void *)&newrect_gl;
					} else {	// QuickDraw rect format
						newrect.top = recttop;
						newrect.left = rectleft;
						newrect.bottom = rectbottom;
						newrect.right = rectright;
						rectp = (void *)&newrect;
					}
					(*callbackProcPtr)(rectp, dataPtr);	// Do the callback for newrect
				}
				rectcount++;	// Update the count even if we didn't store the rectangle
			}
		}

mergergndata:
		// Merge in one or more regions to the current span definition
		for (rgniter = 0, mask = 1; rgniter < rgncount; rgniter++, mask <<= 1) {
			if ((morergndatamask & mask) == 0)
				continue;	// Skip exhausted regions
			coordp = rgncoordp[rgniter];
			if (*coordp != newv)	// It's not this region's turn yet
				continue;
			coordp++;		// Skip the v coordinate, now point to the h coordinates
			searchstart = 0;
			horizcoord = *coordp;
			coordcntparity = 0;
			while (horizcoord != ENDMARKER || coordcntparity != 0) {
				coordp++;
				coordcntparity ^= 1;
				// See whether the new point needs to be inserted into the coordinate
				//	array, or if it 'cancels' a point already in there.  Since the region's
				//	horizontal coordinates are sorted in increasing order, we know that the
				//	next coordinate to add to or delete from our array won't be any earlier
				//	than we've already searched.  So we save the array index where we did the
				//	last insert or delete, so we don't have to unnecessarily compare the
				//	array's previous entries.  ('searchstart' is used for this.)
				for (i = searchstart; i < xcoordcount; i++) {
					coord = gXcoord[i];
					if (horizcoord > coord)	// New point comes after this one
						continue;	// Keep looking for where to put it
					if (horizcoord < coord) {	// Insert the new point here
						if (xcoordcount == MAXCOORDS)	// Array is already full
							break;	// Prevent overflow
						// Shuffle each point up one array slot
						for (j = xcoordcount; j > i; j--) {
							gXcoord[j] = gXcoord[j-1];
							gRgnID[j] = gRgnID[j-1];
						}
						goto newpoint;
					} else {	// horizcoord == coord, XOR the region IDs to see if they cancel
						rgnID = gRgnID[i] ^ mask;
						if (rgnID != 0) {	// Save back the update region ID mask
							gRgnID[i] = rgnID;
						} else {	// This region was the last to reference this point, so remove it
							// Shuffle each point down one array slot
							for (j = i+1; j < xcoordcount; j++) {
								gXcoord[j-1] = gXcoord[j];
								gRgnID[j-1] = gRgnID[j];
							}
							xcoordcount--;
						}
						goto savesearchstart;
					}
				}
				// If we reached here, we add the point at the end of the array
newpoint:
				gXcoord[i] = horizcoord;	// Put the new coordinate here
				gRgnID[i] = mask;			// Store the region ID as a bit mask
				xcoordcount++;
savesearchstart:
				searchstart = i;	// Optimize the start of our next search
				horizcoord = *coordp;
			}
			// Save away the current region pointer (points to ENDMARKER)
			rgncoordp[rgniter] = coordp;
#if DEBUG_BUILD
			LOGPRINTF(("parseNrgns(%d) after rgn %d --", rgncount, rgniter));
			linestartp = rgnlinestartp[rgniter];
			LOGPRINTF((" %4d:", *linestartp));
			coordcntparity = 0;
			for (linestartp++; *linestartp != ENDMARKER || coordcntparity != 0; linestartp++) {
				LOGPRINTF((" %4d", *linestartp));
				coordcntparity ^= 1;
			}
			LOGPRINTF(("\n"));
			LOGPRINTF(("\tgXcoord =\t"));
			for (i = 0; i < xcoordcount; i++)
				LOGPRINTF((" %4d", gXcoord[i]));
			LOGPRINTF(("\n"));
			LOGPRINTF(("\t gRgnID =\t"));
			for (i = 0; i < xcoordcount; i++)
				LOGPRINTF((" %4d", gRgnID[i]));
			LOGPRINTF(("\n"));
#endif
		}
		intersectstartv = UNINITIALIZEDV;	// We need to look for the start of an intersection again
		rgnID = 0;
		for (i = 0; i < xcoordcount-1; i++) {
			rgnID ^= gRgnID[i];
			if (rgnID != allrgnmask)
				continue;
			// We have a region intersection in this line of data
			intersectstartv = newv;	// Save this vertical coordinate for later rectangle output
			goto dowehavergndata;	// Don't need to check the rest of the line data
		}

dowehavergndata:
		// Update region pointers as needed, and find the next vertical coordinate to process
		morergndatamask = 0;
		if (flags & BOTTOM_TO_TOP) {
			newv = -32767;
			for (rgniter = 0, mask = 1; rgniter < rgncount; rgniter++, mask <<= 1) {
				linestartp = rgnlinestartp[rgniter];
				endp = rgnendp[rgniter];
				if (linestartp == endp)	// No more data in this region
					continue;
				coordp = rgncoordp[rgniter];
				coord = *coordp;
				if (coord == ENDMARKER) {	// Means we need to find the next line beginning
					linestartp--;	// Back up to previous marker
					do {
						// We always back up by two shorts at a time.  The region change points always come
						//	in pairs, and the line vertical coordinate is always preceded by 0x7FFF (pairs
						//	again), except for the very first line in a region, which we also check for.
						//	('endp' points to the highest line in the region, since we're going btm-to-top.)
						linestartp -= 2;
					} while (*linestartp != ENDMARKER && linestartp > endp);
					linestartp++;	// This now points to the beginning of the current line's data (Y coord)
					rgnlinestartp[rgniter] = linestartp;	// Save away the new line start
					rgncoordp[rgniter] = linestartp;	// Update the copy in memory
					coord = *linestartp;	// Get new vertical coordinate
				}
				if (coord > newv)	// Identify the next highest v coordinate (min rectangle height)
					newv = coord;
				morergndatamask |= mask;	// Mark that this region still has data left
			}
		} else {	// Top to bottom
			newv = 32767;
			for (rgniter = 0, mask = 1; rgniter < rgncount; rgniter++, mask <<= 1) {
				linestartp = rgnlinestartp[rgniter];
				endp = rgnendp[rgniter];
				if (linestartp == endp)	// No more data in this region
					continue;
				coordp = rgncoordp[rgniter];
				coord = *coordp;
				if (coord == ENDMARKER) {	// Means we need to go to the next line
					linestartp = coordp + 1;	// Next line starts after the marker
					rgnlinestartp[rgniter] = linestartp;	// Save away the new line start
					rgncoordp[rgniter] = linestartp;	// Update the copy in memory
					if (linestartp == endp)	// No more data in this region
						continue;
					coord = *linestartp;	// Get new vertical coordinate
				}
				if (coord < newv)	// Identify the next lowest v coordinate (min rectangle height)
					newv = coord;
				morergndatamask |= mask;	// Mark that this region still has data left
			}
		}

	} while (morergndatamask != 0);

	return(rectcount);
}
