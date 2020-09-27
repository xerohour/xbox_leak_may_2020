/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	rectlinearbspline.h

Abstract:

	Data structures for Linear B-Spline surface patches

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __RECTLINEARBSPLINE_h__
#define __RECTLINEARBSPLINE_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DRECTPATCH_INFO RectLinearBSplineInfo[] = {
	{ 0, 0, 4, 7, 4, D3DBASIS_BSPLINE, D3DORDER_LINEAR },
};
static float RectLinearBSplineNumSegments[][4] = {
	{ 32.0f, 32.0f, 32.0f, 32.0f },
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex RectLinearBSplineVertices[] = {
	{0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 3.0f, 0.0f, 0.0f},
	{0.0f, 0.5f, 2.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {3.0f, 0.5f, 2.0f, 0.0f, 0.0f},
	{0.0f, 0.5f, 1.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {3.0f, 0.5f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, -0.5f, -1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, -1.0f, 0.0f, 0.0f}, {2.0f, 0.0f, -1.0f, 0.0f, 0.0f}, {3.0f, -0.5f, -1.0f, 0.0f, 0.0f},
	{-1.0f, 1.0f, -2.5f, 0.0f, 0.0f}, {1.0f, -1.0f, -2.0f, 0.0f, 0.0f}, {2.0f, -3.0f, -2.0f, 0.0f, 0.0f}, {4.0f, 0.0f, -2.5f, 0.0f, 0.0f},
	{-2.0f, -1.0f, -4.0f, 0.0f, 0.0f}, {0.0f, 1.0f, -3.0f, 0.0f, 0.0f}, {3.0f, -4.0f, -3.0f, 0.0f, 0.0f}, {5.0f, -2.0f, -4.0f, 0.0f, 0.0f},
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the BSpline vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex RectLinearBSplineFrameVertices[] = {
	{0.0f, 0.0f, 3.0f, 0xffffff00}, {1.0f, 1.0f, 3.0f, 0xffffff00}, {2.0f, 1.0f, 3.0f, 0xffffff00}, {3.0f, 0.0f, 3.0f, 0xffffff00},
	{0.0f, 0.5f, 2.0f, 0xffffff00}, {1.0f, 2.0f, 2.0f, 0xffffff00}, {2.0f, 2.0f, 2.0f, 0xffffff00}, {3.0f, 0.5f, 2.0f, 0xffffff00},
	{0.0f, 0.5f, 1.0f, 0xffffff00}, {1.0f, 2.0f, 1.0f, 0xffffff00}, {2.0f, 2.0f, 1.0f, 0xffffff00}, {3.0f, 0.5f, 1.0f, 0xffffff00},
	{0.0f, 0.0f, 0.0f, 0xffffff00}, {1.0f, 1.0f, 0.0f, 0xffffff00}, {2.0f, 1.0f, 0.0f, 0xffffff00}, {3.0f, 0.0f, 0.0f, 0xffffff00},
	{0.0f, -0.5f, -1.0f, 0xffffff00}, {1.0f, 0.0f, -1.0f, 0xffffff00}, {2.0f, 0.0f, -1.0f, 0xffffff00}, {3.0f, -0.5f, -1.0f, 0xffffff00},
	{-1.0f, 1.0f, -2.5f, 0xffffff00}, {1.0f, -1.0f, -2.0f, 0xffffff00}, {2.0f, -3.0f, -2.0f, 0xffffff00}, {4.0f, 0.0f, -2.5f, 0xffffff00},
	{-2.0f, -1.0f, -4.0f, 0xffffff00}, {0.0f, 1.0f, -3.0f, 0xffffff00}, {3.0f, -4.0f, -3.0f, 0xffffff00}, {5.0f, -2.0f, -4.0f, 0xffffff00},
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD RectLinearBSplineFrameIndices[] = {
	0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7,
	8, 9, 9, 10, 10, 11, 12, 13, 13, 14, 14, 15,
	0, 4, 4, 8, 8, 12, 1, 5, 5, 9, 9, 13,
	2, 6, 6, 10, 10, 14, 3, 7, 7, 11, 11, 15,
	12, 16, 13, 17, 14, 18, 15, 19,
	16, 17, 17, 18, 18, 19, 20, 21, 21, 22, 22, 23, 24, 25, 25, 26, 26, 27, 
	16, 20, 20, 24, 17, 21, 21, 25, 18, 22, 22, 26, 19, 23, 23, 27,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT RectLinearBSplineNumVertices	= sizeof(RectLinearBSplineVertices) / sizeof(HopVertex);
static UINT RectLinearBSplineNumIndices		= sizeof(RectLinearBSplineFrameIndices) / sizeof(WORD);
static UINT RectLinearBSplineNumPatches		= 1;
static UINT RectLinearBSplineNumLines		= RectLinearBSplineNumIndices / 2;

#endif
