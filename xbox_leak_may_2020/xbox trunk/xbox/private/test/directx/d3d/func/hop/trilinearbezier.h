/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	trilinearbezier.h

Abstract:

	Data structures for Linear Bezier tri patches

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __TRILINEARBEZIER_h__
#define __TRILINEARBEZIER_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DTRIPATCH_INFO TriLinearBezierInfo[] = {
	{ 0, 3, D3DBASIS_BEZIER, D3DORDER_LINEAR },
};
static float TriLinearBezierNumSegments[][4] = {
	{ 8.0f, 8.0f, 8.0f },
	{ 7.0f, 13.0f, 22.0f },	// Bad data
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex TriLinearBezierVertices[] = {
	{0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {2.0f, -1.0f, 1.0f, 0.0f, 0.0f}, {1.0f, 3.0f, 2.0f, 0.0f, 0.0f}, 
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the bezier vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex TriLinearBezierFrameVertices[] = {
	{0.0f, 0.0f, 3.0f, 0xffffff00}, {2.0f, -1.0f, 1.0f, 0xffffff00}, {1.0f, 3.0f, 2.0f, 0xffffff00},
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD TriLinearBezierFrameIndices[] = {
	0, 1, 1, 2, 2, 0,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT TriLinearBezierNumVertices	= sizeof(TriLinearBezierVertices) / sizeof(HopVertex);
static UINT TriLinearBezierNumIndices	= sizeof(TriLinearBezierFrameIndices) / sizeof(WORD);
static UINT TriLinearBezierNumPatches	= 1;
static UINT TriLinearBezierNumLines		= TriLinearBezierNumIndices / 2;

#endif
