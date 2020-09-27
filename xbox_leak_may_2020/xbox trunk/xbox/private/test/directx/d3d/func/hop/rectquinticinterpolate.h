/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	rectquinticinterpolate.h

Abstract:

	Data structures for Quintic Interpolate rect patches
	-*-* UNSUPPORTED *-*-

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __RECTQUINTICINTERPOLATE_h__
#define __RECTQUINTICINTERPOLATE_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DRECTPATCH_INFO RectQuinticInterpolateInfo[] = {
	{ 0, 0, 6, 7, 6, D3DBASIS_INTERPOLATE, D3DORDER_QUINTIC },
};
static float RectQuinticInterpolateNumSegments[][4] = {
	{ 32.0f, 32.0f, 32.0f, 32.0f },
	{ 7.0f, 13.0f, 22.0f, 29.0f },	// Bad data
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex RectQuinticInterpolateVertices[] = {
	{-1.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {4.0f, 1.0f, 3.0f, 0.0f, 0.0f},
	{-1.0f, 0.0f, 2.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 2.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {3.0f, 0.5f, 2.0f, 0.0f, 0.0f}, {4.0f, 0.0f, 2.0f, 0.0f, 0.0f},
	{-1.0f, -1.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 1.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {3.0f, 0.5f, 1.0f, 0.0f, 0.0f}, {4.0f, -1.0f, 1.0f, 0.0f, 0.0f},
	{-2.0f, -2.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 0.0f, 0.0f, 0.0f}, {5.0f, -2.0f, 0.0f, 0.0f, 0.0f},
	{-2.0f, -1.0f, -1.0f, 0.0f, 0.0f}, {0.0f, -0.5f, -1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, -1.0f, 0.0f, 0.0f}, {2.0f, 0.0f, -1.0f, 0.0f, 0.0f}, {3.0f, -0.5f, -1.0f, 0.0f, 0.0f}, {5.0f, -1.0f, -1.0f, 0.0f, 0.0f},
	{-3.0f, -2.0f, -2.0f, 0.0f, 0.0f}, {-1.0f, 1.0f, -2.5f, 0.0f, 0.0f}, {1.0f, -1.0f, -2.0f, 0.0f, 0.0f}, {2.0f, -3.0f, -2.0f, 0.0f, 0.0f}, {4.0f, 0.0f, -2.5f, 0.0f, 0.0f}, {5.0f, -2.0f, -2.0f, 0.0f, 0.0f},
	{-4.0f, -2.0f, -4.0f, 0.0f, 0.0f}, {-2.0f, -1.0f, -4.0f, 0.0f, 0.0f}, {0.0f, 1.0f, -3.0f, 0.0f, 0.0f}, {3.0f, -4.0f, -3.0f, 0.0f, 0.0f}, {5.0f, -2.0f, -4.0f, 0.0f, 0.0f}, {6.0f, -1.0f, -4.0f, 0.0f, 0.0f},
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the bezier vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex RectQuinticInterpolateFrameVertices[] = {
	{-1.0f, 1.0f, 3.0f, 0xffffff00}, {0.0f, 0.0f, 3.0f, 0xffffff00}, {1.0f, 1.0f, 3.0f, 0xffffff00}, {2.0f, 1.0f, 3.0f, 0xffffff00}, {3.0f, 0.0f, 3.0f, 0xffffff00}, {4.0f, 1.0f, 3.0f, 0xffffff00},
	{-1.0f, 0.0f, 2.0f, 0xffffff00}, {0.0f, 0.5f, 2.0f, 0xffffff00}, {1.0f, 2.0f, 2.0f, 0xffffff00}, {2.0f, 2.0f, 2.0f, 0xffffff00}, {3.0f, 0.5f, 2.0f, 0xffffff00}, {4.0f, 0.0f, 2.0f, 0xffffff00},
	{-1.0f, -1.0f, 1.0f, 0xffffff00}, {0.0f, 0.5f, 1.0f, 0xffffff00}, {1.0f, 2.0f, 1.0f, 0xffffff00}, {2.0f, 2.0f, 1.0f, 0xffffff00}, {3.0f, 0.5f, 1.0f, 0xffffff00}, {4.0f, -1.0f, 1.0f, 0xffffff00},
	{-2.0f, -2.0f, 0.0f, 0xffffff00}, {0.0f, 0.0f, 0.0f, 0xffffff00}, {1.0f, 1.0f, 0.0f, 0xffffff00}, {2.0f, 1.0f, 0.0f, 0xffffff00}, {3.0f, 0.0f, 0.0f, 0xffffff00}, {5.0f, -2.0f, 0.0f, 0xffffff00},
	{-2.0f, -1.0f, -1.0f, 0xffffff00}, {0.0f, -0.5f, -1.0f, 0xffffff00}, {1.0f, 0.0f, -1.0f, 0xffffff00}, {2.0f, 0.0f, -1.0f, 0xffffff00}, {3.0f, -0.5f, -1.0f, 0xffffff00}, {5.0f, -1.0f, -1.0f, 0xffffff00},
	{-3.0f, -2.0f, -2.0f, 0xffffff00}, {-1.0f, 1.0f, -2.5f, 0xffffff00}, {1.0f, -1.0f, -2.0f, 0xffffff00}, {2.0f, -3.0f, -2.0f, 0xffffff00}, {4.0f, 0.0f, -2.5f, 0xffffff00}, {5.0f, -2.0f, -2.0f, 0xffffff00},
	{-4.0f, -2.0f, -4.0f, 0xffffff00}, {-2.0f, -1.0f, -4.0f, 0xffffff00}, {0.0f, 1.0f, -3.0f, 0xffffff00}, {3.0f, -4.0f, -3.0f, 0xffffff00}, {5.0f, -2.0f, -4.0f, 0xffffff00}, {6.0f, -1.0f, -4.0f, 0xffffff00},
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD RectQuinticInterpolateFrameIndices[] = {
	0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
	6, 7, 7, 8, 8, 9, 9, 10, 10, 11,
	12, 13, 13, 14, 14, 15, 15, 16, 16, 17,
	18, 19, 19, 20, 20, 21, 21, 22, 22, 23,
	24, 25, 25, 26, 26, 27, 27, 28, 28, 29,
	30, 31, 31, 32, 32, 33, 33, 34, 34, 35,
	36, 37, 37, 38, 38, 39, 39, 40, 40, 41,
	0, 6, 6, 12, 12, 18, 18, 24, 24, 30, 30, 36,
	1, 7, 7, 13, 13, 19, 19, 25, 25, 31, 31, 37,
	2, 8, 8, 14, 14, 20, 20, 26, 26, 32, 32, 38,
	3, 9, 9, 15, 15, 21, 21, 27, 27, 33, 33, 39,
	4, 10, 10, 16, 16, 22, 22, 28, 28, 34, 34, 40,
	5, 11, 11, 17, 17, 23, 23, 29, 29, 35, 35, 41,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT RectQuinticInterpolateNumVertices	= sizeof(RectQuinticInterpolateVertices) / sizeof(HopVertex);
static UINT RectQuinticInterpolateNumIndices	= sizeof(RectQuinticInterpolateFrameIndices) / sizeof(WORD);
static UINT RectQuinticInterpolateNumPatches	= 1;
static UINT RectQuinticInterpolateNumLines		= RectQuinticInterpolateNumIndices / 2;

#endif
