//-----------------------------------------------------------------------------
//  File: SliceTexture.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <stdlib.h>
#include "SliceTexture.h"
#include "SwizzleFormat.h"
#include "XBUtil.h"
#include "XPath.h"
#include "mipmap.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//////////////////////////////////////////////////////////////////////
// To see the slicing of the geometry and creation of the slice
// textures, set the following define to 1 and recompile.
// The slicing frustum is drawn "from the side" so the current
// clipping range can be seen.   Also, the texture is displayed
// at the end, after filtering down from the supersamples.

#define DEBUG_SHOW_SLICING 0

//-----------------------------------------------------------------------------
// Name: Constructor
// Desc: 
//-----------------------------------------------------------------------------
CSliceTexture::CSliceTexture()
{
	ZeroMemory(this, sizeof(CSliceTexture));
}

//-----------------------------------------------------------------------------
// Name: Destructor
// Desc: 
//-----------------------------------------------------------------------------
CSliceTexture::~CSliceTexture()
{
	DWORD i;
	for(i=0; i<SLICETEXTURE_MAXSLICE*2-1; i++)
		SAFE_RELEASE(m_rpTexture[i]);
}

//-----------------------------------------------------------------------------
// Name: GenerateSlices
// Desc: Call draw callback with z range set to take a series of slices
// and capturing the result in the slice textures.
//-----------------------------------------------------------------------------
HRESULT CSliceTexture::GenerateSlices(DWORD width,
									  DWORD height, 
									  DWORD nslices, 
									  CSliceTextureDrawCallback *pDrawCB, 				
									  D3DTEXTUREADDRESS WrapU, 
									  D3DTEXTUREADDRESS WrapV)
{
	HRESULT hr;
	assert(nslices <= SLICETEXTURE_MAXSLICE);
	if (WrapU != D3DTADDRESS_CLAMP && WrapU != D3DTADDRESS_WRAP) return E_NOTIMPL;
	if (WrapV != D3DTADDRESS_CLAMP && WrapV != D3DTADDRESS_WRAP) return E_NOTIMPL;
		
	// Check the format of the textures
	bool bFormatOK = true;
	if (m_dwNumSlices > 0)
	{
		D3DSURFACE_DESC desc;
		m_rpTexture[0]->GetLevelDesc(0, &desc);
		if (desc.Format != D3DFMT_A8R8G8B8)		// this is the only render target format we support
			bFormatOK = false;
	}

	// make sure volume info is up to date
	if ( m_Width != width
		 || m_Height != height
		 || !bFormatOK )
	{
		m_dwNumSlices = 0;
		m_dwNumSlicesLOD = 0;
		for(UINT i=0; i<SLICETEXTURE_MAXSLICE*2-1; i++)
			SAFE_RELEASE(m_rpTexture[i]);
	}
	m_Width = width;
	m_Height = height;

	// create textures if necessary
	if (m_dwNumSlices != nslices)
	{
		UINT i;
		// count number of level-of-detail textures needed
		UINT nLOD = 0;
		for (i = 1; (1u << i) <= nslices; i++)
			nLOD += nslices >> i;
		m_dwLODMax = i - 1;

		// create new textures and level-of-detail textures
		static DWORD nLevel = 0;
		for(i=0; i<nslices+nLOD; i++)
			if(!m_rpTexture[i])
			{
				hr = g_pd3dDevice->CreateTexture(width, height, nLevel, 0, D3DFMT_A8R8G8B8, 0, &m_rpTexture[i]);
				if (FAILED(hr))
					return hr;
			}
			
		// release unused textures
		for(i=nslices+nLOD; i<SLICETEXTURE_MAXSLICE*2-1; i++)
			SAFE_RELEASE(m_rpTexture[i]);
	}
	m_dwNumSlices = nslices;

	// Render into volume slices
	struct { // save current back buffer, z buffer, and transforms
		IDirect3DSurface8 *pBackBuffer, *pZBuffer;
		D3DMATRIX matWorld, matView, matProjection;
	} save;
	g_pd3dDevice->GetRenderTarget(&save.pBackBuffer);
	g_pd3dDevice->GetDepthStencilSurface(&save.pZBuffer);
	g_pd3dDevice->GetTransform( D3DTS_WORLD, &save.matWorld);
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &save.matView);
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &save.matProjection);

	// Set up antialiasing filter and temporary render target
	UINT nSuper = 4;
	MipmapFilter filter(nSuper);
	LPDIRECT3DTEXTURE8 pTextureSuper = NULL;
	hr = g_pd3dDevice->CreateTexture(m_Width * nSuper, m_Height * nSuper, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTextureSuper);
	if (FAILED(hr))
		return hr;
	LPDIRECT3DSURFACE8 pZBufferSuper = NULL;
	LPDIRECT3DSURFACE8 pSurfaceSuper = NULL;
	// make a new depth buffer
	hr = g_pd3dDevice->CreateDepthStencilSurface(m_Width * nSuper, m_Height * nSuper, D3DFMT_LIN_D24S8, D3DMULTISAMPLE_NONE, &pZBufferSuper);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pTextureSuper);
		return hr;
	}
	pTextureSuper->GetSurfaceLevel(0, &pSurfaceSuper);
	hr = g_pd3dDevice->SetRenderTarget(pSurfaceSuper, pZBufferSuper);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pZBufferSuper);
		SAFE_RELEASE(pSurfaceSuper);
		SAFE_RELEASE(pTextureSuper);
		return hr;
	}

	// Create scratch textures for filtering down to the final texture
	LPDIRECT3DTEXTURE8 pTextureScratchY = NULL;
	LPDIRECT3DTEXTURE8 pTextureScratchX = NULL;
	hr = g_pd3dDevice->CreateTexture(m_Width * nSuper, m_Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTextureScratchY);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pTextureSuper);
		return hr;
	}
	hr = g_pd3dDevice->CreateTexture(m_Width, m_Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTextureScratchX);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pTextureScratchY);
		SAFE_RELEASE(pTextureSuper);
		return hr;
	}

	// set transformation to flip z and look down y axis, with bottom-most slice scaled and translated to map to [0,1]
	D3DMATRIX matView;
	matView.m[0][0] = 1.f;	matView.m[0][1] =  0.f;	matView.m[0][2] = 0.f;				matView.m[0][3] = 0.f;
	matView.m[1][0] = 0.f;	matView.m[1][1] =  0.f;	matView.m[1][2] = 0.5f * nslices;	matView.m[1][3] = 0.f;
	matView.m[2][0] = 0.f;	matView.m[2][1] =  1.f;	matView.m[2][2] = 0.f;				matView.m[2][3] = 0.f;
	matView.m[3][0] = 0.f;	matView.m[3][1] =  0.f;	matView.m[3][2] = 0.5f * nslices;	matView.m[3][3] = 1.f;
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matIdentity);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matIdentity);
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity);
	hr = pDrawCB->Begin(false);
	if (FAILED(hr))
		goto e_Exit;

	// draw each slice
	int i;
	for (i = 0; i < (int)nslices; i++)
	{
		g_pd3dDevice->SetRenderTarget(pSurfaceSuper, pZBufferSuper);
		g_pd3dDevice->BeginScene();
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0);
		matView.m[1][2] = 0.5f * nslices;
		matView.m[3][2] = 0.5f * nslices - (float)i;	// offset to next slice
		D3DXVECTOR3 vMin(-1.f, (float)i/(float)nslices, -1.f), vMax(1.f, (float)(i+1)/(float)nslices, 1.f);
		// We want the texture to wrap, so draw multiple times with offsets in the plane so that 
		// the boundaries will be filled in by the overlapping geometry.
		int iX0, iX1;
		if (WrapU == D3DTADDRESS_WRAP)
		{
			iX0 = -1; 
			iX1 = 1;
		}
		else
			iX0 = iX1 = 0;
		int iY0, iY1;
		if (WrapV == D3DTADDRESS_WRAP)
		{
			iY0 = -1; 
			iY1 = 1;
		}
		else
			iY0 = iY1 = 0;
#if DEBUG_SHOW_SLICING
		// Show the slicing in action by drawing the clip frustum.
		extern HRESULT DebugFrustumBegin();
		extern HRESULT DebugFrustumEnd();
		extern HRESULT DebugPresent();
#endif
		for (int iX = iX0; iX <= iX1; iX++)
		{
			for (int iY = iY0; iY <= iY1; iY++)
			{
				matView.m[3][0] = 2.f * iX;
				matView.m[3][1] = 2.f * iY;
				g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView);	// position the current slice
#if DEBUG_SHOW_SLICING
				{
					// Draw the geometry in the "from-the-side" view
					DebugFrustumBegin();
					pDrawCB->Draw(vMin, vMax);
					DebugFrustumEnd();
				}
#endif	
				pDrawCB->Draw(vMin, vMax);
			}
		}
#if DEBUG_SHOW_SLICING
		DebugPresent();
#endif
		g_pd3dDevice->EndScene();

		// Before we filter the texture, make sure we're done rendering
		g_pd3dDevice->SetRenderTarget(save.pBackBuffer, save.pZBuffer);
		g_pd3dDevice->BlockUntilIdle();

		// Filter down from high-resolution original buffer to texture
		LPDIRECT3DTEXTURE8 pTexture = m_rpTexture[i];
		LPDIRECT3DSURFACE8 pSurface = NULL;
		pTexture->GetSurfaceLevel(0, &pSurface);
		hr = Decimate(pSurface, pTextureSuper, 0, WrapU, WrapV, pTextureScratchY, pTextureScratchX, &filter);
		if (FAILED(hr))
			goto e_Exit;
#if DEBUG_SHOW_SLICING
		// Display the resulting slice texture
		extern HRESULT DebugTexture(LPDIRECT3DTEXTURE8 pTexture, UINT iLevel);
		DebugTexture(pTexture, 0);
#endif		
		GenerateMipmaps(pTexture, 0, WrapU, WrapV, pTextureScratchY, pTextureScratchX);
	}

e_Exit:
	pDrawCB->End();

	// restore back and z buffer and transforms
	g_pd3dDevice->SetRenderTarget(save.pBackBuffer, save.pZBuffer);
	SAFE_RELEASE(save.pBackBuffer);
	SAFE_RELEASE(save.pZBuffer);
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &save.matWorld);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &save.matView);
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &save.matProjection);
	SAFE_RELEASE(pZBufferSuper);
	SAFE_RELEASE(pSurfaceSuper);
	SAFE_RELEASE(pTextureScratchX);
	SAFE_RELEASE(pTextureScratchY);
	SAFE_RELEASE(pTextureSuper);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Choose level of detail 
//
// 0       = finest detail, all slices of original source textures
// ...
// i       = reduced number of slices, N / (1 << i)
// i + f   = odd slices fade to clear, texLOD[2*j+1] = tex[2*j+1] * (1-f) + clear * f
//           even slices compensate,   texLOD[2*j] = (tex[2*j+1] * f) OVER tex[2*j]
// i + 1   = reduced number of slices, N / (1 << (i+1))
// ...
// log2(N) = coarsest, one slice with composite of all source textures
//-----------------------------------------------------------------------------
HRESULT CSliceTexture::SetLevelOfDetail(float fLevelOfDetail)
{
	// Choose number of LOD slices
	if (fLevelOfDetail < 0.f)
	{
		m_fLevelOfDetail = 0.f;
		m_iLOD = 0;
		m_fLODFraction = 0.f;
	}
	else if (fLevelOfDetail > (float)m_dwLODMax)
	{
		m_fLevelOfDetail = (float)m_dwLODMax;
		m_iLOD = m_dwLODMax;
		m_fLODFraction = 0.f;
	}
	else
	{
		m_fLevelOfDetail = fLevelOfDetail;
		m_iLOD = (UINT)floorf(fLevelOfDetail);
		m_fLODFraction = fLevelOfDetail - (float)m_iLOD;
	}
	m_dwNumSlicesLOD = LevelOfDetailCount(m_iLOD);
	UINT index = LevelOfDetailIndex(m_iLOD);
	m_rpTextureLOD = m_rpTexture + index;
	return S_OK;
}

//-----------------------------------------------------------------------------
//
// Generate level-of-detail textures by compositing together alternating layers.
//
//-----------------------------------------------------------------------------
HRESULT CSliceTexture::ComputeLevelOfDetailTextures()
{
	// All the textures must have the same number of mip levels.
	DWORD nMip = m_rpTexture[0]->GetLevelCount();

	// save current back buffer and z buffer
	struct {
		IDirect3DSurface8 *pBackBuffer, *pZBuffer;
	} save;
	g_pd3dDevice->GetRenderTarget(&save.pBackBuffer);
	g_pd3dDevice->GetDepthStencilSurface(&save.pZBuffer);

	// set render state for compositing textures
	g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_TEX1);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// use pixel shaders to composite two or three layers at a time
	DWORD dwPS2 = 0;
	{
#include "comp2.inl"
		g_pd3dDevice->CreatePixelShader(&psd, &dwPS2);
	}
	DWORD dwPS3 = 0;
	{
#include "comp3.inl"
		g_pd3dDevice->CreatePixelShader(&psd, &dwPS3);
	}

	// set default texture stage states
	UINT xx; // texture stage index
	for (xx = 0; xx < 4; xx++)
	{
		g_pd3dDevice->SetTexture(xx, NULL);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLOROP, D3DTOP_DISABLE);	// Are the COLOROP and ALPHAOP needed since we're using a pixel shader?
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);	// pass texture coords without transformation
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_TEXCOORDINDEX, 0);			// all the textures use the same tex coords
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MAGFILTER, D3DTEXF_POINT);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MINFILTER, D3DTEXF_POINT);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MIPFILTER, D3DTEXF_POINT);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MIPMAPLODBIAS, 0);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MAXMIPLEVEL, 0);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLORSIGN, 0);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE);
	}
	
	// Compute all the level-of-detail textures
	for (UINT iLOD = 1; m_dwNumSlices >> iLOD; iLOD++)
	{
		UINT nSliceSrc = LevelOfDetailCount(iLOD-1);
		LPDIRECT3DTEXTURE8 *apTextureSrc = m_rpTexture + LevelOfDetailIndex(iLOD - 1);
		UINT nSliceDst = LevelOfDetailCount(iLOD);
		LPDIRECT3DTEXTURE8 *apTextureDst = m_rpTexture + LevelOfDetailIndex(iLOD);
		
		// Composite source textures into LOD textures
		UINT iMipNotHandled = -1;
		for (UINT iSliceDst = 0; iSliceDst < nSliceDst; iSliceDst++)
		{
			LPDIRECT3DTEXTURE8 pTextureDst = apTextureDst[iSliceDst];
			UINT nComp;
			if (iSliceDst == nSliceDst-1 && nSliceSrc > nSliceDst * 2)
			{
				// composite 3 textures into the top-most level when number of source textures is odd
				nComp = 3;
				g_pd3dDevice->SetPixelShader(dwPS3);
			}
			else
			{
				// composite 2 textures (this is the default)
				nComp = 2;
				g_pd3dDevice->SetPixelShader(dwPS2);
			}
			for (xx = 0; xx < nComp; xx++)
			{
				g_pd3dDevice->SetTexture(xx, apTextureSrc[ iSliceDst * 2 + xx]);
				g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			}
			for (; xx<4; xx++)
			{
				g_pd3dDevice->SetTexture(xx, NULL);
				g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLOROP, D3DTOP_DISABLE);
				g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}
			for (UINT iMip = 0; iMip < nMip; iMip++)
			{
				DWORD width = m_Width / (1 << iMip);
				if (width == 0) width = 1;
				DWORD height = m_Height / (1 << iMip);
				if (height == 0) height = 1;

				// Xbox render target must of be at least 16x16
				if (width*4 < 64 || width * height < 64)			
				{
					iMipNotHandled = iMip;
					break; // skip rest of coarser mipmaps and go to next slice
				}

				// Use a screen space quad to do the compositing.
				struct quad {
					float x, y, z, w;
					float u, v;
				} aQuad[4] =
				{
					{-0.5f,        -0.5f,         1.0f, 1.0f, 0.0f, 0.0f},
					{width - 0.5f, -0.5f,         1.0f, 1.0f, 1.0f, 0.0f},
					{-0.5f,        height - 0.5f, 1.0f, 1.0f, 0.0f, 1.0f},
					{width - 0.5f, height - 0.5f, 1.0f, 1.0f, 1.0f, 1.0f}
				};

				// get destination surface and set as render target
				IDirect3DSurface8 *pSurface;
				pTextureDst->GetSurfaceLevel(iMip, &pSurface);
				g_pd3dDevice->SetRenderTarget(pSurface, NULL); // no depth buffering
				g_pd3dDevice->BeginScene();	
				g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aQuad, sizeof(quad)); // one quad blends 2 or 3 textures
				g_pd3dDevice->EndScene();
				pSurface->Release();
			}
		}
		if (iMipNotHandled > 0 && iMipNotHandled != -1)
		{
			// fill in the small mips with filtered versions of the previous levels
			for (UINT iSliceDst = 0; iSliceDst < nSliceDst; iSliceDst++)
			{
				LPDIRECT3DTEXTURE8 pTextureDst = apTextureDst[iSliceDst];
				// D3DXFilterTexture(pTextureDst, NULL, iMipNotHandled - 1, D3DX_FILTER_BOX | D3DX_FILTER_MIRROR);
				GenerateMipmaps(pTextureDst, iMipNotHandled - 1);
			}
		}
	}

	// clean up pixel shaders
	g_pd3dDevice->SetPixelShader(0);
	g_pd3dDevice->DeletePixelShader(dwPS2);
	g_pd3dDevice->DeletePixelShader(dwPS3);
	
	// restore render states
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	// clear texture stage states
	for (xx=0; xx<4; xx++)
	{
		g_pd3dDevice->SetTexture(xx, NULL);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}

	// restore back buffer and z buffer
	g_pd3dDevice->SetRenderTarget(save.pBackBuffer, save.pZBuffer);
	save.pBackBuffer->Release();
	save.pZBuffer->Release();
	return S_OK;
}

//-----------------------------------------------------------------------------
//
// Copy textures to new texture format
//
//-----------------------------------------------------------------------------
HRESULT CSliceTexture::SetTextureFormat(D3DFORMAT fmtNew)
{
	HRESULT hr;
	for (UINT iTexture = 0; iTexture < SLICETEXTURE_MAXSLICE * 2 - 1; iTexture++)	// convert all the slice textures
	{
		LPDIRECT3DTEXTURE8 pTextureDst = NULL;
		LPDIRECT3DTEXTURE8 pTextureSrc = m_rpTexture[iTexture];
		if (pTextureSrc == NULL)
			break;
		hr = CopyTexture(&pTextureDst, fmtNew, pTextureSrc);
		if (FAILED(hr))
			return hr;
		m_rpTexture[iTexture] = pTextureDst; // already addref'd
		pTextureSrc->Release();	// we're done with the old texture
	}
	return S_OK;
}
