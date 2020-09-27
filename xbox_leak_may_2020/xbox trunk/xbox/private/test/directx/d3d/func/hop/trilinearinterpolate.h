/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	trilinearinterpolate.h

Abstract:

	Data structures for Linear Interpolate tri patches
	-*-* UNSUPPORTED *-*-

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __TRILINEARINTERPOLATE_h__
#define __TRILINEARINTERPOLATE_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DTRIPATCH_INFO TriLinearInterpolateInfo[] = {
	{ 0, 4, D3DBASIS_BEZIER, D3DORDER_CUBIC },
};
static float TriLinearInterpolateNumSegments[][4] = {
	{ 32.0f, 32.0f, 32.0f, 32.0f },
	{ 7.0f, 13.0f, 22.0f, 29.0f },	// Bad data
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex TriLinearInterpolateVertices[] = {
	{0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 3.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 3.0f, 0.0f, 0.0f},
	{0.0f, 0.5f, 2.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {3.0f, 0.5f, 2.0f, 0.0f, 0.0f},
	{0.0f, 0.5f, 1.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 1.0f, 0.0f, 0.0f}, {3.0f, 0.5f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {2.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 0.0f, 0.0f, 0.0f},
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the bezier vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex TriLinearInterpolateFrameVertices[] = {
	{0.0f, 0.0f, 3.0f, 0xffffff00}, {1.0f, 1.0f, 3.0f, 0xffffff00}, {2.0f, 1.0f, 3.0f, 0xffffff00}, {3.0f, 0.0f, 3.0f, 0xffffff00},
	{0.0f, 0.5f, 2.0f, 0xffffff00}, {1.0f, 2.0f, 2.0f, 0xffffff00}, {2.0f, 2.0f, 2.0f, 0xffffff00}, {3.0f, 0.5f, 2.0f, 0xffffff00},
	{0.0f, 0.5f, 1.0f, 0xffffff00}, {1.0f, 2.0f, 1.0f, 0xffffff00}, {2.0f, 2.0f, 1.0f, 0xffffff00}, {3.0f, 0.5f, 1.0f, 0xffffff00},
	{0.0f, 0.0f, 0.0f, 0xffffff00}, {1.0f, 1.0f, 0.0f, 0xffffff00}, {2.0f, 1.0f, 0.0f, 0xffffff00}, {3.0f, 0.0f, 0.0f, 0xffffff00},
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD TriLinearInterpolateFrameIndices[] = {
	0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7,
	8, 9, 9, 10, 10, 11, 12, 13, 13, 14, 14, 15,
	0, 4, 4, 8, 8, 12, 1, 5, 5, 9, 9, 13,
	2, 6, 6, 10, 10, 14, 3, 7, 7, 11, 11, 15,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT TriLinearInterpolateNumVertices  = sizeof(TriLinearInterpolateVertices) / sizeof(HopVertex);
static UINT TriLinearInterpolateNumIndices   = sizeof(TriLinearInterpolateFrameIndices) / sizeof(WORD);
static UINT TriLinearInterpolateNumPatches   = 1;
static UINT TriLinearInterpolateNumLines	  = TriLinearInterpolateNumIndices / 2;

#endif
