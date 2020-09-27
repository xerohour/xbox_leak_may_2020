/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	triquinticbspline.h

Abstract:

	Data structures for Quinitic B-Spline tri patches
	-*-* UNSUPPORTED *-*-

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __TRIQUINTICBSPLINE_h__
#define __TRIQUINTICBSPLINE_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DTRIPATCH_INFO TriQuinticBSplineInfo[] = {
	{ 0, 21, D3DBASIS_BSPLINE, D3DORDER_QUINTIC },
};
static float TriQuinticBSplineNumSegments[][4] = {
	{ 32.0f, 32.0f, 32.0f },
	{ 7.0f, 13.0f, 22.0f },	// Bad data
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex TriQuinticBSplineVertices[] = {
	{0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, 
	{-0.75f, 1.0f, 2.0f, 0.0f, 0.0f}, {0.75f, 1.0f, 2.0f, 0.0f, 0.0f}, 
	{-1.5f, 0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {1.5f, 0.0f, 1.0f, 0.0f, 0.0f}, 
	{-2.25f, -1.0f, 0.0f, 0.0f, 0.0f}, {-0.75f, 1.0f, 0.0f, 0.0f, 0.0f}, {0.75f, 1.0f, 0.0f, 0.0f, 0.0f}, {2.25f, -1.0f, 0.0f, 0.0f, 0.0f},
	{-3.0f, -1.0f, -1.0f, 0.0f, 0.0f}, {-1.5f, 2.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 0.0f, 0.0f}, {1.5f, 2.0f, -1.0f, 0.0f, 0.0f}, {3.0f, -1.0f, -1.0f, 0.0f, 0.0f},
	{-3.75f, 0.0f, -2.0f, 0.0f, 0.0f}, {-2.25f, 1.0f, -2.0f, 0.0f, 0.0f}, {-0.75f, 2.0f, -2.0f, 0.0f, 0.0f}, {0.75f, 2.0f, -2.0f, 0.0f, 0.0f}, {2.25f, 1.0f, -2.0f, 0.0f, 0.0f}, {3.75f, 0.0f, -2.0f, 0.0f, 0.0f},
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the bezier vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex TriQuinticBSplineFrameVertices[] = {
	{0.0f, 0.0f, 3.0f, 0xffffff00}, 
	{-0.75f, 1.0f, 2.0f, 0xffffff00}, {0.75f, 1.0f, 2.0f, 0xffffff00}, 
	{-1.5f, 0.0f, 1.0f, 0xffffff00}, {0.0f, 2.0f, 1.0f, 0xffffff00}, {1.5f, 0.0f, 1.0f, 0xffffff00}, 
	{-2.25f, -1.0f, 0.0f, 0xffffff00}, {-0.75f, 1.0f, 0.0f, 0xffffff00}, {0.75f, 1.0f, 0.0f, 0xffffff00}, {2.25f, -1.0f, 0.0f, 0xffffff00},
	{-3.0f, -1.0f, -1.0f, 0xffffff00}, {-1.5f, 2.0f, -1.0f, 0xffffff00}, {0.0f, 0.0f, -1.0f, 0xffffff00}, {1.5f, 2.0f, -1.0f, 0xffffff00}, {3.0f, -1.0f, -1.0f, 0xffffff00},
	{-3.75f, 0.0f, -2.0f, 0xffffff00}, {-2.25f, 1.0f, -2.0f, 0xffffff00}, {-0.75f, 2.0f, -2.0f, 0xffffff00}, {0.75f, 2.0f, -2.0f, 0xffffff00}, {2.25f, 1.0f, -2.0f, 0xffffff00}, {3.75f, 0.0f, -2.0f, 0xffffff00},
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD TriQuinticBSplineFrameIndices[] = {
	0, 1, 1, 3, 3, 6, 6, 10, 10, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 14, 14, 9, 9, 5, 5, 2, 2, 0,
	10, 16, 6, 11, 11, 17, 3, 7, 7, 12, 12, 18,
	1, 4, 4, 8, 8, 13, 13, 19,
	19, 14, 18, 13, 13, 9, 17, 12, 12, 8, 8, 5, 16, 11, 11, 7, 7, 4, 4, 2,
	1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 8, 9, 10, 11, 11, 12, 12, 13, 13, 14,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT TriQuinticBSplineNumVertices	= sizeof(TriQuinticBSplineVertices) / sizeof(HopVertex);
static UINT TriQuinticBSplineNumIndices		= sizeof(TriQuinticBSplineFrameIndices) / sizeof(WORD);
static UINT TriQuinticBSplineNumPatches		= 1;
static UINT TriQuinticBSplineNumLines		= TriQuinticBSplineNumIndices / 2;

#endif
