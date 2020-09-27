/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	rectquinticbezier.h

Abstract:

	Data strucuters for Quintic Bezier surface patches

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __RECTQUINTICBEZIER_h__
#define __RECTQUINTICBEZIER_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DRECTPATCH_INFO RectQuinticBezierInfo[] = {
	{ 0, 0, 6, 6, 6, D3DBASIS_BEZIER, D3DORDER_QUINTIC },
};
static float RectQuinticBezierNumSegments[][4] = {
	{32.0f, 32.0f, 32.0f, 32.0f},
	{ 7.0f, 13.0f, 22.0f, 29.0f },	// Bad data
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex RectQuinticBezierVertices[] = {
	{-1.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {4.0f, 1.0f, 3.0f, 0.0f, 0.0f},
	{-1.0f, 0.0f, 2.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 2.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {3.0f, 0.5f, 2.0f, 0.0f, 0.0f}, {4.0f, 0.0f, 2.0f, 0.0f, 0.0f},
	{-1.0f, -1.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 1.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {3.0f, 0.5f, 1.0f, 0.0f, 0.0f}, {4.0f, -1.0f, 1.0f, 0.0f, 0.0f},
	{-2.0f, -2.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 0.0f, 0.0f, 0.0f}, {5.0f, -2.0f, 0.0f, 0.0f, 0.0f},
	{-2.0f, -1.0f, -1.0f, 0.0f, 0.0f}, {0.0f, -0.5f, -1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, -1.0f, 0.0f, 0.0f}, {2.0f, 0.0f, -1.0f, 0.0f, 0.0f}, {3.0f, -0.5f, -1.0f, 0.0f, 0.0f}, {5.0f, -1.0f, -1.0f, 0.0f, 0.0f},
	{-3.0f, -2.0f, -2.0f, 0.0f, 0.0f}, {-1.0f, 1.0f, -2.5f, 0.0f, 0.0f}, {1.0f, -1.0f, -2.0f, 0.0f, 0.0f}, {2.0f, -3.0f, -2.0f, 0.0f, 0.0f}, {4.0f, 0.0f, -2.5f, 0.0f, 0.0f}, {5.0f, -2.0f, -2.0f, 0.0f, 0.0f},
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the bezier vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex RectQuinticBezierFrameVertices[] = {
	{-1.0f, 1.0f, 3.0f, 0xffffff00}, {0.0f, 0.0f, 3.0f, 0xffffff00}, {1.0f, 1.0f, 3.0f, 0xffffff00}, {2.0f, 1.0f, 3.0f, 0xffffff00}, {3.0f, 0.0f, 3.0f, 0xffffff00}, {4.0f, 1.0f, 3.0f, 0xffffff00},
	{-1.0f, 0.0f, 2.0f, 0xffffff00}, {0.0f, 0.5f, 2.0f, 0xffffff00}, {1.0f, 2.0f, 2.0f, 0xffffff00}, {2.0f, 2.0f, 2.0f, 0xffffff00}, {3.0f, 0.5f, 2.0f, 0xffffff00}, {4.0f, 0.0f, 2.0f, 0xffffff00},
	{-1.0f, -1.0f, 1.0f, 0xffffff00}, {0.0f, 0.5f, 1.0f, 0xffffff00}, {1.0f, 2.0f, 1.0f, 0xffffff00}, {2.0f, 2.0f, 1.0f, 0xffffff00}, {3.0f, 0.5f, 1.0f, 0xffffff00}, {4.0f, -1.0f, 1.0f, 0xffffff00},
	{-2.0f, -2.0f, 0.0f, 0xffffff00}, {0.0f, 0.0f, 0.0f, 0xffffff00}, {1.0f, 1.0f, 0.0f, 0xffffff00}, {2.0f, 1.0f, 0.0f, 0xffffff00}, {3.0f, 0.0f, 0.0f, 0xffffff00}, {5.0f, -2.0f, 0.0f, 0xffffff00},
	{-2.0f, -1.0f, -1.0f, 0xffffff00}, {0.0f, -0.5f, -1.0f, 0xffffff00}, {1.0f, 0.0f, -1.0f, 0xffffff00}, {2.0f, 0.0f, -1.0f, 0xffffff00}, {3.0f, -0.5f, -1.0f, 0xffffff00}, {5.0f, -1.0f, -1.0f, 0xffffff00},
	{-3.0f, -2.0f, -2.0f, 0xffffff00}, {-1.0f, 1.0f, -2.5f, 0xffffff00}, {1.0f, -1.0f, -2.0f, 0xffffff00}, {2.0f, -3.0f, -2.0f, 0xffffff00}, {4.0f, 0.0f, -2.5f, 0xffffff00}, {5.0f, -2.0f, -2.0f, 0xffffff00},
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD RectQuinticBezierFrameIndices[] = {
	0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
	6, 7, 7, 8, 8, 9, 9, 10, 10, 11,
	12, 13, 13, 14, 14, 15, 15, 16, 16, 17,
	18, 19, 19, 20, 20, 21, 21, 22, 22, 23,
	24, 25, 25, 26, 26, 27, 27, 28, 28, 29,
	30, 31, 31, 32, 32, 33, 33, 34, 34, 35,
	0, 6, 6, 12, 12, 18, 18, 24, 24, 30,
	1, 7, 7, 13, 13, 19, 19, 25, 25, 31,
	2, 8, 8, 14, 14, 20, 20, 26, 26, 32,
	3, 9, 9, 15, 15, 21, 21, 27, 27, 33,
	4, 10, 10, 16, 16, 22, 22, 28, 28, 34,
	5, 11, 11, 17, 17, 23, 23, 29, 29, 35,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT RectQuinticBezierNumVertices	= sizeof(RectQuinticBezierVertices) / sizeof(HopVertex);
static UINT RectQuinticBezierNumIndices		= sizeof(RectQuinticBezierFrameIndices) / sizeof(WORD);
static UINT RectQuinticBezierNumPatches		= RectQuinticBezierNumVertices / 36;
static UINT RectQuinticBezierNumLines		= RectQuinticBezierNumIndices / 2;

#endif
