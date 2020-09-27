//-----------------------------------------------------------------------------
// File: XBPerf.h
//
// Desc: Sample to show off tri-stripping performance results
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>




//-----------------------------------------------------------------------------
// Name: XBPerf_CalcCacheHits()
// Desc: Calculate the number of cache hits and degenerate triangles
//-----------------------------------------------------------------------------
HRESULT XBPerf_CalcCacheHits( D3DPRIMITIVETYPE dwPrimType, DWORD dwVertexSize,
						      WORD* pIndices, DWORD dwNumIndices,
						      DWORD* pdwNumDegenerateTris,
						      DWORD* pdwNumCacheHits,
						      DWORD* pdwNumPagesCrossed );




//-----------------------------------------------------------------------------
// Name: XBPerf_GetCpuSpeed()
// Desc: Returns a calculation of the CPU speed.
//-----------------------------------------------------------------------------
double XBPerf_GetCpuSpeed();
