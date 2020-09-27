//-----------------------------------------------------------------------------
// File: util.cpp
// Desc: Handy utility functions
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "util.h"
#include <tchar.h>

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

inline float MAX(float a, float b) { return a > b ? a : b; }
inline float MIN(float a, float b) { return a < b ? a : b; }

//-----------------------------------------------------------------------------
//  Take the union of two boxes
//-----------------------------------------------------------------------------
static void UnionBox(D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax)
{
	pvMin->x = MIN(pvMin->x, vMin.x);
	pvMin->y = MIN(pvMin->y, vMin.y);
	pvMin->z = MIN(pvMin->z, vMin.z);
	pvMax->x = MAX(pvMax->x, vMax.x);
	pvMax->y = MAX(pvMax->y, vMax.y);
	pvMax->z = MAX(pvMax->z, vMax.z);
}

//-----------------------------------------------------------------------------
//  Find the bounding box for the vertices
//-----------------------------------------------------------------------------
HRESULT ComputeBoundingBox( PVOID pPoints,
							DWORD VertexCount,
							DWORD VertexSize,
							const D3DXMATRIX *pMatrix,
							D3DXVECTOR3 *pvMin,
							D3DXVECTOR3 *pvMax )
{
	// initialize bounds to be reset on the first point
	pvMin->x = pvMin->y = pvMin->z = FLT_MAX;
	pvMax->x = pvMax->y = pvMax->z = -FLT_MAX;

	// walk through all the points, expanding the bbox as needed
    BYTE*       pVertices = (BYTE *)pPoints;
    D3DXVECTOR3 vPos;
	if (pMatrix != NULL)
	{
		while( VertexCount-- )
		{
			D3DXVec3TransformCoord( &vPos, (D3DXVECTOR3*)pVertices, pMatrix );
			UnionBox(pvMin, pvMax, vPos, vPos);	// expand the bounding box to include the point
			pVertices += VertexSize;
		}
	}
	else
	{
		while( VertexCount-- )
		{
			UnionBox(pvMin, pvMax, *(D3DXVECTOR3*)pVertices, *(D3DXVECTOR3*)pVertices);	// expand the bounding box to include the point
			pVertices += VertexSize;
		}
	}
    return S_OK;
}

//-----------------------------------------------------------------------------
//  Take the union of two spheres
//-----------------------------------------------------------------------------
void UnionSphere(D3DXVECTOR3 *pvCenter, float *pfRadius, 
				 const D3DXVECTOR3 &vCenter, const float &fRadius)
{
	D3DXVECTOR3 v =  vCenter - *pvCenter;
	float vlen = D3DXVec3Length(&v);
	float a = MIN(-*pfRadius, vlen - fRadius);
	float b = MAX(*pfRadius, vlen + fRadius);
	*pfRadius = 0.5f * (b - a);
	*pvCenter += (0.5f * (b + a) / vlen) * v;
}

//-----------------------------------------------------------------------------
//  Find the bounding sphere for the vertices
//-----------------------------------------------------------------------------
HRESULT ComputeBoundingSphere(
        PVOID pPointsFVF, 
        DWORD NumVertices, 
        DWORD FVF,
        D3DXVECTOR3 *pvCenter, 
        FLOAT *pfRadius)
{
	if (NumVertices == 0)
	{
		pvCenter->x = pvCenter->y = pvCenter->z = 0.f;
		*pfRadius = 0;
		return S_FALSE;
	}
	UINT cbVertexSize = D3DXGetFVFVertexSize(FVF);
	// use first vertex as starting point
	float feps = 1e-6f;  // use a small epsilon as zero radius
	*pvCenter = *(D3DXVECTOR3 *)pPointsFVF;
	*pfRadius = feps;
	// union in the rest of the points as zero-radius spheres
	for (UINT iVertex = 1; iVertex < NumVertices; iVertex++)
		UnionSphere(pvCenter, pfRadius, *(D3DXVECTOR3 *)((BYTE *)pPointsFVF + cbVertexSize * iVertex), feps);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Draw hedgehog normals
//-----------------------------------------------------------------------------
HRESULT DrawNormalsOffset(LPDIRECT3DVERTEXBUFFER8 pVB, DWORD nVertex, DWORD cbVertexSize, DWORD cbPositionOffset, DWORD cbNormalOffset, float fNormalScale)
{
	HRESULT hr = S_OK;
	const UINT nLineBuf = 200;
	struct Vertex {
		float x, y, z;
		D3DCOLOR diffuse;
	} aVertex[nLineBuf*2];
	Vertex *pVertex = &aVertex[0];
	BYTE *pVertexSrc = NULL;
	UINT nLine = 0;
	D3DCOLOR color0 = D3DCOLOR_RGBA(0x00, 0xff, 0x00, 0xff);
	D3DCOLOR color1 = D3DCOLOR_RGBA(0x00, 0xff, 0xff, 0xff);
	SET_D3DRS( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	SET_D3DRS( D3DRS_ZWRITEENABLE, TRUE);
	SET_D3DRS( D3DRS_ALPHATESTENABLE, FALSE);
	SET_D3DRS( D3DRS_NORMALIZENORMALS, FALSE);
	SET_D3DRS( D3DRS_ALPHABLENDENABLE, FALSE );
	SET_D3DRS( D3DRS_LIGHTING, FALSE);
	SET_D3DVS(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	SET_D3DTEXTURE( 0, NULL);
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0,D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	SET_D3DTSS( 0,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 1, NULL);
	SET_D3DTSS( 1,D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 2, NULL);
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 3, NULL);
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	hr = pVB->Lock(NULL, NULL, (BYTE **)&pVertexSrc, 0l );
	if (FAILED(hr))
		return hr;
	for (UINT iVertex = 0; iVertex < nVertex; iVertex++)
	{
		float *pfPosition = (float *)(pVertexSrc + cbVertexSize * iVertex + cbPositionOffset);
		float *pfNormal = (float *)(pVertexSrc + cbVertexSize * iVertex + cbNormalOffset);
		pVertex[0].x = pfPosition[0];
		pVertex[0].y = pfPosition[1];
		pVertex[0].z = pfPosition[2];
		pVertex[0].diffuse = color0;
		pVertex[1].x = pVertex[0].x + pfNormal[0] * fNormalScale;
		pVertex[1].y = pVertex[0].y + pfNormal[1] * fNormalScale;
		pVertex[1].z = pVertex[0].z + pfNormal[2] * fNormalScale;
		pVertex[1].diffuse = color1;
		pVertex += 2;
		nLine++;
		if (nLine == nLineBuf)
		{
			// buffer is full, so draw what we have
			hr = g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, nLine, aVertex, sizeof(Vertex));
			if (FAILED(hr))
				goto e_Exit;
			nLine = 0;
			pVertex = &aVertex[0];
		}
	}
	if (nLine != 0)
	{
		// draw any left over lines
		hr = g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, nLine, aVertex, sizeof(Vertex));
		if (FAILED(hr))
			goto e_Exit;
	}
e_Exit:
	if (pVertexSrc)
		pVB->Unlock();
	return hr;
}

HRESULT DrawNormals(LPDIRECT3DVERTEXBUFFER8 pVB, DWORD nVertex, DWORD dwFVF, float fNormalScale)
{
	if (!((dwFVF & D3DFVF_XYZ) && (dwFVF & D3DFVF_NORMAL)))
		return S_FALSE; // no normals to draw
	UINT cbVertexSize = D3DXGetFVFVertexSize(dwFVF);
	return DrawNormalsOffset(pVB, nVertex, cbVertexSize, 0 /* position offset */, 12 /* normal offset */, fNormalScale);
}

//-----------------------------------------------------------------------------
//  Draw coordinate axes.
//-----------------------------------------------------------------------------
HRESULT DrawAxes()
{
	const float fAxisRodRadius = 0.05f;
	const float fAxisRodLength = 0.8f;
	const float fAxisConeRadius = fAxisRodRadius * 3.f;
	const float fAxisConeLength = 1.f;
	static struct AxisVertex {
		float x, y, z;
		D3DCOLOR color;
	} XAxis[] = {
		{ 0.f,             0.f,				 0.f }, 
		{ fAxisRodLength,  fAxisRodRadius,   0.f }, 
		{ fAxisRodLength,  fAxisConeRadius,  0.f }, 
		{ fAxisConeLength, 0.f,              0.f }, 
		{ fAxisRodLength,  0.f,              fAxisConeRadius }, 
		{ fAxisRodLength,  0.f,              fAxisRodRadius }, 
		{ 0.f,             0.f,              0.f },
		{ fAxisConeLength, 0.f,              0.f }, 
	};
	const int nVertex = sizeof(XAxis)/sizeof(AxisVertex);
	static AxisVertex YAxis[nVertex];
	static AxisVertex ZAxis[nVertex];
	static bool bInitialized = false;
	if (!bInitialized)
	{
		const D3DCOLOR XColor = 0xffff0000;
		const D3DCOLOR YColor = 0xff00ff00;
		const D3DCOLOR ZColor = 0xff0000ff;
		for (UINT i = 0; i < nVertex; i++)
		{
			XAxis[i].color = XColor;
			YAxis[i].x = XAxis[i].y; YAxis[i].y = XAxis[i].x; YAxis[i].z = XAxis[i].z; // swap x and y
			YAxis[i].color = YColor;
			ZAxis[i].x = XAxis[i].z; ZAxis[i].y = XAxis[i].y; ZAxis[i].z = XAxis[i].x; // swap x and z
			ZAxis[i].color = ZColor;
		}
		bInitialized = true;
	}
	
	// set and save state
	SET_D3DRS(D3DRS_LIGHTING, FALSE);
	SET_D3DRS(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
	SET_D3DVS(D3DFVF_XYZ|D3DFVF_DIFFUSE); // vertex shader
	SET_D3DTEXTURE( 0, NULL);
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0,D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	SET_D3DTSS( 0,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 1, NULL);
	SET_D3DTSS( 1,D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 2, NULL);
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 3, NULL);
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );

	// draw the axes
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, XAxis, sizeof(AxisVertex));
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, YAxis, sizeof(AxisVertex));
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, ZAxis, sizeof(AxisVertex));	
	return S_OK;
}
