/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Vertex.h

Abstract:

	Vertex definitions for high order primitives

Author:

	Robert Heitkamp (robheit) 23-Jan-2001

Revision History:

	23-Jan-2001	robheit
		Initial Version

--*/

#ifndef __VERTEX_h__
#define __VERTEX_h__

//------------------------------------------------------------------------------
//	HopVertex
//------------------------------------------------------------------------------
struct HopVertex
{
	float	x, y, z;
	float	tu, tv;
};
#define HOP_VERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

//------------------------------------------------------------------------------
//	HopColorVertex:
//------------------------------------------------------------------------------
struct HopColorVertex
{
	float		x, y, z;
	D3DCOLOR	color;
};
#define HOP_COLORVERTEX	(D3DFVF_XYZ | D3DFVF_DIFFUSE)

//------------------------------------------------------------------------------
//	Vertex Shader for High Order Primitives
//------------------------------------------------------------------------------    
static const DWORD VertexShader[] =
{
	// Vertex Data
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION,  D3DVSDT_FLOAT3),

	// Data generation section implemented as a virtual stream 
	D3DVSD_STREAM_TESS(),

	// Generate normal _using_ the position input and copy it to 
	// the normal register (output)
	D3DVSD_TESSNORMAL(D3DVSDE_POSITION, D3DVSDE_NORMAL),

	// Enables tessellator-generated surface parameters
	D3DVSD_TESSUV(D3DVSDE_TEXCOORD0),

	D3DVSD_END()
};

#endif
