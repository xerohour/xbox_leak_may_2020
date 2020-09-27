/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	trilinearbspline.h

Abstract:

	Data structures for Linear BSpline tri patches
	-*-* UNSUPPORTED *-*-

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __TRILINEARBSPLINE_h__
#define __TRILINEARBSPLINE_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DTRIPATCH_INFO TriLinearBSplineInfo[] = {
	{ 0, 3, D3DBASIS_BSPLINE, D3DORDER_LINEAR },
};
static float TriLinearBSplineNumSegments[][4] = {
	{ 8.0f, 8.0f, 8.0f },
	{ 7.0f, 13.0f, 22.0f },	// Bad data
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex TriLinearBSplineVertices[] = {
	{1.0f, 3.0f, 2.0f, 0.0f, 0.0f}, {2.0f, -1.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, 
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the bezier vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex TriLinearBSplineFrameVertices[] = {
	{0.0f, 0.0f, 3.0f, 0xffffff00}, {2.0f, -1.0f, 1.0f, 0xffffff00}, {1.0f, 3.0f, 2.0f, 0xffffff00},
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD TriLinearBSplineFrameIndices[] = {
	0, 1, 1, 2, 2, 0,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT TriLinearBSplineNumVertices	= sizeof(TriLinearBSplineVertices) / sizeof(HopVertex);
static UINT TriLinearBSplineNumIndices  = sizeof(TriLinearBSplineFrameIndices) / sizeof(WORD);
static UINT TriLinearBSplineNumPatches  = 1;
static UINT TriLinearBSplineNumLines	= TriLinearBSplineNumIndices / 2;

#endif
