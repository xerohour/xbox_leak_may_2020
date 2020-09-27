//-----------------------------------------------------------------------------
//  File: Terrain.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <xtl.h>
#include <xgraphics.h>
#include <XBApp.h>
#include <XPath.h>
#include <XBUtil.h>
#include "Clip.h"
#include "Terrain.h"
#include "debug.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//////////////////////////////////////////////////////////////////////
// Helpers for filling geometric data from height-field image
//
UINT GetPixelSize(D3DFORMAT format);
D3DXCOLOR *GetPixelColor(D3DXCOLOR *pColor, BYTE *pPixel, D3DFORMAT format);
HRESULT CopyRectsVB(IDirect3DSurface8* pSourceSurface, CONST RECT* pSourceRectsArray,
					UINT cRects, void *pvDestinationVertices, UINT cbDestinationStride,
					UINT cbDestinationPitch, CONST POINT* pDestinationPointsArray);

//////////////////////////////////////////////////////////////////////
// Name: Grid::Initialize
// Desc: Allocate grid vertex buffer and allocate and fill index buffer.
//
HRESULT	Grid::Initialize(UINT width, UINT height, UINT cbVertexSize)
{
	HRESULT hr;

	m_width = width;
	m_height = height;
	
	// Create the vertex buffer
	SAFE_RELEASE(m_pVB);
	hr = g_pd3dDevice->CreateVertexBuffer(m_width * m_height * cbVertexSize, 0, 0, D3DPOOL_DEFAULT, &m_pVB);

	// Create the index buffer
	SAFE_RELEASE(m_pIB);
	hr = g_pd3dDevice->CreateIndexBuffer(3*2*(m_width-1)*(m_height-1)*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB);
	if (FAILED(hr))
        return hr;

	// Fill the index buffer
	hr = FillIndexBuffer(m_pIB, NULL, NULL);
	if (FAILED(hr))
		return hr;

	return hr;
}

//////////////////////////////////////////////////////////////////////
// Name: FillIndexbuffer
// Desc: Fill index buffer with tile indices from a region of the mesh
//
HRESULT Grid::FillIndexBuffer(LPDIRECT3DINDEXBUFFER8 pIB, const RECT *pRect, UINT *pnTri )
{
	assert(pIB);
	RECT rectFull = {0, 0, m_width, m_height};
	RECT rect;
	if (!pRect)
		rect = rectFull;
	else
	{
		// Clip rect to full rect
		rect = *pRect;
		if (rect.left   < rectFull.left)   rect.left   = rectFull.left;
		if (rect.top    < rectFull.top)    rect.top    = rectFull.top;
		if (rect.right  > rectFull.right)  rect.right  = rectFull.right;
		if (rect.bottom > rectFull.bottom) rect.bottom = rectFull.bottom;
	}
	if (pnTri) *pnTri = 0;
	if (rect.left >= rect.right - 1
		|| rect.top >= rect.bottom - 1)
		return S_FALSE;	// empty rect
	WORD *pIndices;
	if( FAILED( pIB->Lock( 0, 0, (BYTE**)&pIndices, 0 ) ) )
		return E_FAIL;
	long iiTri = 0;
	for (long iY = rect.top; iY < rect.bottom - 1; iY++ )
	{
		for (long iX = rect.left; iX < rect.right - 1; iX++ )
		{
			long lVertex = iY*m_width + iX;
			assert(lVertex >= 0 && lVertex <= 0xffff);
			WORD iVertex = (WORD)lVertex;
#if 0
			if ((iX ^ iY) & 1)
			{
				// flip alternating tiles to get coverage along both 45-degree lines
#endif				
				// tri 0
				pIndices[iiTri] = iVertex;
				pIndices[iiTri+1] = iVertex+m_width;
				pIndices[iiTri+2] = iVertex+m_width+1;
				// tri 1
				pIndices[iiTri+3] = iVertex;
				pIndices[iiTri+4] = iVertex+m_width+1;
				pIndices[iiTri+5] = iVertex+1;
#if 0				
			}
			else
			{
				// tri 0
				pIndices[iiTri] = iVertex;
				pIndices[iiTri+1] = iVertex+m_width;
				pIndices[iiTri+2] = iVertex+1;
				// tri 1
				pIndices[iiTri+3] = iVertex+1;
				pIndices[iiTri+4] = iVertex+m_width;
				pIndices[iiTri+5] = iVertex+m_width+1;
			}
#endif			
			iiTri += 6;
			if (pnTri) *pnTri += 2;
		}
	}
	pIB->Unlock();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Name: Grid::Cleanup
// Desc: Clean up buffers.
//
HRESULT	Grid::Cleanup()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Name: Terrain constructor
// Desc: Set defaults.
//
Terrain::Terrain()
{
	m_pTerrain = NULL;			// terrain height field
	m_pTexture = NULL;			// terrain texture
	m_pTexture2 = NULL;
}

//////////////////////////////////////////////////////////////////////
// Name: Initialize()
// Desc: Create the height field vertex buffers from m_pTerrain
//       height field texture
//
HRESULT Terrain::Initialize(LPDIRECT3DTEXTURE8 pTerrain,
							LPDIRECT3DTEXTURE8 pTexture,
							LPDIRECT3DTEXTURE8 pTexture2,
							const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax,
							const D3DXVECTOR2 &vTextureScale, const D3DXVECTOR2 &vTextureOffset,
							UINT width, UINT height)
{
	HRESULT hr;
	m_vMin = vMin;
	m_vMax = vMax;
	m_vTextureScale = vTextureScale;
	m_vTextureOffset = vTextureOffset;
	
	LPDIRECT3DTEXTURE8 pTerrainOld = m_pTerrain;
	m_pTerrain = pTerrain;
	pTerrain->AddRef();
	SAFE_RELEASE(pTerrainOld);
	
	LPDIRECT3DTEXTURE8 pTextureOld = m_pTexture;
	m_pTexture = pTexture;
	pTexture->AddRef();
	SAFE_RELEASE(pTextureOld);

	LPDIRECT3DTEXTURE8 pTexture2Old = m_pTexture2;
	m_pTexture2 = pTexture2;
	pTexture2->AddRef();
	SAFE_RELEASE(pTexture2Old);

	// Allocate terrain grid
	hr = m_grid.Initialize(width, height, sizeof(TERRAINVERTEX));
	if (FAILED(hr))
		return hr;

	// Fill terrain VB with height field data
	hr = ResetGrid();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetHeightFieldNeighbors
// Desc: Returns four neighbors of normalized point
//-----------------------------------------------------------------------------
HRESULT Terrain::GetHeightFieldNeighbors(const D3DXVECTOR2 &vP, D3DXVECTOR2 *pvFraction, TERRAINVERTEX rVertex[4])
{
	HRESULT hr;
	IDirect3DSurface8 *pSurfaceTerrainLevel = NULL;
	static UINT iLevel = 0;
	hr = m_pTerrain->GetSurfaceLevel(iLevel, &pSurfaceTerrainLevel );
	if (FAILED(hr))
		return hr;
	D3DSURFACE_DESC desc;
	pSurfaceTerrainLevel->GetDesc(&desc);
	D3DFORMAT pixelFormat = desc.Format;
	DWORD dwPixelStride = GetPixelSize(pixelFormat);
	if (dwPixelStride == -1)
	{
		pSurfaceTerrainLevel->Release();
		return E_NOTIMPL;	// we can't handle the format
	}
	float fx0 = vP.x;
	float fy0 = vP.y;
#if 0
	// mirror wrap
	fx0 = fmodf(fx0, 2.f);
	if (fx0 <= -1.f) fx0 += 2.f;			// -2.f to -1.f
	else if (fx0 < 0.f) fx0 = -fx0;			// -1.f to 0.f
	else if (fx0 > 1.f) fx0 = 2.f - fx0;	//  1.f to 2.f
	fy0 = fmodf(fy0, 2.f);
	if (fy0 <= -1.f) fy0 += 2.f;			// -2.f to -1.f
	else if (fy0 < 0.f) fy0 = -fy0;			// -1.f to 0.f
	else if (fy0 > 1.f) fy0 = 2.f - fy0;	//  1.f to 2.f
#endif
	// get pixel coordinates
	float fx = fx0 * desc.Width + 0.5f;
	float fy = fy0 * desc.Height + 0.5f;
	int ix = (int)floorf(fx);
	int iy = (int)floorf(fy);
	pvFraction->x = fx - ix;
	pvFraction->y = fy - iy;
			
	// clamp to edge
	// TODO: better handling of right and bottom wrap boundaries
	if (ix < 0) ix = 0;
	if (iy < 0) iy = 0;
	if (ix >= (int)desc.Width - 1)
		ix = desc.Width - 2;
	if (iy >= (int)desc.Height - 1)
		iy = desc.Height - 2;
		
	// grab neighboring points from height-field texture
	RECT rect = {ix, iy, ix + 2, iy + 2};
	POINT point = {0, 0};
	hr = CopyRectsVB( pSurfaceTerrainLevel, &rect, 1, rVertex, sizeof(TERRAINVERTEX), 2 * sizeof(TERRAINVERTEX), &point);
	pSurfaceTerrainLevel->Release();
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ResampleGrid
// Desc: Sample the height field at the current uv positions
//-----------------------------------------------------------------------------
HRESULT Terrain::ResampleGrid()
{
	HRESULT hr;
	// Loop through all the vertices in the mesh, getting height value from array
	// TODO: use mipmaps for coarser sampling densities
	TERRAINVERTEX *pVertices;
	hr = m_grid.m_pVB->Lock(0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;
	for (UINT iYGrid = 0; iYGrid < m_grid.m_height; iYGrid++)
	for (UINT iXGrid = 0; iXGrid < m_grid.m_width; iXGrid++)
	{
		TERRAINVERTEX *pVertex = &pVertices[iYGrid * m_grid.m_width + iXGrid];
		D3DXVECTOR2 vP(pVertex->vBase.x, pVertex->vBase.z);
		
		// lookup neighboring points from height-field texture
		D3DXVECTOR2 vFraction;
		TERRAINVERTEX rVertex[4];
		hr = GetHeightFieldNeighbors(vP, &vFraction, rVertex);
		if (FAILED(hr))
			goto e_Exit;
		
		// Get interpolated height value
		pVertex->vBase.y  = (1-vFraction.x) * (1-vFraction.y) * rVertex[0].vBase.y;
		pVertex->vBase.y +=   vFraction.x   * (1-vFraction.y) * rVertex[1].vBase.y;
		pVertex->vBase.y += (1-vFraction.x) *   vFraction.y   * rVertex[2].vBase.y;
		pVertex->vBase.y +=   vFraction.x   *   vFraction.y   * rVertex[3].vBase.y;
	}

 e_Exit:
	m_grid.m_pVB->Unlock();
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RefineGrid
// Desc: Expand sampling grid around specified point to get more local samples
//-----------------------------------------------------------------------------
HRESULT Terrain::RefineGrid(const D3DXVECTOR3 &vPosition, float fEpsilon)
{
	HRESULT hr;
	
	// Map to normalized coords
	D3DXVECTOR3 vSize = m_vMax - m_vMin;
	D3DXVECTOR3 vP = vPosition - m_vMin;
	vP.x /= vSize.x;
	vP.z /= vSize.z;

	// Distort sampling field to get more local samples around point.
	// Center grid on point, and shrink samples to be close by.  Then
	// gradually move samples back out to default sampling rate.
	TERRAINVERTEX *pVertices;
	hr = m_grid.m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;
	float fWidthScale = 1.f / (m_grid.m_width - 1);
	float fHeightScale = 1.f / (m_grid.m_height - 1);
	static float fShrink = 0.2f; // shrink factor for highest sampling rate
	static float fRadius = 0.45f; // radius of region with highest sampling rate
	float fRadius2 = fRadius*fRadius;
	float fExpandExp = logf(fShrink) / (fRadius - 0.5f);
	for (UINT iY = 0; iY < m_grid.m_height; iY++)
	for (UINT iX = 0; iX < m_grid.m_width; iX++)
	{
		TERRAINVERTEX *pVertex = &pVertices[iY * m_grid.m_width + iX];
		float u = iX * fWidthScale - 0.5f;	// -0.5 to 0.5
		float v = iY * fHeightScale - 0.5f;
#if 0
		// L2 norm maps a circle
		float r2 = u*u + v*v;
		if (r2 < fRadius2)
		{
			// uniform shrink
			u *= fShrink;
			v *= fShrink;
		}
		else if (r2 < 0.25f) // 0.5*0.5
		{
			// make samples further apart based on distance from point,
			// returning to unit scaling at 0.5
			float fExpand = fShrink * expf(fExpandExp * (sqrtf(r2) - fRadius));
			u *= fExpand;
			v *= fExpand;
		}
#else
		// L infinity norm maps squares
		float fu = fabsf(u), fv = fabsf(v);
		float r = fu > fv ? fu : fv;
		if (r < fRadius)
		{
			// uniform shrink
			u *= fShrink;
			v *= fShrink;
		}
		else if (r < 0.5f)
		{
			// make samples further apart based on distance from point,
			// returning to unit scaling at 0.5
			float fExpand = fShrink * expf(fExpandExp * (r - fRadius));
			u *= fExpand;
			v *= fExpand;
		}
#endif		
		pVertex->vBase.x = u + vP.x; // center on desired point
		pVertex->vBase.y = 0.f;
		pVertex->vBase.z = v + vP.z;
	}
	m_grid.m_pVB->Unlock();
	
	return ResampleGrid();
}

//-----------------------------------------------------------------------------
// Name: ResetGrid
// Desc: Drop all refinements and use just the base height field
//-----------------------------------------------------------------------------
HRESULT Terrain::ResetGrid()
{
	HRESULT hr;
	
	// Reset sampling grid to uniform sampling.
	TERRAINVERTEX *pVertices;
	hr = m_grid.m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;
	float fWidthScale = 1.f / (m_grid.m_width - 1);
	float fHeightScale = 1.f / (m_grid.m_height - 1);
	for (UINT iY = 0; iY < m_grid.m_height; iY++)
	for (UINT iX = 0; iX < m_grid.m_width; iX++)
	{
		TERRAINVERTEX *pVertex = &pVertices[iY * m_grid.m_width + iX];
		float u = iX * fWidthScale;
		float v = iY * fHeightScale;
		pVertex->vBase.x = u;
		pVertex->vBase.y = 0.f;
		pVertex->vBase.z = v;
	}
	m_grid.m_pVB->Unlock();
	
	return ResampleGrid();
}

//-----------------------------------------------------------------------------
// Name: DrawTerrain()
// Desc: Draws the scene
//
HRESULT Terrain::DrawTerrain()
{
	if (m_grid.m_pVB == NULL)
		return S_FALSE;

	// Set vertex shader
	g_pd3dDevice->SetVertexShader( D3DFVF_XYZ );

	// Set render state
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE/*D3DCULL_CW */ );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	// Set texture state
	UINT xx = 0;
	g_pd3dDevice->SetTexture( xx, m_pTexture );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_OBJECT);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	D3DXMATRIX matXZScale(m_vTextureScale.x,  0.f,                0.f, 0.f,
                          0.f,                0.f,                1.f, 0.f,
                          0.f,                m_vTextureScale.y,  0.f, 0.f,
                          m_vTextureOffset.x, m_vTextureOffset.y, 0.f, 1.f);
    g_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matXZScale );	// map (X,Z) to (U,V)
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG1, D3DTA_TEXTURE );
#if 0
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
#else		
	g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffaaaaaa);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG2, D3DTA_TFACTOR );
#endif		
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MAXANISOTROPY, 3 ); // make the terrain texture a little sharper as it recedes
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
#if 1
	// Use a second texture for tree shadows
	xx = 1;
	g_pd3dDevice->SetTexture( xx, m_pTexture2 );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_TEXCOORDINDEX, xx | D3DTSS_TCI_OBJECT);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	D3DXMATRIX matXZ(1.f, 0.f, 0.f, 0.f,
                     0.f, 0.f, 1.f, 0.f,
                     0.f, 1.f, 0.f, 0.f,
                     0.f, 0.f, 0.f, 1.f);
    g_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matXZ );	// map (X,Z) to (U,V)
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG2, D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MAXANISOTROPY, 3 ); // make the terrain texture a little sharper as it recedes
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	for (xx = 2; xx < 4; xx++)
#else		
	for (xx = 1; xx < 4; xx++)
#endif		
	{
		g_pd3dDevice->SetTexture( xx, NULL );
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_TEXCOORDINDEX, xx);
	}

	// Set world matrix
	D3DXVECTOR3 vSize = m_vMax - m_vMin;
    D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, vSize.x, vSize.y, vSize.z);
	D3DXMATRIX matOffset;
	D3DXMatrixTranslation(&matOffset, m_vMin.x, m_vMin.y, m_vMin.z);
    D3DXMATRIX matWorld;
    g_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
	D3DXMATRIX matWorldNew = matScale * matOffset * matWorld;
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldNew );

	// Draw the terrain
	UINT width = m_grid.m_width;
	UINT height = m_grid.m_height;
	g_pd3dDevice->SetIndices( m_grid.m_pIB, 0 );
	g_pd3dDevice->SetStreamSource( 0, m_grid.m_pVB, sizeof(TERRAINVERTEX));
	g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, width*height, 0, (width-1)*(height-1)*2 );

	// Cleanup
	g_pd3dDevice->SetTexture( 0, NULL);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXANISOTROPY, 1 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTexture( 1, NULL);
 	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAXANISOTROPY, 1 );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );	// restore world matrix
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Get interpolated point from within a terrain cell, based
// on which triangle the point falls within.
//
//         X -->
//       0       1
//     0 A-------B                           A-------B
//   Z   |\      |  a = 1-z                  |\    * |  a = 1-x
//   |   |  \    |  b = 0                    |  \    |  b = x-z
//   |   |    \  |  c = z-x                  |    \  |  c = 0
//   v   | *    \|  d = x                    |      \|  d = z
//     1 C-------D                           C-------D
//
inline void CellPoint(D3DXVECTOR3 *pv,
					  float x, float z,
					  const D3DXVECTOR3 *rvCorner)
{
	float a, b, c, d;
	if (x > z)
	{
		a = 1 - x;
		b = x - z;
		// c = 0
		d = z;
		*pv = a * rvCorner[0]
			+ b * rvCorner[1]
		//	+ c * rvCorner[2]
			+ d * rvCorner[3];
	}
	else
	{
		a = 1 - z;
		// b = 0
		c = z - x;
		d = x;
		*pv = a * rvCorner[0]
		//	+ b * rvCorner[1]
			+ c * rvCorner[2]
			+ d * rvCorner[3];
	}
}

//////////////////////////////////////////////////////////////////////
// Works like CellPoint but uses a float array instead of a point array.
//
inline void CellHeight(float *pfHeight,
					   float x, float z,
					   const float *rfCornerHeight)
{
	float a, b, c, d;
	if (x > z)
	{
		a = 1 - x;
		b = x - z;
		// c = 0
		d = z;
		*pfHeight = a * rfCornerHeight[0]
			+ b * rfCornerHeight[1]
		//	+ c * rfCornerHeight[2]
			+ d * rfCornerHeight[3];
	}
	else
	{
		a = 1 - z;
		// b = 0
		c = z - x;
		d = x;
		*pfHeight = a * rfCornerHeight[0]
		//	+ b * rfCornerHeight[1]
			+ c * rfCornerHeight[2]
			+ d * rfCornerHeight[3];
	}
}

//-----------------------------------------------------------------------------
// Name: GetTerrainPoint
// Desc: Returns height and normal of point directly below vPosition.
//-----------------------------------------------------------------------------
HRESULT Terrain::GetTerrainPoint(const D3DXVECTOR3 &vPosition,
								 float *pfTerrainHeight,
								 D3DXVECTOR3 *pvTerrainNormal)
{
	// TODO: return normal, too
	*pvTerrainNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
	*pfTerrainHeight = 0.f; // default height

	// Map position to normalized coords
	// height field is indexed by (X,Z), with height stored in Y	
	D3DXVECTOR3 vSize = m_vMax - m_vMin;
	D3DXVECTOR3 vP;	
	vP.x = (vPosition.x - m_vMin.x) / vSize.x;
	// vP.y = (vPosition.x - m_vMin.y) / vSize.y; // Y coord is not needed
	vP.z = (vPosition.z - m_vMin.z) / vSize.z;

	long width = m_grid.m_width;
	long height = m_grid.m_height;
	
	// Use normalized position to choose terrain cell
	long XCenter, ZCenter;	// indices of upper-left corner of terrain cell
	float fX = vP.x * (width - 1);
	XCenter = (long)fX;
	fX -= (float)XCenter;	// X fraction
	if (XCenter < 0) XCenter = 0;
	if (XCenter > width - 2) XCenter = width - 2;
	float fZ = vP.z * (height - 1);
	ZCenter = (long)fZ;
	fZ -= (float)ZCenter;	// Z fraction
	if (ZCenter < 0) ZCenter = 0;
	if (ZCenter > height - 2) ZCenter = height - 2;
	
	// get indices into vertex array
	long lVertex0 = ZCenter * width + XCenter;
	long lVertex1 = lVertex0 + 1;
	long lVertex2 = lVertex0 + width;
	long lVertex3 = lVertex2 + 1;
	assert(lVertex0 >= 0 && lVertex0 <= 0xffff);
	assert(lVertex1 >= 0 && lVertex1 <= 0xffff);
	assert(lVertex2 >= 0 && lVertex2 <= 0xffff);
	assert(lVertex3 >= 0 && lVertex3 <= 0xffff);
	WORD iVertex0 = (WORD)lVertex0;
	WORD iVertex1 = (WORD)lVertex1;
	WORD iVertex2 = (WORD)lVertex2;
	WORD iVertex3 = (WORD)lVertex3;

	// Gather the corner heights
	LPDIRECT3DVERTEXBUFFER8 pVB = m_grid.m_pVB;
	if (pVB == NULL) return S_FALSE;
	TERRAINVERTEX *pVertices;
	if( FAILED( pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return E_FAIL;
	float rfCornerHeight[4];
	rfCornerHeight[0] = pVertices[iVertex0].vBase.y;
	rfCornerHeight[1] = pVertices[iVertex1].vBase.y;
	rfCornerHeight[2] = pVertices[iVertex2].vBase.y;
	rfCornerHeight[3] = pVertices[iVertex3].vBase.y;
	pVB->Unlock();

	// Get interpolated height
	CellHeight(pfTerrainHeight, fX, fZ, rfCornerHeight);

	// Scale and offset to output coords
	*pfTerrainHeight *= vSize.y;
	*pfTerrainHeight += m_vMin.y;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Clean up after ourselves.
//-----------------------------------------------------------------------------
HRESULT Terrain::Cleanup()
{
	SAFE_RELEASE(m_pTexture2);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pTerrain);
	m_grid.Cleanup();
    return S_OK;
}

// Returns size in bytes of pixel of given format, or -1 if size can
// not be determined from the format alone (e.g., D3DFMT_VERTEXDATA).
UINT GetPixelSize(D3DFORMAT format)
{
    switch (format)
    {
		case D3DFMT_A8R8G8B8:   return 4;
		case D3DFMT_X8R8G8B8:   return 4;
		case D3DFMT_A8:         return 1;
		case D3DFMT_LIN_A8R8G8B8:   return 4;
		case D3DFMT_LIN_X8R8G8B8:   return 4;
		case D3DFMT_LIN_A8:         return 1;
		default:
			return -1;
    }
}

// Unpacks color into pColor and returns pointer to pColor,
// or returns NULL if unpacking is not possible (or unimplemented.)
D3DXCOLOR *GetPixelColor(D3DXCOLOR *pColor, BYTE *pPixel, D3DFORMAT format)
{
    switch (format)
    {
		case D3DFMT_A8R8G8B8:
		case D3DFMT_LIN_A8R8G8B8:
		{
			float f = 1.f / 255.f;
			pColor->b = f * pPixel[0];
			pColor->g = f * pPixel[1];
			pColor->r = f * pPixel[2];
			pColor->a = f * pPixel[3];
			return pColor;
		}
		
		case D3DFMT_X8R8G8B8:
		case D3DFMT_LIN_X8R8G8B8:
		{
			float f = 1.f / 255.f;
			pColor->b = f * pPixel[0];
			pColor->g = f * pPixel[1];
			pColor->r = f * pPixel[2];
			pColor->a = 1.f;
			return pColor;
		}
		
		case D3DFMT_A8:
		case D3DFMT_LIN_A8:
		{
			float f = 1.f / 255.f;
			pColor->b = 0.f;
			pColor->g = 0.f;
			pColor->r = 0.f;
			pColor->a = f * pPixel[0];
			return pColor;
		}
		default:
			return NULL;
    }
}

//-----------------------------------------------------------------------------
// Name: CopyRectsVB()
// Desc: Like IDirect3DDevice8's CopyRects, but copies pixel data
// to vertex buffers
//-----------------------------------------------------------------------------
HRESULT CopyRectsVB(
	IDirect3DSurface8* pSourceSurface,
	CONST RECT* pSourceRectsArray,
	UINT cRects,
	void *pvDestinationVertices,
	// we'd like to use IDirect3DVertexBuffer8* pDestinationVB, but
	// multiple locks on the destination with DISCARD_CONTENTS doesn't work
	UINT cbDestinationStride,
	UINT cbDestinationPitch,
	CONST POINT* pDestinationPointsArray)
{
	HRESULT hr;
	D3DSURFACE_DESC desc;
	hr = pSourceSurface->GetDesc(&desc);
	if (FAILED(hr))
		return hr;
	D3DFORMAT pixelFormat = desc.Format;
	DWORD dwPixelStride = GetPixelSize(pixelFormat);
	if (dwPixelStride == -1)
		return E_NOTIMPL;	// we can't handle the format
	RECT rectFull;
	rectFull.left = 0;
	rectFull.top = 0;
	rectFull.right = desc.Width;
	rectFull.bottom = desc.Height;
	if (pSourceRectsArray == NULL)
	{
		if (cRects != 1
			&& cRects != 0)
			return E_INVALIDARG;
		pSourceRectsArray = &rectFull;
		cRects = 1;
	}
	for (UINT iRect = 0; iRect < cRects; iRect++)
	{
		// validate size of source rect
		const RECT *pRect = &pSourceRectsArray[iRect];
		if (pRect->top < rectFull.top
			|| pRect->left < rectFull.left
			|| pRect->bottom > rectFull.bottom
			|| pRect->right > rectFull.right)
			return E_INVALIDARG;
		// Get offset to upper left corner of destination
		POINT pointDestinationOffset;
		if (pDestinationPointsArray != NULL)
		{
			pointDestinationOffset.x = pDestinationPointsArray[iRect].x - pRect->left;
			pointDestinationOffset.y = pDestinationPointsArray[iRect].y - pRect->top;
		}
		else
		{
			pointDestinationOffset.x = 0;
			pointDestinationOffset.y = 0;
		}
		// WARNING: No validation is done on the destination rect
		D3DLOCKED_RECT lockedRect;
		hr = pSourceSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
		if (FAILED(hr))
			return hr;
		if (XGIsSwizzledFormat(pixelFormat))
		{
			Swizzler s(desc.Width, desc.Height, 0);
			// Expand and copy the colors
			for (LONG iY = pRect->top; iY < pRect->bottom; iY++)
			{
				BYTE *pDestinationRow = (BYTE *)pvDestinationVertices + (iY + pointDestinationOffset.y) * cbDestinationPitch;
				s.SetV(s.SwizzleV(iY));
				s.SetU(s.SwizzleU(pRect->left));
				for (LONG iX = pRect->left; iX < pRect->right; iX++)
				{
					// Unpack and then pack color
					D3DXCOLOR pixelColor;
					BYTE *pPixel = (BYTE *)lockedRect.pBits + dwPixelStride * s.Get2D();
					if (GetPixelColor(&pixelColor, pPixel, pixelFormat) == NULL)
					{
						pSourceSurface->UnlockRect();
						return E_NOTIMPL;
					}
					D3DXVECTOR3 *pxyz = (D3DXVECTOR3 *)(pDestinationRow + (iX + pointDestinationOffset.x) * cbDestinationStride);
					pxyz->x = pixelColor.r;
					pxyz->y = pixelColor.g; 	// height
					pxyz->z = pixelColor.b;
					s.IncU();
				}
			}
		}
		else
		{
			// Expand and copy the colors
			for (LONG iY = pRect->top; iY < pRect->bottom; iY++)
			{
				BYTE *pSourceRow = (BYTE *)lockedRect.pBits + lockedRect.Pitch * (iY - pRect->top);
				BYTE *pDestinationRow = (BYTE *)pvDestinationVertices + (iY + pointDestinationOffset.y) * cbDestinationPitch;
				for (LONG iX = pRect->left; iX < pRect->right; iX++)
				{
					// Unpack color and copy to xyz
					D3DXCOLOR pixelColor;
					BYTE *pPixel = pSourceRow + (iX - pRect->left) * dwPixelStride;
					if (GetPixelColor(&pixelColor, pPixel, pixelFormat) == NULL)
					{
						pSourceSurface->UnlockRect();
						return E_NOTIMPL;
					}
					D3DXVECTOR3 *pxyz = (D3DXVECTOR3 *)(pDestinationRow + (iX + pointDestinationOffset.x) * cbDestinationStride);
					pxyz->x = pixelColor.r;
					pxyz->y = pixelColor.g;		// height
					pxyz->z = pixelColor.b;
				}
			}
		}
		pSourceSurface->UnlockRect();
		if (FAILED(hr))
			return hr;
	}
	return S_OK;
}

