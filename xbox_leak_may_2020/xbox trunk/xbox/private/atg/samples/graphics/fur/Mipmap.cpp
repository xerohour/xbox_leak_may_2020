//-----------------------------------------------------------------------------
//  
//  File: mipmap.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Mipmap filtering using the GPU
//
//-----------------------------------------------------------------------------
#include "mipmap.h"
#include <XBUtil.h>
#include "SwizzleFormat.h"
#include <assert.h>

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//////////////////////////////////////////////////////////////////////
// Default filter is lanczos with halfwidth 2 and 2 supersamples.  That
// works out to 8 taps per output pixel, with a small negative lobe for
// sharpness.
static MipmapFilter s_MipmapFilterDefault(2, 2.f, MipmapFilter::lanczos, MIPMAPFILTER_NORMALIZE_ADD);

//////////////////////////////////////////////////////////////////////
// Sample filter kernel, weight by super sample area, and normalize.
//
MipmapFilter::MipmapFilter(UINT nSuperSample,
						   float fHalfWidth,
						   float (*pfFilter)(float x, float fHalfWidth),
						   DWORD dwFlags)
{
	if (nSuperSample == 0) nSuperSample = 1;
	m_nSuperSample = nSuperSample;
	m_fHalfWidth = fHalfWidth;

	// If number of supersamples is even, offset by half pixel.
	float fSampleOffset;
	if (nSuperSample % 2)
		fSampleOffset = 0.f;
	else
		fSampleOffset = 0.5f;		

	// Get number of supersamples in support
	int iSuperMin = (int)ceilf(-fHalfWidth * nSuperSample - fSampleOffset);
	int iSuperMax = (int)floorf(fHalfWidth * nSuperSample - fSampleOffset);
	m_nSample = iSuperMax - iSuperMin + 1;

	// Get adjusted filter support width
	float fStretch = 1.f;
	if (dwFlags & MIPMAPFILTER_STRETCH) 
	{
		fStretch = 1.f - 0.5f / fHalfWidth;
		iSuperMin = (int)ceilf(-fHalfWidth * nSuperSample - fSampleOffset);
		iSuperMax = (int)floorf(fHalfWidth * nSuperSample - fSampleOffset);
		m_nSample = iSuperMax - iSuperMin + 1;
	}

	// Allocate sample array
	m_rSample = new Sample [ m_nSample ];

	// Sample filter at supersample positions
	float fStep = 1.f / nSuperSample;
	float fWeight = fStretch * fStep;
	UINT iSample = 0;
	m_iSuperMin = m_iSuperMax = 0;
	for (int iSuper = iSuperMin; iSuper <= iSuperMax; iSuper++)
	{
		float fOffset = (iSuper + fSampleOffset) * fStep;
		float fValue = fWeight * (*pfFilter)(fOffset * fStretch, fHalfWidth);
		if (!(dwFlags & MIPMAPFILTER_KEEPZEROS))
		{
			float fQuant = 255.f * fabsf(fValue);
			if (fQuant < 0.5f) continue; // remove values that quantize to zero
		}
		m_rSample[iSample].m_fOffset = fOffset;
		m_rSample[iSample].m_fValue = fValue;
		iSample++;
		if (iSuper < m_iSuperMin) m_iSuperMin = iSuper;
		if (iSuper > m_iSuperMax) m_iSuperMax = iSuper;
	}

	// Re-allocate sample array after removing zeros
	if (iSample < m_nSample)
	{
		Sample *rSampleOld = m_rSample;
		m_nSample = iSample;
		m_rSample = new Sample [ m_nSample ];
		for (iSample = 0; iSample < m_nSample; iSample++)
			m_rSample[iSample] = rSampleOld[iSample];
		delete [] rSampleOld;
	}

	// Normalize
	if (dwFlags & MIPMAPFILTER_NORMALIZE_MULTIPLY)
		NormalizeMultiply();
	else if (dwFlags & MIPMAPFILTER_NORMALIZE_ADD)
		NormalizeAdd();
}

//////////////////////////////////////////////////////////////////////
// Add (1 - sum)/N to each filter value.
//
void MipmapFilter::NormalizeAdd()
{
	float fSum = 0.f;
	for (UINT iSample = 0; iSample < m_nSample; iSample++)
		fSum += m_rSample[iSample].m_fValue;
	float fOffset = (1.f - fSum) / m_nSample;
	for ( iSample = 0; iSample < m_nSample; iSample++)
		m_rSample[iSample].m_fValue += fOffset;
}

//////////////////////////////////////////////////////////////////////
// Multiply each filter coefficient by 1/sum of the original coefficients.
//
void MipmapFilter::NormalizeMultiply()
{
	float fSum = 0.f;
	for (UINT iSample = 0; iSample < m_nSample; iSample++)
		fSum += m_rSample[iSample].m_fValue;
	float fScale = 1.f/fSum;
	for ( iSample = 0; iSample < m_nSample; iSample++)
		m_rSample[iSample].m_fValue *= fScale;
}

//////////////////////////////////////////////////////////////////////
// Standard sin(pi*x)/(pi*x) function that is the inverse fourier
// transform of a box filter in the frequency domain.  The problem with
// the sinc is that it has infinite extent. The lanczos and hamming filters
// below take a finite window and smoothly "roll off" at the edges.
//
float MipmapFilter::sinc(float x)
{
	if (x == 0.f)
		return 1.f;
	else
		return sinf(D3DX_PI * x) / (D3DX_PI * x);
}

//////////////////////////////////////////////////////////////////////
// Triangle filter is not a too bad approximation to a windowed sinc,
// but doesn't have a negative lobe to make the result sharp.  The
// lanczos, hamming, and mitchell filters are better.
//
float MipmapFilter::triangle(float x, float fHalfWidth)
{
	float r = x / fHalfWidth;
	if (r < -1.f || r > 1.f)
		return 0.0;
	else if (r < 0.0) 
		return (1 + r) / fHalfWidth;
	else 
		return (1 - r) / fHalfWidth;
}

//////////////////////////////////////////////////////////////////////
// Sinc-windowed sinc
//
float MipmapFilter::lanczos(float x, float fHalfWidth)
{
	if (x < -fHalfWidth || x > fHalfWidth)
		return 0.f;
	else
		return sinc(x/fHalfWidth) * sinc(x);
}

//////////////////////////////////////////////////////////////////////
// Hamming-windowed sinc
//
float MipmapFilter::hamming(float x, float fHalfWidth)
{
	if (x < -fHalfWidth || x > fHalfWidth)
		return 0.f;
	else
		return (0.46f * cosf(D3DX_PI * x / fHalfWidth) + 0.54f) * sinc(x);
}

//////////////////////////////////////////////////////////////////////
// Cubic filter that has a shape similar to the lanczos and hamming filters,
// but has slightly smaller negative lobes.
//
float MipmapFilter::mitchell(float x, float fHalfWidth)
{
	const float B = 0.3f;
	const float C = 0.3f;
	float r = (x < 0.f) ? -x : x;
	r /= 0.5f * fHalfWidth;
	if (r <= 1.f)
		return (6 - 2*B + r*r*(-18 + 12*B + 6*C + r*(12 - 9*B - 6*C)))/6;
	else if (r <= 2.f)
		return (8*B + 24*C + r*(-12*B - 48*C + r*(6*B + 30*C + r*(-B - 6*C))))/6;
	else 
		return 0.f;
}

//////////////////////////////////////////////////////////////////////
// CopyRects even if the formats are swizzled.
// 
HRESULT CopyRects(D3DSurface *pSurfaceSrc, CONST RECT *pSrcRectsArray, UINT cRects, D3DSurface *pSurfaceDst, CONST POINT *pDstPointsArray)
{
	HRESULT hr;
	D3DSURFACE_DESC descSrc;
	pSurfaceSrc->GetDesc(&descSrc);
	BOOL bSwizzledSrc = XGIsSwizzledFormat(descSrc.Format);
	D3DSURFACE_DESC descDst;
	pSurfaceDst->GetDesc(&descDst);
	BOOL bSwizzledDst = XGIsSwizzledFormat(descDst.Format);
	if (! bSwizzledSrc && !bSwizzledDst ) // if formats are both unswizzled, use regular CopyRects
		return g_pd3dDevice->CopyRects(pSurfaceSrc, pSrcRectsArray, cRects, pSurfaceDst, pDstPointsArray);
	if (MapSwizzledToLinearFormat(descSrc.Format) != MapSwizzledToLinearFormat(descDst.Format))
		return D3DERR_WRONGTEXTUREFORMAT;

	// Otherwise, copy pixel by pixel
	DWORD cbPixel = XGBytesPerPixelFromFormat(descSrc.Format);
	D3DLOCKED_RECT lockedRectSrc;
	hr = pSurfaceSrc->LockRect(&lockedRectSrc, NULL, 0);
	if (FAILED(hr))
		return hr;
	D3DLOCKED_RECT lockedRectDst;
	hr = pSurfaceDst->LockRect(&lockedRectDst, NULL, 0);
	if (FAILED(hr))
	{
		pSurfaceSrc->UnlockRect();
		return hr;
	}
	for (UINT iRect = 0; iRect < cRects; iRect++)
	{
		RECT rectSrc;
		if (pSrcRectsArray)
			rectSrc = pSrcRectsArray[iRect];
		else
		{
			rectSrc.left = 0;
			rectSrc.right = descSrc.Width;
			rectSrc.top = 0;
			rectSrc.bottom = descSrc.Height;
		}
		POINT pointDst;
		if (pDstPointsArray)
			pointDst = pDstPointsArray[iRect];
		else
			pointDst.x = pointDst.y = 0;
		if (bSwizzledSrc && bSwizzledDst)
		{
			Swizzler swizSrc(descSrc.Width, descSrc.Height, 0);
			Swizzler swizDst(descDst.Width, descDst.Height, 0);
			swizSrc.SetV(swizSrc.SwizzleV(rectSrc.top));
			swizDst.SetV(swizDst.SwizzleV(pointDst.y));
			for (long v = rectSrc.top; v < rectSrc.bottom; v++)
			{
				swizSrc.SetU(swizSrc.SwizzleU(rectSrc.left));
				swizDst.SetU(swizDst.SwizzleU(pointDst.x));
				for (long u = rectSrc.left; u < rectSrc.right; u++)
				{
					BYTE *pSrc = (BYTE *)lockedRectSrc.pBits + cbPixel * swizSrc.Get2D();
					BYTE *pDst = (BYTE *)lockedRectDst.pBits + cbPixel * swizDst.Get2D();
					memcpy(pDst, pSrc, cbPixel);
					swizSrc.IncU();
					swizDst.IncU();
				}
				swizSrc.IncV();
				swizDst.IncV();
			}
		}
		else if (bSwizzledSrc) // && !bSwizzledDst
		{
			Swizzler swizSrc(descSrc.Width, descSrc.Height, 0);
			swizSrc.SetV(swizSrc.SwizzleV(rectSrc.top));
			for (long v = rectSrc.top; v < rectSrc.bottom; v++)
			{
				long vDst = pointDst.y + v - rectSrc.top;
				swizSrc.SetU(swizSrc.SwizzleU(rectSrc.left));
				for (long u = rectSrc.left; u < rectSrc.right; u++)
				{
					long uDst = pointDst.x + u - rectSrc.left;
					BYTE *pSrc = (BYTE *)lockedRectSrc.pBits + cbPixel * swizSrc.Get2D();
					BYTE *pDst = (BYTE *)lockedRectDst.pBits + vDst * lockedRectDst.Pitch + uDst * cbPixel;
					memcpy(pDst, pSrc, cbPixel);
					swizSrc.IncU();
				}
				swizSrc.IncV();
			}
		}
		else // !bSwizzledSrc && bSwizzledDst
		{
			Swizzler swizDst(descDst.Width, descDst.Height, 0);
			swizDst.SetV(swizDst.SwizzleV(pointDst.y));
			for (long v = rectSrc.top; v < rectSrc.bottom; v++)
			{
				swizDst.SetU(swizDst.SwizzleU(pointDst.x));
				for (long u = rectSrc.left; u < rectSrc.right; u++)
				{
					BYTE *pSrc = (BYTE *)lockedRectSrc.pBits + v * lockedRectSrc.Pitch + u * cbPixel;
					BYTE *pDst = (BYTE *)lockedRectDst.pBits + cbPixel * swizDst.Get2D();
					memcpy(pDst, pSrc, cbPixel);
					swizDst.IncU();
				}
				swizDst.IncV();
			}
		}
	}
	pSurfaceSrc->UnlockRect();
	pSurfaceDst->UnlockRect();
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Helper function for allocating scratch textures, if needed.
//
static HRESULT AllocScratchYX(MipmapFilter *pFilter,
							  D3DSURFACE_DESC &descSrc,
							  LPDIRECT3DTEXTURE8 &pTextureScratchY,
							  LPDIRECT3DTEXTURE8 &pTextureScratchX)
{
	HRESULT hr;
	if (pTextureScratchY == NULL)
	{
		DWORD width = descSrc.Width;
		DWORD height = descSrc.Height / pFilter->m_nSuperSample;	// result will be decimated in Y
		if (height < 1) height = 1;
		if (width * XGBytesPerPixelFromFormat(descSrc.Format) < 64  // Xbox render target must have at least the minimum pitch
			|| width * height < 64)
		{
			width = 64;	// 16 is smallest reasonable render target
			if (height < width)	// if source height is larger use it, otherwise use reasonable default
				height = width;
		}
		hr = g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, descSrc.Format, D3DPOOL_DEFAULT, &pTextureScratchY);
		if (FAILED(hr))
			return hr;
	}
	else
	{
		D3DSURFACE_DESC descScratchY;
		pTextureScratchY->GetLevelDesc(0, &descScratchY);
		if (descScratchY.Width < descSrc.Width										// Y scratch must match _source_ width
			|| descScratchY.Height < descSrc.Height / pFilter->m_nSuperSample		// ... and destination height
			|| descScratchY.Width * XGBytesPerPixelFromFormat(descScratchY.Format) < 64  // Xbox render target must have at least the minimum pitch
			|| descScratchY.Width * descScratchY.Height < 64
			|| MapSwizzledToLinearFormat(descSrc.Format) != MapSwizzledToLinearFormat(descScratchY.Format))	// formats must be compatible
			return E_INVALIDARG;
		pTextureScratchY->AddRef();	// to balance Release, below
	}
	if (pTextureScratchX == NULL)
	{
		DWORD width = descSrc.Width / pFilter->m_nSuperSample;		// result will be decimated in X
		if (width < 1) width = 1;
		DWORD height = descSrc.Height / pFilter->m_nSuperSample;	// result will be decimated in Y by previous filter pass
		if (height < 1) height = 1;
		if (width * XGBytesPerPixelFromFormat(descSrc.Format) < 64  // Xbox render target must have at least the minimum pitch
			|| width * height < 64)
		{
			width = 64;	// 16 is smallest reasonable render target
			if (height < width)	// if source height is larger use it, otherwise use reasonable default
				height = width;
		}
		hr = g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, descSrc.Format, D3DPOOL_DEFAULT, &pTextureScratchX);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pTextureScratchY);
			return hr;
		}
	}
	else
	{
		D3DSURFACE_DESC descScratchX;
		pTextureScratchX->GetLevelDesc(0, &descScratchX);
		if (descScratchX.Width < descSrc.Width / pFilter->m_nSuperSample			// X scratch must match destination width
			|| descScratchX.Height < descSrc.Height / pFilter->m_nSuperSample		// ... and destination height
			|| descScratchX.Width * XGBytesPerPixelFromFormat(descScratchX.Format) < 64  // Xbox render target must have at least the minimum pitch
			|| descScratchX.Width * descScratchX.Height < 64
			|| MapSwizzledToLinearFormat(descSrc.Format) != MapSwizzledToLinearFormat(descScratchX.Format))	// formats must be compatible
		{
			SAFE_RELEASE(pTextureScratchY);
			return E_INVALIDARG;
		}
		pTextureScratchX->AddRef();	// to balance Release, below
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Uses Decimate to compute mipmaps starting with iSourceLevel
//
HRESULT GenerateMipmaps(LPDIRECT3DTEXTURE8 pTexture,
						UINT iSourceLevel,
						D3DTEXTUREADDRESS WrapU, 
						D3DTEXTUREADDRESS WrapV,
						LPDIRECT3DTEXTURE8 pTextureScratchY,
						LPDIRECT3DTEXTURE8 pTextureScratchX,
						MipmapFilter *pFilter)
{
	HRESULT hr;
	DWORD nMip = pTexture->GetLevelCount();
	if (iSourceLevel >= nMip - 1) return S_FALSE;	// nothing to filter
	if (!pFilter) pFilter = &s_MipmapFilterDefault;
	D3DSURFACE_DESC descSrc;
	hr = pTexture->GetLevelDesc(iSourceLevel, &descSrc);
	if (FAILED(hr))
		return hr;
	hr = AllocScratchYX(pFilter, descSrc, pTextureScratchY, pTextureScratchX);
	if (FAILED(hr))
		return hr;
	LPDIRECT3DSURFACE8 pSurfaceDst = NULL;
	for (UINT iMip = iSourceLevel + 1; iMip < nMip; iMip++)
	{
		hr = pTexture->GetSurfaceLevel(iMip, &pSurfaceDst); // get destination surface
		if (FAILED(hr))
			goto e_Exit;
		hr = Decimate(pSurfaceDst, pTexture, iMip - 1, WrapU, WrapV, pTextureScratchY, pTextureScratchX, pFilter);
		if (FAILED(hr))
			goto e_Exit;
		SAFE_RELEASE(pSurfaceDst);
	}
 e_Exit:
	SAFE_RELEASE(pSurfaceDst);
	SAFE_RELEASE(pTextureScratchX);
	SAFE_RELEASE(pTextureScratchY);
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Use filter coefficients to resample from higher resolution
// pSurfaceSrc to lower resolution pSurfaceDst.
// The width of Src must be m_nSuperSample * width of Dst and
// the height of Src must be m_nSuperSample * height of Dst.
// The Y scratch texture must have width >= Src and height >= Src / 2.
// The X scratch texture must have width >= Src / 2 and height >= Src / 2.
HRESULT Decimate(LPDIRECT3DSURFACE8 pSurfaceDst,
				 LPDIRECT3DTEXTURE8 pTextureSrc,
				 UINT iSourceLevel,	// index of source miplevel in pTextureSrc
				 D3DTEXTUREADDRESS WrapU, 
				 D3DTEXTUREADDRESS WrapV,
				 LPDIRECT3DTEXTURE8 pTextureScratchY,
				 LPDIRECT3DTEXTURE8 pTextureScratchX,
				 MipmapFilter *pFilter)
{
	HRESULT hr;
	if (!pFilter) pFilter = &s_MipmapFilterDefault;

	// Get sizes of intermediate and final render targets
	D3DSURFACE_DESC descSrc;
	pTextureSrc->GetLevelDesc(iSourceLevel, &descSrc);
	D3DSURFACE_DESC descDst;
	pSurfaceDst->GetDesc(&descDst);
	if ((descDst.Width != 1 && descDst.Width > descSrc.Width / pFilter->m_nSuperSample)
		|| (descDst.Height != 1 && descDst.Height > descSrc.Height / pFilter->m_nSuperSample))
		return E_INVALIDARG;	// texture sizes don't match
	hr = AllocScratchYX(pFilter, descSrc, pTextureScratchY, pTextureScratchX);
	if (FAILED(hr))
		return hr;
	D3DSURFACE_DESC descScratchY, descScratchX;
	pTextureScratchY->GetLevelDesc(0, &descScratchY);
	pTextureScratchX->GetLevelDesc(0, &descScratchX);
	DWORD dwStateBlock = (DWORD)-1;
	hr = g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &dwStateBlock);
	if (FAILED(hr))
		return hr;
	
	// save current back buffer, z buffer, and pixel shader
	struct {
		IDirect3DSurface8 *pBackBuffer, *pZBuffer;
	} save;
	g_pd3dDevice->GetRenderTarget(&save.pBackBuffer);
	g_pd3dDevice->GetDepthStencilSurface(&save.pZBuffer);

	// set render state for filtering
	g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_TEX4);	// for screen-space texture-mapped quadrilateral
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// use pixel shaders to add contributions from four samples at a time
	DWORD dwPS = 0;
#pragma warning(push)
#pragma warning(disable: 4245)	// conversion from int to DWORD
#include "mipmap.inl"
#pragma warning(pop)
	g_pd3dDevice->CreatePixelShader(&psd, &dwPS);
	g_pd3dDevice->SetPixelShader(dwPS);

	// set texture stage state
	UINT xx; // texture stage index
	for (xx = 0; xx < 4; xx++)
	{
		g_pd3dDevice->SetTexture(xx, pTextureSrc);	// use our source texture for all four stages
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);	// pass texture coords without transformation
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_TEXCOORDINDEX, xx); // each texture has different tex coords
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ADDRESSU, WrapU);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ADDRESSV, WrapV);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MAXMIPLEVEL, iSourceLevel);	// set most-detailed level to the desired source texture
		float fBias = -1000.f; 														// bias mipmap toward the most detailed level
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MINFILTER, D3DTEXF_POINT);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MIPFILTER, D3DTEXF_POINT);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MAGFILTER, D3DTEXF_POINT);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLORSIGN, 0);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE);
	}

	//
	// Pass 1: Filter in Y from Src to ScratchY
	//
	{
		LPDIRECT3DSURFACE8 pSurfaceScratchY = NULL;
		pTextureScratchY->GetSurfaceLevel(0, &pSurfaceScratchY);
		hr = g_pd3dDevice->SetRenderTarget(pSurfaceScratchY, NULL); // no depth buffering
		pSurfaceScratchY->Release();
		if (FAILED(hr))
			goto e_Exit;
		DWORD width = descSrc.Width;
		DWORD height = descSrc.Height / pFilter->m_nSuperSample;
		if (height < 1) height = 1;
		struct quad {
			float x, y, z, w1;
			struct uv {
				float u, v;
			} tex[4];	// each texture has different offset
		} aQuad[4] =
		  { //   X             Y              Z   1/W     u0  v0      u1  v1      u2  v2      u3  v3
			  {-0.5f,        -0.5f,         1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
			  {width - 0.5f, -0.5f,         1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
			  {-0.5f,        height - 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
			  {width - 0.5f, height - 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}
		  };
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);	// on first rendering, copy new value over current render target contents
		xx = 0;	// current texture stage
		float rfZero[4] = {0.f, 0.f, 0.f, 0.f};
		float fOffsetScale = (float)pFilter->m_nSuperSample / (float)descSrc.Height;	// source texture coords
		for (UINT iSample = 0; iSample < pFilter->m_nSample; iSample++)
		{
			// set filter coefficients
			float fValue = pFilter->m_rSample[iSample].m_fValue;
			if (fValue > 0.f)
			{
				float rf[4] = {fValue, fValue, fValue, fValue};
				g_pd3dDevice->SetPixelShaderConstant(xx, rf, 1);			// positive coeff
				g_pd3dDevice->SetPixelShaderConstant(xx + 4, rfZero, 1);	// negative coeff set to zero
			}
			else
			{
				float rf[4] = {-fValue, -fValue, -fValue, -fValue};
				g_pd3dDevice->SetPixelShaderConstant(xx, rfZero, 1);		// positive coeff set to zero
				g_pd3dDevice->SetPixelShaderConstant(xx + 4, rf, 1);		// negative coeff
			}
			// Align supersamples with center of destination pixels
			float fOffset = pFilter->m_rSample[iSample].m_fOffset * fOffsetScale;
			aQuad[0].tex[xx].u = 0.f;
			aQuad[0].tex[xx].v = fOffset;
			aQuad[1].tex[xx].u = 1.f;
			aQuad[1].tex[xx].v = fOffset;
			aQuad[2].tex[xx].u = 0.f;
			aQuad[2].tex[xx].v = 1.f + fOffset;
			aQuad[3].tex[xx].u = 1.f;
			aQuad[3].tex[xx].v = 1.f + fOffset;
			if (!XGIsSwizzledFormat(descSrc.Format))
			{
				for (int i = 0; i < 4; i++)
				{
					aQuad[i].tex[xx].u *= descSrc.Width;
					aQuad[i].tex[xx].v *= descSrc.Height;
				}
			}
			xx++;
			if (xx == 4)	// max texture stages
			{
				// draw the quad to filter the coefficients so far
				g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aQuad, sizeof(quad)); // one quad blends 4 textures
				g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE); // on subsequent renderings, add to what's in the render target
				xx = 0;
			}
		}
		if (xx != 0) // add in results of final filter coefficients
		{
			// zero out rest of texture stage coefficients
			for (xx; xx < 4; xx++)
			{
				g_pd3dDevice->SetPixelShaderConstant(xx, rfZero, 1);		// positive coeff set to zero
				g_pd3dDevice->SetPixelShaderConstant(xx + 4, rfZero, 1);	// negative coeff set to zero
			}
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aQuad, sizeof(quad)); // one quad blends 4 textures
		}
	}

	//
	// Pass 2: Filter in X from ScratchY to ScratchX
	//
	{
		DWORD width = descSrc.Width / pFilter->m_nSuperSample;
		if (width < 1) width = 1;
		DWORD height = descSrc.Height / pFilter->m_nSuperSample;
		if (height < 1) height = 1;
		for (xx = 0; xx < 4; xx++)
			g_pd3dDevice->SetTexture(xx, pTextureScratchY);	// use the Y scratch texture as source for all four stages
		// Adjust scratch texture coords to include just the used region
		D3DXVECTOR2 vOrigin(0.f, 0.f);
		D3DXVECTOR2 vSize((float)descSrc.Width/(float)descScratchY.Width, (float)height/(float)descScratchY.Height);
#if 0
		if (descDst.Width * XGBytesPerPixelFromFormat(descDst.Format) >= 64  // Xbox render target must have at least the minimum pitch
			&& descDst.Width * descDst.Height >= 64)
		{
			// If destination surface is large enough, use it as a render target.
			hr = g_pd3dDevice->SetRenderTarget(pSurfaceDst, NULL); // no depth buffering
			if (FAILED(hr))
				goto e_Exit;
		}
		else
#endif
		{
			// Set scratchX as render target
			LPDIRECT3DSURFACE8 pSurfaceScratchX = NULL;
			pTextureScratchX->GetSurfaceLevel(0, &pSurfaceScratchX);
			hr = g_pd3dDevice->SetRenderTarget(pSurfaceScratchX, NULL); // no depth buffering
			pSurfaceScratchX->Release();
			if (FAILED(hr))
				goto e_Exit;
		}
		struct quad {
			float x, y, z, w1;
			struct uv {
				float u, v;
			} tex[4];	// each texture has different offset
		} aQuad[4] =
		  { //   X             Y              Z   1/W     u0  v0      u1  v1      u2  v2      u3  v3
			  {-0.5f,        -0.5f,         1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
			  {width - 0.5f, -0.5f,         1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
			  {-0.5f,        height - 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
			  {width - 0.5f, height - 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}
		  };
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);	// on first rendering, copy new value over current render target contents
		xx = 0;	// current texture stage
		float rfZero[4] = {0.f, 0.f, 0.f, 0.f};
		// TODO: review offset scale when width gets close to 1
		float fOffsetScale = (float)pFilter->m_nSuperSample / (float)descScratchY.Width;	// source texture coords
		for (UINT iSample = 0; iSample < pFilter->m_nSample; iSample++)
		{
			// set filter coefficients
			float fValue = pFilter->m_rSample[iSample].m_fValue;
			if (fValue > 0.f)
			{
				float rf[4] = {fValue, fValue, fValue, fValue};
				g_pd3dDevice->SetPixelShaderConstant(xx, rf, 1);			// positive coeff
				g_pd3dDevice->SetPixelShaderConstant(xx + 4, rfZero, 1);	// negative coeff set to zero
			}
			else
			{
				float rf[4] = {-fValue, -fValue, -fValue, -fValue};
				g_pd3dDevice->SetPixelShaderConstant(xx, rfZero, 1);		// positive coeff set to zero
				g_pd3dDevice->SetPixelShaderConstant(xx + 4, rf, 1);		// negative coeff
			}
			// Align supersamples with center of destination pixels
			float fOffset = pFilter->m_rSample[iSample].m_fOffset * fOffsetScale;
			aQuad[0].tex[xx].u = vOrigin.x + fOffset;
			aQuad[0].tex[xx].v = vOrigin.y;
			aQuad[1].tex[xx].u = vOrigin.x + vSize.x + fOffset;
			aQuad[1].tex[xx].v = vOrigin.y;
			aQuad[2].tex[xx].u = vOrigin.x + fOffset;
			aQuad[2].tex[xx].v = vOrigin.y + vSize.y;
			aQuad[3].tex[xx].u = vOrigin.x + vSize.x + fOffset;
			aQuad[3].tex[xx].v = vOrigin.y + vSize.y;
			if (!XGIsSwizzledFormat(descScratchY.Format))
			{
				for (int i = 0; i < 4; i++)
				{
					aQuad[i].tex[xx].u *= descScratchY.Width;
					aQuad[i].tex[xx].v *= descScratchY.Height;
				}
			}
			xx++;
			if (xx == 4)	// max texture stages
			{
				// draw the quad to filter the coefficients so far
				g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aQuad, sizeof(quad)); // one quad blends 4 textures
				g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE); // on subsequent renderings, add to what's in the render target
				xx = 0;
			}
		}
		if (xx != 0) // add in results of final filter coefficients
		{
			// zero out rest of texture stage coefficients
			for (xx; xx < 4; xx++)
			{
				g_pd3dDevice->SetPixelShaderConstant(xx, rfZero, 1);		// positive coeff set to zero
				g_pd3dDevice->SetPixelShaderConstant(xx + 4, rfZero, 1);	// negative coeff set to zero
			}
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aQuad, sizeof(quad)); // one quad blends 4 textures
		}
	}

	// 
	// Copy result to output
	//
	{
		DWORD width = descDst.Width;
		DWORD height = descDst.Height;
		
		// Stop using the scratch texture
		g_pd3dDevice->SetRenderTarget(save.pBackBuffer, save.pZBuffer);
		for (xx = 0; xx < 4; xx++)
			g_pd3dDevice->SetTexture(xx, NULL);
		g_pd3dDevice->BlockUntilIdle();

		// Copy result from scratch texture to destination surface
		LPDIRECT3DSURFACE8 pSurfaceScratchX = NULL;
		pTextureScratchX->GetSurfaceLevel(0, &pSurfaceScratchX);
		RECT rect = { 0, 0, width, height };
		POINT point = { 0, 0 };
		hr = CopyRects(pSurfaceScratchX, &rect, 1, pSurfaceDst, &point);
		pSurfaceScratchX->Release();
		if (FAILED(hr))
			goto e_Exit;
	}

e_Exit:
	SAFE_RELEASE(pTextureScratchX);
	SAFE_RELEASE(pTextureScratchY);

	// clean up pixel shaders
	g_pd3dDevice->SetPixelShader(0);
	g_pd3dDevice->DeletePixelShader(dwPS);
	
	// restore render states
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	// clear texture stages
	for (xx=0; xx<4; xx++)
	{
		g_pd3dDevice->SetTexture(xx, NULL);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MIPMAPLODBIAS, 0);
	}

	// restore back buffer and z buffer
	g_pd3dDevice->SetRenderTarget(save.pBackBuffer, save.pZBuffer);
	SAFE_RELEASE(save.pBackBuffer);
	SAFE_RELEASE(save.pZBuffer);
	g_pd3dDevice->ApplyStateBlock(dwStateBlock);
	g_pd3dDevice->DeleteStateBlock(dwStateBlock);
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Create a new texture and copy the current texture to the new
// texture, with a format conversion.
//
HRESULT CompressTexture(LPDIRECT3DTEXTURE8 *ppTextureDst, D3DFORMAT fmtNew, LPDIRECT3DTEXTURE8 pTextureSrc)
{
	HRESULT hr;
	if (!pTextureSrc)
		return S_FALSE;
	*ppTextureDst = NULL;
	UINT nLevel = pTextureSrc->GetLevelCount();
	BYTE *pUnswizzledSrc = NULL;
	BYTE *pUnswizzledDst = NULL;
	LPDIRECT3DTEXTURE8 pTextureDst = NULL;
	LPDIRECT3DSURFACE8 pSurfaceSrc = NULL;
	LPDIRECT3DSURFACE8 pSurfaceDst = NULL;
	D3DSURFACE_DESC desc0;
	pTextureSrc->GetLevelDesc(0, &desc0);
	hr = g_pd3dDevice->CreateTexture(desc0.Width, desc0.Height, nLevel, 0, fmtNew, 0, &pTextureDst);
	if (FAILED(hr))
		goto e_Exit;
	if (XGIsSwizzledFormat(desc0.Format))
	{
		pUnswizzledSrc = new BYTE [ desc0.Width * desc0.Height * XGBytesPerPixelFromFormat(desc0.Format)]; 
		if (pUnswizzledSrc == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}
	}
	if (XGIsSwizzledFormat(fmtNew))
	{
		pUnswizzledDst = new BYTE [ desc0.Width * desc0.Height * XGBytesPerPixelFromFormat(fmtNew)];
		if (pUnswizzledDst == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}
	}
	for (UINT iLevel = 0; iLevel < nLevel; iLevel++)
	{
		D3DSURFACE_DESC desc;
		pTextureSrc->GetLevelDesc(iLevel, &desc);
		assert(desc.Format == desc0.Format);
		assert(desc.Width <= desc0.Width);
		assert(desc.Height <= desc0.Height);
		hr = pTextureSrc->GetSurfaceLevel(iLevel, &pSurfaceSrc);
		if (FAILED(hr))
			goto e_Exit;
		hr = pTextureDst->GetSurfaceLevel(iLevel, &pSurfaceDst);
		if (FAILED(hr))
			goto e_Exit;
		D3DLOCKED_RECT lockedRectSrc;
		hr = pSurfaceSrc->LockRect(&lockedRectSrc, NULL, D3DLOCK_READONLY);
		if (FAILED(hr))
			goto e_Exit;
		D3DLOCKED_RECT lockedRectDst;
		hr = pSurfaceDst->LockRect(&lockedRectDst, NULL, 0);
		if (FAILED(hr))
		{
			pSurfaceSrc->UnlockRect();
			goto e_Exit;
		}
		VOID *pBufSrc;
		DWORD dwPitchSrc;
		D3DFORMAT fmtSrc;
		if (XGIsSwizzledFormat(desc.Format))
		{
			XGUnswizzleRect(lockedRectSrc.pBits, desc.Width, desc.Height, NULL, pUnswizzledSrc, 0, NULL, XGBytesPerPixelFromFormat(desc.Format));
			pBufSrc = pUnswizzledSrc;
			dwPitchSrc = 0;
			fmtSrc = MapSwizzledToLinearFormat(desc.Format);
		}
		else
		{
			pBufSrc = lockedRectSrc.pBits;
			dwPitchSrc = lockedRectSrc.Pitch;
			fmtSrc = desc.Format;
		}
		VOID *pBufDst;
		DWORD dwPitchDst;
		D3DFORMAT fmtDst;
		if (XGIsSwizzledFormat(fmtNew))
		{
			pBufDst = pUnswizzledDst;
			dwPitchDst = 0;
			fmtDst = MapSwizzledToLinearFormat(fmtNew);
		}
		else
		{
			pBufDst = lockedRectDst.pBits;
			dwPitchDst = lockedRectDst.Pitch;
			fmtDst = fmtNew;
		}
		hr = XGCompressRect(pBufDst, fmtDst, dwPitchDst, desc.Width, desc.Height, pBufSrc, fmtSrc, dwPitchSrc, 0.f, 0 );
		if (FAILED(hr))
		{
			pSurfaceSrc->UnlockRect();
			pSurfaceDst->UnlockRect();
			goto e_Exit;
		}
		if (XGIsSwizzledFormat(fmtNew))
			XGSwizzleRect(pBufDst, dwPitchDst, NULL, lockedRectDst.pBits, desc.Width, desc.Height, NULL, XGBytesPerPixelFromFormat(fmtNew));
		pSurfaceSrc->UnlockRect();
		pSurfaceDst->UnlockRect();
		SAFE_RELEASE(pSurfaceSrc);
		SAFE_RELEASE(pSurfaceDst);
	}
	*ppTextureDst = pTextureDst;	// already AddRef'd
	pTextureDst = NULL;	// to avoid a Release below
 e_Exit:
	SAFE_RELEASE(pTextureDst);
	SAFE_DELETE(pUnswizzledSrc);
	SAFE_DELETE(pUnswizzledDst);
	SAFE_RELEASE(pSurfaceSrc);
	SAFE_RELEASE(pSurfaceDst);
	return hr;
}
