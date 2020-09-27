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
#include "Terrain.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

UINT GetPixelSize(D3DFORMAT format);
D3DXCOLOR *GetPixelColor(D3DXCOLOR *pColor, BYTE *pPixel, D3DFORMAT format);
HRESULT CopyRectsVB(IDirect3DSurface8* pSourceSurface, CONST RECT* pSourceRectsArray,
					UINT cRects, void *pvDestinationVertices, UINT cbDestinationStride,
					UINT cbDestinationPitch, CONST POINT* pDestinationPointsArray);

//-----------------------------------------------------------------------------
// Name: Grid::Initialize
// Desc: Allocate grid vertex buffer and allocate and fill index buffer.
//-----------------------------------------------------------------------------
HRESULT	Grid::Initialize(UINT width, UINT height, UINT cbVertexSize)
{
	HRESULT hr;

	m_width = width;
	m_height = height;
	
	// Create the vertex buffer
	SAFE_RELEASE(m_pVB);
	hr = g_pd3dDevice->CreateVertexBuffer(m_width * m_height * cbVertexSize, 0, 0, D3DPOOL_DEFAULT, &m_pVB);

	// Create the index buffer
	hr = g_pd3dDevice->CreateIndexBuffer(3*2*(m_width-1)*(m_height-1)*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB);
	if (FAILED(hr))
        return hr;

	// Fill the tile index buffer.
	WORD *pIndices;
	if( FAILED( m_pIB->Lock( 0, 0, (BYTE**)&pIndices, 0 ) ) )
		return E_FAIL;
	{
		for (UINT iY=0; iY < m_height-1; iY++ )
		{
			for (UINT iX=0; iX < m_width-1; iX++ )
			{
				WORD iVertex = iY*m_width + iX;
				WORD iiTri = 3*2*(iY*(m_width - 1) + iX);
				if ((iX ^ iY) & 1)
				{
					// flip alternating tiles to get "fin" coverage along both 45-degree lines
					// tri 0
					pIndices[iiTri] = iVertex;
					pIndices[iiTri+1] = iVertex+m_width;
					pIndices[iiTri+2] = iVertex+m_width+1;
					// tri 1
					pIndices[iiTri+3] = iVertex;
					pIndices[iiTri+4] = iVertex+m_width+1;
					pIndices[iiTri+5] = iVertex+1;
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
			}
		}
	}
	m_pIB->Unlock();

	return hr;
}

//-----------------------------------------------------------------------------
// Name: Grid::Cleanup
// Desc: Clean up buffers.
//-----------------------------------------------------------------------------
HRESULT	Grid::Cleanup()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Terrain constructor
// Desc: Set defaults.
//-----------------------------------------------------------------------------
Terrain::Terrain()
{
	m_pTerrain = NULL;			// terrain height field
	m_pTexture = NULL;			// terrain texture
	m_dwTerrainVertexShader = 0; // terrain vertex shader handle
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Create the height field vertex buffers from m_pTerrain
//       height field texture
//-----------------------------------------------------------------------------
HRESULT Terrain::Initialize(LPDIRECT3DTEXTURE8 pTerrain, LPDIRECT3DTEXTURE8 pTexture, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, float fGrassHeight)
{
	HRESULT hr;
	m_vMin = vMin;
	m_vMax = vMax;
	m_fGrassHeight = fGrassHeight;
	
	LPDIRECT3DTEXTURE8 pTerrainOld = m_pTerrain;
	m_pTerrain = pTerrain;
	pTerrain->AddRef();
	SAFE_RELEASE(pTerrainOld);
	
	LPDIRECT3DTEXTURE8 pTextureOld = m_pTexture;
	m_pTexture = pTexture;
	pTexture->AddRef();
	SAFE_RELEASE(pTextureOld);

	UINT width = 32;
	UINT height = 32;

	// Allocate terrain grid
	hr = m_grid.Initialize(width, height, sizeof(TERRAINVERTEX));
	if (FAILED(hr))
		return hr;

	// Fill terrain VB with height field data
	hr = Reset();
	if (FAILED(hr))
		return hr;
	
	// load the terrain vertex shader
	DWORD vsdecl[] = 
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),		// u, v, height
		D3DVSD_REG(1, D3DVSDT_FLOAT3),		// tip u, v, height
		D3DVSD_END()
	};
	XBUtil_CreateVertexShader(g_pd3dDevice, _FNA("Shaders\\terrain_xyz.xvu"), vsdecl, &m_dwTerrainVertexShader);
	if(!m_dwTerrainVertexShader)
	{
		OUTPUT_DEBUG_STRING( "Initialize : error loading terrain_xyz.xvu\n" );
		return XBAPPERR_MEDIANOTFOUND;
	}
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
// Name: InDomain
// Desc: Test point against 2D bounding box. Z is ignored.
//-----------------------------------------------------------------------------
inline bool InDomain(const D3DXVECTOR3 &vPosition, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax)
{
	return (vPosition.x >= vMin.x
			&& vPosition.y >= vMin.y
// ignore z range
//			&& vPosition.z >= vMin.z
			&& vPosition.x < vMax.x
			&& vPosition.y < vMax.y
//			&& vPosition.z < vMax.z
			);
}

//-----------------------------------------------------------------------------
// Name: GetTerrainPoint
// Desc: Returns height and normal of point directly below vPosition.
//-----------------------------------------------------------------------------
HRESULT Terrain::GetTerrainPoint(const D3DXVECTOR3 &vPosition, float *pfTerrainHeight, D3DXVECTOR3 *pvTerrainNormal)
{
	HRESULT hr;

	// TODO: return normal, too
	*pvTerrainNormal = D3DXVECTOR3(0.f, 0.f, 1.f);
	*pfTerrainHeight = 0.f; // default height
//	if (!InDomain(vPosition, m_vMin, m_vMax))
//		return S_FALSE;

	// Map to normalized coords
	D3DXVECTOR3 vSize = m_vMax - m_vMin;
	D3DXVECTOR2 vP((vPosition.x - m_vMin.x) / vSize.x,
				   (vPosition.y - m_vMin.y) / vSize.y);

	// Lookup height-field neighbors
	D3DXVECTOR2 vFraction;
	TERRAINVERTEX rVertex[4];
	hr = GetHeightFieldNeighbors(vP, &vFraction, rVertex);
	if (FAILED(hr))
		return hr;
	
	// Get interpolated height value
	*pfTerrainHeight = (1-vFraction.x) * (1-vFraction.y) * rVertex[0].vBase.z;
	*pfTerrainHeight += vFraction.x * (1-vFraction.y) * rVertex[1].vBase.z;
	*pfTerrainHeight += (1-vFraction.x) * vFraction.y * rVertex[2].vBase.z;
	*pfTerrainHeight += vFraction.x * vFraction.y * rVertex[3].vBase.z;

	// Scale and offset to output coords
	*pfTerrainHeight *= vSize.z;
	*pfTerrainHeight += m_vMin.z;
	return S_OK;
}


// DEBUG helps
static UINT s_nPosition = 0;
static D3DXVECTOR3 s_rvPosition[1000];
static D3DXVECTOR3 s_rvPositionX[1000];
static D3DXVECTOR3 s_rvPositionY[1000];
static D3DXVECTOR3 s_rvPositionZ[1000];

//-----------------------------------------------------------------------------
// Name: Resample
// Desc: Sample the height field at the current uv positions
//-----------------------------------------------------------------------------
HRESULT Terrain::Resample()
{
	s_nPosition = 0;	// DEBUG

	HRESULT hr;
	// Loop through all the vertices in the mesh, getting height value from array
	// TODO: use mipmaps for coarser sampling densities
	TERRAINVERTEX *pVertices;
	hr = m_grid.m_pVB->Lock(0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;
	float fGrassHeightCanonical = m_fGrassHeight / (m_vMax.z - m_vMin.z);
	for (UINT iYGrid = 0; iYGrid < m_grid.m_height; iYGrid++)
	for (UINT iXGrid = 0; iXGrid < m_grid.m_width; iXGrid++)
	{
		TERRAINVERTEX *pVertex = &pVertices[iYGrid * m_grid.m_width + iXGrid];
		D3DXVECTOR2 vP(pVertex->vBase.x, pVertex->vBase.y);
		
		// lookup neighboring points from height-field texture
		D3DXVECTOR2 vFraction;
		TERRAINVERTEX rVertex[4];
		hr = GetHeightFieldNeighbors(vP, &vFraction, rVertex);
		if (FAILED(hr))
			goto e_Exit;
		
		// Get interpolated height value
		pVertex->vBase.z  = (1-vFraction.x) * (1-vFraction.y) * rVertex[0].vBase.z;
		pVertex->vBase.z +=   vFraction.x   * (1-vFraction.y) * rVertex[1].vBase.z;
		pVertex->vBase.z += (1-vFraction.x) *   vFraction.y   * rVertex[2].vBase.z;
		pVertex->vBase.z +=   vFraction.x   *   vFraction.y   * rVertex[3].vBase.z;

		// Set offset layer
		pVertex->vTip = pVertex->vBase;
		pVertex->vTip.z += fGrassHeightCanonical;
	}

 e_Exit:
	m_grid.m_pVB->Unlock();
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: BallUpdate
// Desc: Have motion of ball from A to B affect terrain.
//-----------------------------------------------------------------------------
HRESULT Terrain::BallUpdate(float fTimeStep, const D3DXVECTOR3 &vPositionA, const D3DXVECTOR3 &vPositionB, float fRadius) // ball moved from A to B
{
	// Make a pass over each vertex in mesh.  If the vertex is within fRadius, then offset tip
	HRESULT hr;
	D3DXVECTOR3 vSize(m_vMax.x - m_vMin.x, m_vMax.y - m_vMin.y, m_vMax.z - m_vMin.z);
	D3DXVECTOR3 vDelta = vPositionB - vPositionA;
	float fGrassHeightCanonical = m_fGrassHeight / (m_vMax.z - m_vMin.z);
	float fRadius2 = fRadius * fRadius;
	static float fRestoreRate = 1.f;
	float fRestore = expf(-fRestoreRate * fTimeStep);
	TERRAINVERTEX *pVertices;
	hr = m_grid.m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;
#if 1
	// Keep track of a few positions for debugging
	if (s_nPosition >= 1000)
		s_nPosition = 0;
	s_rvPosition[s_nPosition] =
	s_rvPositionX[s_nPosition] =
	s_rvPositionY[s_nPosition] =
	s_rvPositionZ[s_nPosition] = vPositionA;
	s_rvPositionX[s_nPosition].x += fRadius;
	s_rvPositionY[s_nPosition].y += fRadius;
	s_rvPositionZ[s_nPosition].z += fRadius;
	s_nPosition++;
#endif
	for (UINT iY = 0; iY < m_grid.m_height; iY++)
	for (UINT iX = 0; iX < m_grid.m_width; iX++)
	{
		TERRAINVERTEX *pVertex = &pVertices[iY * m_grid.m_width + iX];
		// Map to world coords
		D3DXVECTOR3 vTipWorld = pVertex->vTip;
		vTipWorld.x *= vSize.x;
		vTipWorld.y *= vSize.y;
		vTipWorld.z *= vSize.z;
		vTipWorld += m_vMin;
		D3DXVECTOR3 vDist = vPositionA - vTipWorld;	// assume that points are updated often so we don't have to test along line
		float fLenSq = D3DXVec2LengthSq((D3DXVECTOR2 *)&vDist);	// use 2D distance
		if (fLenSq < fRadius2 && vDist.z < fRadius)	// if ball is near or in grass
		{
			// TODO: add scaling, etc., for finer control
			vTipWorld += vDelta;

			// Scale tip vector to be hair length
			D3DXVECTOR3 vBaseWorld = pVertex->vBase;
			vBaseWorld.x *= vSize.x;
			vBaseWorld.y *= vSize.y;
			vBaseWorld.z *= vSize.z;
			vBaseWorld += m_vMin;
			D3DXVECTOR3 vGrass = vTipWorld - vBaseWorld;
			static float fFlattenedFraction = 0.3f;
			float fGrassFlattenedHeight = fFlattenedFraction * m_fGrassHeight;
			if (vGrass.z < fGrassFlattenedHeight) vGrass.z = fGrassFlattenedHeight;
			float fGrassLength = D3DXVec3Length(&vGrass);
		//	if (fGrassLength > m_fGrassHeight)
				vGrass *= m_fGrassHeight / fGrassLength;
			vTipWorld = vBaseWorld + vGrass;

			// Map back into canonical coords
			vTipWorld -= m_vMin;
			vTipWorld.x /= vSize.x;
			vTipWorld.y /= vSize.y;
			vTipWorld.z /= vSize.z;
			pVertex->vTip = vTipWorld;
		}
		else
		{
			// Slowly restore grass to it's default position
			D3DXVECTOR3 vTarget = pVertex->vBase;
			vTarget.z += fGrassHeightCanonical;
			pVertex->vTip *= fRestore;
			pVertex->vTip += (1 - fRestore) * vTarget;
		}
	}
	m_grid.m_pVB->Unlock();
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Refine
// Desc: Expand sampling grid around specified point to get more local samples
//-----------------------------------------------------------------------------
HRESULT Terrain::Refine(const D3DXVECTOR3 &vPosition, float fEpsilon)
{
	HRESULT hr;
	
	// Map to normalized coords
	D3DXVECTOR3 vSize = m_vMax - m_vMin;
	D3DXVECTOR3 vP = vPosition - m_vMin;
	vP.x /= vSize.x;
	vP.y /= vSize.y;

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
		pVertex->vBase.y = v + vP.y;
		pVertex->vBase.z = 0.f;
	}
	m_grid.m_pVB->Unlock();
	
	return Resample();
}

//-----------------------------------------------------------------------------
// Name: Reset
// Desc: Drop all refinements and use just the base height field
//-----------------------------------------------------------------------------
HRESULT Terrain::Reset()
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
		pVertex->vBase.y = v;
		pVertex->vBase.z = 0.f;
	}
	m_grid.m_pVB->Unlock();
	
	return Resample();
}

//-----------------------------------------------------------------------------
// Name: Begin()
// Desc: Set up drawing state
//-----------------------------------------------------------------------------
HRESULT Terrain::Begin()
{
	// Update level of detail
	// Cull active tiles that are outside the frustum
	// Calculate ordering of tiles
	UINT xx = 0;	// texture stage
	g_pd3dDevice->SetTexture( xx, m_pTexture );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_TEXCOORDINDEX, 0);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG1, D3DTA_TEXTURE );
#if 0		
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
#else		
	g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff003300 ); // just the green channel, darkened
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG2, D3DTA_TFACTOR );
#endif		
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE/*D3DCULL_CW */);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	D3DXVECTOR4 vShellOffset(0.f, 0.f, 0.f, 0.f); // base tile layer has 0 offset
	D3DXVECTOR4 vTextureScale(1.f, 1.f, 0.f, 0.f); // base tile texture scale is identity
	D3DXVECTOR4 vColor(0.1f, 0.1f, 0.1f, 1.f); // base tile color is dark
	g_pd3dDevice->SetVertexShaderConstant( 1, &vShellOffset, 1);
	g_pd3dDevice->SetVertexShaderConstant( 2, &vTextureScale, 1);
	g_pd3dDevice->SetVertexShaderConstant( 3, &vColor, 1);
	float fFraction = 0.f;
	D3DXVECTOR4 vLayerFraction(1.f, 0.f, 1.f - fFraction, fFraction);
	g_pd3dDevice->SetVertexShaderConstant( 8, &vLayerFraction, 1);
	float fExtraShadowOffset = 0.5f;
	D3DXVECTOR4 vExtraShadow(-(1.f - fExtraShadowOffset) / m_fGrassHeight, 1.f, m_fGrassHeight, 1.f);
	g_pd3dDevice->SetVertexShaderConstant( 9, &vExtraShadow, 1);
	g_pd3dDevice->SetIndices( m_grid.m_pIB, 0 );
	g_pd3dDevice->SetVertexShader( m_dwTerrainVertexShader );
	
	// Set vertex shader matrix
	D3DXVECTOR3 vSize = m_vMax - m_vMin;
    D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, vSize.x, vSize.y, vSize.z);
	D3DXMATRIX matOffset;
	D3DXMatrixTranslation(&matOffset, m_vMin.x, m_vMin.y, m_vMin.z);
    D3DXMATRIX matWorld;
    g_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
    D3DXMATRIX matView;
    g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
    D3DXMATRIX matProjection;
    g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjection );
	D3DXMATRIX matWorldViewProjection = matScale * matOffset * matWorld * matView * matProjection;
	D3DXMATRIX matTranspose;
	D3DXMatrixTranspose(&matTranspose, &matWorldViewProjection);
	g_pd3dDevice->SetVertexShaderConstant( 4, &matTranspose, 4 );
	D3DXVECTOR4 vFraction(1.0, 0.f, 1.f, 0.f);	// set tile fraction
	g_pd3dDevice->SetVertexShaderConstant( 0, &vFraction, 1);
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawTerrain()
// Desc: Draws the scene as tiles, seams, and corners
//-----------------------------------------------------------------------------
HRESULT Terrain::DrawTerrain()
{
	LPDIRECT3DVERTEXBUFFER8 pVB = m_grid.m_pVB;
	UINT width = m_grid.m_width;
	UINT height = m_grid.m_height;
	if (pVB == NULL) return S_FALSE;
	g_pd3dDevice->SetStreamSource( 0, pVB, sizeof(TERRAINVERTEX) );
	g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, width*height, 0, (width-1)*(height-1)*2 );
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawTerrain()
// Desc: Draws debugging display
//-----------------------------------------------------------------------------
HRESULT Terrain::DrawDebug()
{
	if (s_nPosition < 2) return S_OK;
	HRESULT hr = S_OK;
	g_pd3dDevice->SetVertexShader(D3DFVF_XYZ);
	g_pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DMATERIAL8 material;
	ZeroMemory(&material, sizeof(material));
	material.Emissive  = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	g_pd3dDevice->SetMaterial(&material);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, s_nPosition - 1, s_rvPosition, sizeof(D3DXVECTOR3));
	material.Emissive  = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	g_pd3dDevice->SetMaterial(&material);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, s_nPosition - 1, s_rvPositionX, sizeof(D3DXVECTOR3));
	material.Emissive  = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);
	g_pd3dDevice->SetMaterial(&material);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, s_nPosition - 1, s_rvPositionY, sizeof(D3DXVECTOR3));
	material.Emissive  = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
	g_pd3dDevice->SetMaterial(&material);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, s_nPosition - 1, s_rvPositionZ, sizeof(D3DXVECTOR3));
	return hr;
}

//-----------------------------------------------------------------------------
// Name: End()
// Desc: Clean up after drawing
//-----------------------------------------------------------------------------
HRESULT Terrain::End()
{
	// does nothing
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Clean up after ourselves.
//-----------------------------------------------------------------------------
HRESULT Terrain::Cleanup()
{
	if (m_dwTerrainVertexShader)
	{
		g_pd3dDevice->DeleteVertexShader(m_dwTerrainVertexShader);
		m_dwTerrainVertexShader = 0;
	}
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
					pxyz->y = pixelColor.b;
					pxyz->z = pixelColor.g;	// swap with b to put g as height
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
					pxyz->y = pixelColor.b;
					pxyz->z = pixelColor.g; // swap with b to put g as height
				}
			}
		}
		pSourceSurface->UnlockRect();
		if (FAILED(hr))
			return hr;
	}
	return S_OK;
}

