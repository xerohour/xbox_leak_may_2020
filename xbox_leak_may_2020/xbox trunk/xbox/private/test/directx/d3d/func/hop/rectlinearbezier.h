/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	rectlinearbezier.h

Abstract:

	Data structures for Linear Bezier surface patches

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __RECTLINEARBEZIER_h__
#define __RECTLINEARBEZIER_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Vertex.h"

//------------------------------------------------------------------------------
//	Patch Information:
//------------------------------------------------------------------------------
static D3DRECTPATCH_INFO RectLinearBezierInfo[] = {
	{ 0, 0, 2, 2, 2, D3DBASIS_BEZIER, D3DORDER_LINEAR },
};
static float RectLinearBezierNumSegments[][4] = {
//	{ 1.0f, 1.0f, 1.0f, 1.0f },
//	{ 6.0f, 6.0f, 6.0f, 6.0f },
	{ 18.0f, 18.0f, 18.0f, 18.0f },
//	{ 16.0f, 24.0f, 32.0f, 64.0f },
//	{ 32.0f, 32.0f, 32.0f, 32.0f },
//	{ 31.0f, 31.0f, 31.0f, 31.0f },
};

//------------------------------------------------------------------------------
//	Patch Vertices:
//------------------------------------------------------------------------------
static HopVertex RectLinearBezierVertices[] = {
	{0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 3.0f, 0.0f, 0.0f}, 
	{0.0f, 0.0f, 2.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 2.0f, 0.0f, 0.0f}, 
//	{0.0f, 0.0f, 3.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 3.0f, 0.0f, 0.0f}, 
//	{0.0f, 2.0f, 2.0f, 0.0f, 0.0f}, {1.0f, 0.5f, 2.0f, 0.0f, 0.0f}, 
};

//------------------------------------------------------------------------------
//	Frame Vertices: Just copy the bezier vertices and add color if value change
//------------------------------------------------------------------------------
static HopColorVertex RectLinearBezierFrameVertices[] = {
	{0.0f, 0.0f, 3.0f, 0xffffff00}, {1.0f, 0.0f, 3.0f, 0xffffff00}, 
	{0.0f, 0.0f, 2.0f, 0xffffff00}, {1.0f, 0.0f, 2.0f, 0xffffff00}, 

//	{0.0f, 0.0f, 3.0f, 0xffffff00}, {1.0f, 2.0f, 3.0f, 0xffffff00}, 
//	{0.0f, 2.0f, 2.0f, 0xffffff00}, {1.0f, 0.5f, 2.0f, 0xffffff00}, 
};

//------------------------------------------------------------------------------
//	Frame Indices:
//------------------------------------------------------------------------------
static WORD RectLinearBezierFrameIndices[] = {
	0, 1, 2, 3, 0, 2, 1, 3,
};

//------------------------------------------------------------------------------
//	Number of Patches:
//------------------------------------------------------------------------------
static UINT RectLinearBezierNumVertices	= sizeof(RectLinearBezierVertices) / sizeof(HopVertex);
static UINT RectLinearBezierNumIndices	= sizeof(RectLinearBezierFrameIndices) / sizeof(WORD);
static UINT RectLinearBezierNumPatches	= 1;
static UINT RectLinearBezierNumLines	= RectLinearBezierNumIndices / 2;

#endif
