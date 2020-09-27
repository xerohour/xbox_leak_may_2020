/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	tricubicbspline.h

Abstract:

	Data structures for Cubic BSpline tri patches
	-*-* UNSUPPORTED *-*-

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __TRICUBICBSPLINE_h__
#define __TRICUBICBSPLINE_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DTRIPATCH_INFO TriCubicBSplineInfo[] = {
	{ 0, 10, D3DBASIS_BSPLINE, D3DORDER_CUBIC },
};
static float TriCubicBSplineNumSegments[][4] = {
	{ 32.0f, 32.0f, 32.0f },
	{ 7.0f, 13.0f, 22.0f },	// Bad data
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex TriCubicBSplineVertices[] = {
	{0.0f, 0.0f, 2.0f, 0.0f, 0.0f}, 
	{-0.75f, 1.0f, 1.0f, 0.0f, 0.0f}, {0.75f, 1.0f, 1.0f, 0.0f, 0.0f},
	{-1.5f, 1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, {1.5f, 1.0f, 0.0f, 0.0f, 0.0f}, 
	{-2.25f, 0.0f, -1.0f, 0.0f, 0.0f}, {-0.75f, 1.0f, -1.0f, 0.0f, 0.0f}, {0.75f, 1.0f, -1.0f, 0.0f, 0.0f}, {2.25f, 0.0f, -1.0f, 0.0f, 0.0f},
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the bezier vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex TriCubicBSplineFrameVertices[] = {
	{0.0f, 0.0f, 2.0f, 0xffffff00}, 
	{-0.75f, 1.0f, 1.0f, 0xffffff00}, {0.75f, 1.0f, 1.0f, 0xffffff00}, 
	{-1.5f, 1.0f, 0.0f, 0xffffff00}, {0.0f, 0.0f, 0.0f, 0xffffff00}, {1.5f, 1.0f, 0.0f, 0xffffff00},
	{-2.25f, 0.0f, -1.0f, 0xffffff00}, {-0.75f, 1.0f, -1.0f, 0xffffff00}, {0.75f, 1.0f, -1.0f, 0xffffff00}, {2.25f, 0.0f, -1.0f, 0xffffff00},
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD TriCubicBSplineFrameIndices[] = {
	0, 1, 1, 3, 3, 6, 6, 7, 7, 8, 8, 9, 9, 5, 5, 2, 2, 0,
	3, 7, 1, 4, 4, 8, 
	8, 5,
	7, 4, 4, 2,
	1, 2,
	3, 4, 4, 5,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT TriCubicBSplineNumVertices	= sizeof(TriCubicBSplineVertices) / sizeof(HopVertex);
static UINT TriCubicBSplineNumIndices	= sizeof(TriCubicBSplineFrameIndices) / sizeof(WORD);
static UINT TriCubicBSplineNumPatches	= 1;
static UINT TriCubicBSplineNumLines		= TriCubicBSplineNumIndices / 2;

#endif
