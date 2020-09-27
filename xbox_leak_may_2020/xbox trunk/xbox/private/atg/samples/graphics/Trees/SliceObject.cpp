//-----------------------------------------------------------------------------
//  
//  File: SliceObject.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
// 	CSliceObject represents a geometric object as textured slices that
// 	move relative to one another for parallax.  This is a good level-
// 	of-detail representation for complex objects such as trees.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <stdlib.h>
#include "SliceObject.h"
#include <XBUtil.h>
#include <XPath.h>

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//////////////////////////////////////////////////////////////////////
// Slice() creates the slice textures based on the SliceInfo values,
// and then uses the SliceTexture::GenerateSlices() routine with a
// callback to draw the geometry into each slice texture.
//
HRESULT CSliceObject::Slice(UINT nDirection, SliceInfo *rSliceInfo)	// slice in each of the directions
{
	HRESULT hr;
	if (nDirection != m_nDirection
		|| m_rSliceTexture == NULL)
	{
		SAFE_DELETE_ARRAY(m_rSliceInfo);
		SAFE_DELETE_ARRAY(m_rSliceTexture);
		SAFE_DELETE_ARRAY(m_rfFade);
		m_nDirection = nDirection;
		if (m_nDirection == 0)
			return S_FALSE;	// nothing to do
		m_rSliceInfo = new SliceInfo [ m_nDirection ];
		if (m_rSliceInfo == NULL)
			return E_OUTOFMEMORY;
		m_rSliceTexture = new CSliceTexture [ m_nDirection ];
		if (m_rSliceTexture == NULL)
			return E_OUTOFMEMORY;
		m_rfFade = new float [ m_nDirection ];
		if (m_rfFade == NULL)
			return E_OUTOFMEMORY;
	}
	for (m_iDirection = 0; m_iDirection < m_nDirection; m_iDirection++)
	{
		m_rSliceInfo[m_iDirection] = rSliceInfo[m_iDirection];
		DWORD nSlice = rSliceInfo[m_iDirection].nSlice;
		DWORD Width = rSliceInfo[m_iDirection].Width;
		DWORD Height = rSliceInfo[m_iDirection].Height;
		D3DTEXTUREADDRESS WrapU = rSliceInfo[m_iDirection].WrapU;
		D3DTEXTUREADDRESS WrapV = rSliceInfo[m_iDirection].WrapV;
		D3DFORMAT Format =  rSliceInfo[m_iDirection].Format;
		
		OUTPUT_DEBUG_STRING( " Slice..." );
		hr = m_rSliceTexture[m_iDirection].GenerateSlices(Width, Height, nSlice, this, WrapU, WrapV);
		if (FAILED(hr))
			return hr;
		OUTPUT_DEBUG_STRING( " LOD..." );
		hr = m_rSliceTexture[m_iDirection].ComputeLevelOfDetailTextures();
		if (FAILED(hr))
			return hr;
		extern bool g_bCompressTextures;
		if (g_bCompressTextures
			&& Format != D3DFMT_A8R8G8B8)	// default format of slice textures
		{
			// TODO: migrate texture format setting down into the GenerateSlices routine
			// so that the overhead of allocating all the uncompressed textures would
			// not be so large.
			OUTPUT_DEBUG_STRING( " Compress..." );
			hr = m_rSliceTexture[m_iDirection].SetTextureFormat(Format);
			if (FAILED(hr))
				return hr;
		}
		m_rfFade[m_iDirection] = 1.f;	// default fade value
		hr = m_rSliceTexture[m_iDirection].SetLevelOfDetail(0.f);
		if (FAILED(hr))
			return hr;
		OUTPUT_DEBUG_STRING( " done.\n" );
	}
	return S_OK;
}

HRESULT CSliceObject::Cleanup()
{
	SAFE_DELETE_ARRAY(m_rSliceInfo);
	SAFE_DELETE_ARRAY(m_rSliceTexture);
	SAFE_DELETE_ARRAY(m_rfFade);
	m_iDirection = m_nDirection = 0;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Set up state for rendering object multiple times into a slice
// texture.  If bWorldCoords is false, then the world matrix is
// set to map the extents to a -1,1 cube.
//
HRESULT CSliceObject::Begin(bool bWorldCoords)
{
	if (!bWorldCoords)
	{
		// set world transformation to scale model to unit cube [-1,1] in all dimensions
		D3DXVECTOR3 va = m_vMin, vb = m_vMax;
		D3DXVECTOR3 vwidth(vb-va), vcenter(0.5f*(vb+va)); // width and center of geometry
		D3DXMATRIX matWorld, matTranslate, matScale;
		D3DXMatrixTranslation(&matTranslate, -vcenter.x, -vcenter.y, -vcenter.z);
		D3DXMatrixScaling(&matScale, 2.f/vwidth.x, 2.f/vwidth.y, 2.f/vwidth.z);
		matWorld = matTranslate * matScale;
		
		// rotate to align Y-axis with slice direction
		D3DXVECTOR3 XAxis = m_rSliceInfo[m_iDirection].vXAxis;
		D3DXVECTOR3 YAxis = m_rSliceInfo[m_iDirection].vDirection;
		D3DXVECTOR3 ZAxis;
		D3DXVec3Cross(&ZAxis, &XAxis, &YAxis);
		D3DMATRIX matAxis = {
			XAxis.x, XAxis.y, XAxis.z, 0.f,
			YAxis.x, YAxis.y, YAxis.z, 0.f,
			ZAxis.x, ZAxis.y, ZAxis.z, 0.f,
			0.f,     0.f,     0.f,     1.f
		};
		matWorld *= matAxis;
		g_pd3dDevice->MultiplyTransform( D3DTS_WORLD, &matWorld);
	}
	return S_OK;
}

HRESULT CSliceObject::End()
{
	// does nothing
	return S_OK;
}

// set level-of-detail for slice textures
HRESULT CSliceObject::SetLevelOfDetail(const float *rfLevelOfDetail)
{
	for (UINT iDirection = 0; iDirection < m_nDirection; iDirection++)
		m_rSliceTexture[iDirection].SetLevelOfDetail(rfLevelOfDetail[iDirection]);
	return S_OK;
}

// Get matrix that maps from [0,1] cube to m_vMin, m_vMax.
// Using Q = P . Matrix, (0,0,0,1) maps to m_vMin. (1,1,1,1) maps to m_vMax
HRESULT CSliceObject::GetCubeMapping(D3DXMATRIX *pMatrix)
{
	D3DXMATRIX matScale, matTranslate;
	D3DXVECTOR3 vWidth(m_vMax - m_vMin);
	D3DXMatrixScaling(&matScale, vWidth.x, vWidth.y, vWidth.z);
	D3DXMatrixTranslation(&matTranslate, m_vMin.x, m_vMin.y, m_vMin.z);
	*pMatrix = matScale * matTranslate;
	return S_OK;
}

extern bool g_bDebugSlice;
extern bool g_bDebugSliceOpaque;

//////////////////////////////////////////////////////////////////////
// Setup render state for drawing cube of slices
//
HRESULT CSliceObject::BeginDrawCubeSlices()
{
	g_pd3dDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX1);
	g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	static DWORD dwAlphaRef = 0;
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, dwAlphaRef );
	static BOOL bAlphaBlend = TRUE;
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, bAlphaBlend );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	g_pd3dDevice->LightEnable(0, FALSE);
	DWORD xx = 0;
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP, D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
	if (g_bDebugSlice && g_bDebugSliceOpaque)
	{
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG1, D3DTA_TFACTOR );
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	}
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Set fade values for each of the slice textures based on the local eye direction
//
HRESULT CSliceObject::SetCubeFade(const D3DXVECTOR3 &vFrom, DWORD dwFlags)
{
	for (UINT iDirection = 0; iDirection < m_nDirection; iDirection++)
		m_rfFade[iDirection] = 0.f;
	for (UINT iCorner = 0; iCorner < 1 /*5*/; iCorner++)
	{
		D3DXVECTOR3 vCenter;
		if (iCorner == 0) 
			vCenter = 0.5f * (m_vMin + m_vMax);
		/*
		else if (iCorner == 1)
			vCenter = 0.5f * (
			*/
		D3DXVECTOR3 vEye = vFrom - vCenter;
		D3DXVec3Normalize(&vEye, &vEye);
		for (iDirection = 0; iDirection < m_nDirection; iDirection++)
		{
			float fFade = fabsf(D3DXVec3Dot(&vEye, (D3DXVECTOR3 *)&m_rSliceInfo[iDirection].vDirection));
			float fZero = m_rSliceInfo[iDirection].fZero;
			float fOne = m_rSliceInfo[iDirection].fOne;
			if (fFade > fOne) fFade = 1.f;
			else if (fFade < fZero) fFade = 0.f;
			else fFade = (fFade - fZero) / (fOne - fZero);
			if (m_rfFade[iDirection] < fFade)
				m_rfFade[iDirection] = fFade;
		}
	}
	if (dwFlags & SLICEOBJECT_MAXFADE)
	{
		// find maximum fade and set it to one and the others to 0
		UINT iDirectionMax = 0;
		for (iDirection = 1; iDirection < m_nDirection; iDirection++)
			if (m_rfFade[iDirection] > m_rfFade[iDirectionMax])
				iDirectionMax = iDirection;
		for (iDirection = 0; iDirection < m_nDirection; iDirection++)
			m_rfFade[iDirection] = (iDirection == iDirectionMax) ? 1.f : 0.f;
	}
	else if (dwFlags & SLICEOBJECT_UNITFADE)
	{
		// make fade values sum to one
		float fSum = 0.f;
		for (iDirection = 0; iDirection < m_nDirection; iDirection++)
			fSum += m_rfFade[iDirection];
		float fScale = 1.f / fSum;
		for (iDirection = 0; iDirection < m_nDirection; iDirection++)
			m_rfFade[iDirection] *= fScale;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Draw slices from front to back based on local eye
//
HRESULT CSliceObject::DrawCubeSlices(const D3DXVECTOR3 &vFrom)
{
	D3DXVECTOR3 vCenter = 0.5f * (m_vMin + m_vMax);
	D3DXVECTOR3 vEye = vFrom - vCenter;
	struct quad_vertex {
		D3DXVECTOR3 pos;
		D3DXVECTOR2 tex;
	} aVertex[4];
	const float fZeroBlend = 0.5f/255.f;	// blend values below this quantize to zero
	for (UINT iDirection = 0; iDirection < m_nDirection; iDirection++)
	{
		float fFade = m_rfFade[iDirection];
		if (fFade < fZeroBlend)	// quantization threshold
			continue; // skip to next direction 

		D3DXVECTOR3 vSweep(0.f, 0.f, 0.f);
		bool bReverse = false;
		switch (iDirection)
		{
		case 0: // sweep along x
			if (vEye.x < 0.f)
			{
				vSweep.x = m_vMin.x - m_vMax.x;
				aVertex[0].pos.x = aVertex[1].pos.x = aVertex[2].pos.x = aVertex[3].pos.x = m_vMax.x;
				bReverse = true;
			}
			else
			{
				vSweep.x = m_vMax.x - m_vMin.x;
				aVertex[0].pos.x = aVertex[1].pos.x = aVertex[2].pos.x = aVertex[3].pos.x = m_vMin.x;
			}
			
			aVertex[0].pos.y = m_vMin.y;    aVertex[0].pos.z = m_vMin.z;
			aVertex[1].pos.y = m_vMax.y;    aVertex[1].pos.z = m_vMin.z;
			aVertex[2].pos.y = m_vMin.y;    aVertex[2].pos.z = m_vMax.z;
			aVertex[3].pos.y = m_vMax.y;    aVertex[3].pos.z = m_vMax.z;
			aVertex[0].tex.x = 0.f;    aVertex[0].tex.y = 0.f;
			aVertex[1].tex.x = 1.f;    aVertex[1].tex.y = 0.f;
			aVertex[2].tex.x = 0.f;    aVertex[2].tex.y = 1.f;
			aVertex[3].tex.x = 1.f;    aVertex[3].tex.y = 1.f; 
			break;
		case 1: // sweep along y
			if (vEye.y < 0.f)
			{
				vSweep.y = m_vMin.y - m_vMax.y;
				aVertex[0].pos.y = aVertex[1].pos.y = aVertex[2].pos.y = aVertex[3].pos.y = m_vMax.y;
				bReverse = true;
			}
			else
			{
				vSweep.y = m_vMax.y - m_vMin.y;
				aVertex[0].pos.y = aVertex[1].pos.y = aVertex[2].pos.y = aVertex[3].pos.y = m_vMin.y;
			}
			aVertex[0].pos.x = m_vMin.x;	aVertex[0].pos.z = m_vMin.z;
			aVertex[1].pos.x = m_vMax.x;	aVertex[1].pos.z = m_vMin.z;
			aVertex[2].pos.x = m_vMin.x;	aVertex[2].pos.z = m_vMax.z;
			aVertex[3].pos.x = m_vMax.x;	aVertex[3].pos.z = m_vMax.z;
			aVertex[0].tex.x = 0.f;    aVertex[0].tex.y = 1.f;
			aVertex[1].tex.x = 1.f;    aVertex[1].tex.y = 1.f;
			aVertex[2].tex.x = 0.f;    aVertex[2].tex.y = 0.f;
			aVertex[3].tex.x = 1.f;    aVertex[3].tex.y = 0.f; 
			break;
		case 2: // sweep along z
			if (vEye.z < 0.f)
			{
				vSweep.z = m_vMin.z - m_vMax.z;
				aVertex[0].pos.z = aVertex[1].pos.z = aVertex[2].pos.z = aVertex[3].pos.z = m_vMax.z;
				bReverse = true;
			}
			else
			{
				vSweep.z = m_vMax.z - m_vMin.z;
				aVertex[0].pos.z = aVertex[1].pos.z = aVertex[2].pos.z = aVertex[3].pos.z = m_vMin.z;
			}
			aVertex[0].pos.x = m_vMin.x;	aVertex[0].pos.y = m_vMin.y;
			aVertex[1].pos.x = m_vMax.x;	aVertex[1].pos.y = m_vMin.y;
			aVertex[2].pos.x = m_vMin.x;	aVertex[2].pos.y = m_vMax.y;
			aVertex[3].pos.x = m_vMax.x;	aVertex[3].pos.y = m_vMax.y;
			aVertex[0].tex.x = 1.f;    aVertex[0].tex.y = 1.f;
			aVertex[1].tex.x = 0.f;    aVertex[1].tex.y = 1.f;
			aVertex[2].tex.x = 1.f;    aVertex[2].tex.y = 0.f;
			aVertex[3].tex.x = 0.f;    aVertex[3].tex.y = 0.f; 
			break;
		default:
			return E_NOTIMPL;
		}

		D3DXCOLOR colorFade(fFade, fFade, fFade, fFade);
		g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, colorFade );
		D3DTEXTUREADDRESS WrapU = m_rSliceInfo[iDirection].WrapU;
		D3DTEXTUREADDRESS WrapV = m_rSliceInfo[iDirection].WrapV;
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, WrapU );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, WrapV );
		CSliceTexture *pSliceTexture = &m_rSliceTexture[iDirection];
		UINT dwNumSlicesLOD = pSliceTexture->m_dwNumSlicesLOD;
		vSweep /= (float)dwNumSlicesLOD;
		static float fHalf = 0.5f;
		for (UINT i = 0; i < 4; i++)	// move to first plane
			aVertex[i].pos += fHalf * vSweep;
		for (UINT iSlice = 0; iSlice < dwNumSlicesLOD; iSlice++)
		{
			int iSliceFlip;
			if (bReverse)
				iSliceFlip = dwNumSlicesLOD - 1 - iSlice;
			else
				iSliceFlip = iSlice;
			g_pd3dDevice->SetTexture( 0, pSliceTexture->m_rpTextureLOD[iSliceFlip] );
			if (g_bDebugSlice)
			{
				colorFade = D3DXCOLOR(fFade,
									  fFade *(float)iSlice/(float)dwNumSlicesLOD,
									  fFade * (m_nDirection == 1 ? 1.f : (float)iDirection/(float)(m_nDirection - 1)), 
									  fFade);
				static D3DCOLOR colorMask = 0xffffffff;
				D3DCOLOR color = (D3DCOLOR)colorFade & colorMask;
				g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, color );
				extern DWORD g_dwTotalSliceCount;
				g_dwTotalSliceCount++;
			}
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aVertex, sizeof(quad_vertex));	
			for (UINT i = 0; i < 4; i++)	// move to next plane
				aVertex[i].pos += vSweep;
		}
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Cleanup after drawing cube slices
//
HRESULT CSliceObject::EndDrawCubeSlices()
{
	// does nothing
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Draw slices from front to back based on local eye
//
HRESULT CSliceObject::DrawSlices(const D3DXVECTOR3 &vFromFade, const D3DXVECTOR3 &vFromSliceOrder, DWORD dwFlags)
{
	BeginDrawCubeSlices();
	SetCubeFade(vFromFade, dwFlags);
	DrawCubeSlices(vFromSliceOrder);
	EndDrawCubeSlices();
	return S_OK;
}

