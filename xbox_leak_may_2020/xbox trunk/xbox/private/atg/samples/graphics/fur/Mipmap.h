//-----------------------------------------------------------------------------
//  
//  File: mipmap.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Mipmap filtering using the GPU
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>

//////////////////////////////////////////////////////////////////////
// Filter coefficients and offsets for separable mipmap filtering.
//
class MipmapFilter {
 public:
	UINT m_nSuperSample;		// number of source samples per destination pixel
	float m_fHalfWidth;			// size of filter support
	UINT m_nSample;				// number of filter samples
	struct Sample {
		float m_fOffset;		// offset in destination pixel coords
		float m_fValue;			// value at offset
	} *m_rSample;
	int m_iSuperMin, m_iSuperMax;	// range of supersamples
 public:
#define MIPMAPFILTER_NORMALIZE_ADD		001 /* add (1-sum)/N to each filter coefficient */
#define MIPMAPFILTER_NORMALIZE_MULTIPLY	002 /* multiply each value by 1/sum */
#define MIPMAPFILTER_STRETCH				010 /* expand filter slightly to make tighter frequency bound */
#define MIPMAPFILTER_KEEPZEROS			020 /* filter values that quantize to zero are usually culled, when 255 * f < 0.5 */
	MipmapFilter(UINT nSuperSample = 2,	// number of source samples per output pixel
		   float fHalfWidth = 2.f,	// in destination pixel coords, filter is assumed to be zero outside this bound
		   float (*pfFilter)(float x, float fHalfWidth) = lanczos,	// filter kernel
		   DWORD dwFlags = MIPMAPFILTER_NORMALIZE_ADD);	// normalization flags
	~MipmapFilter() { delete [] m_rSample; }

	// kernel helper functions
	void NormalizeAdd();		// add (1 - sum)/N to each value
	void NormalizeMultiply();	// multiply each value by 1/sum
	static float triangle(float x, float fHalfWidth);
	static float mitchell(float x, float fHalfWidth);
	static float sinc(float x);							// sin(x) / x
	static float lanczos(float x, float fHalfWidth);	// sinc windowed sinc
	static float hamming(float x, float fHalfWidth);	// hamming-windowed sinc
};

//////////////////////////////////////////////////////////////////////
// Use filter coefficients to resample from higher resolution
// pTextureSrc to lower resolution pSurfaceDst.
// The width of Src must be m_nSuperSample * width of Dst and
// the height of Src must be m_nSuperSample * height of Dst.
// The scratch texture must be swizzled and have width >= Src and height >= Src
// If either pTextureScratchY or pTextureScratchX are NULL, temporary
// textures will be created and then released when done.
//
HRESULT Decimate(LPDIRECT3DSURFACE8 pSurfaceDst,
				 LPDIRECT3DTEXTURE8 pTextureSrc,
				 UINT iSourceLevel,	// index of source miplevel in pTextureSrc
				 D3DTEXTUREADDRESS WrapU = D3DTADDRESS_WRAP, 
				 D3DTEXTUREADDRESS WrapV = D3DTADDRESS_WRAP,
				 LPDIRECT3DTEXTURE8 pTextureScratchY = NULL,	// destination for filtering in Y
				 LPDIRECT3DTEXTURE8 pTextureScratchX = NULL,	// destination for filtering in X
				 MipmapFilter *pFilter = NULL);

//////////////////////////////////////////////////////////////////////
// Compute mipmaps starting from iSourceLevel
// The scratch textures must be swizzled and have width >= Src and height >= Src
// If either pTextureScratchY or pTextureScratchX are NULL, temporary
// textures will be created and then released when done.
// The filter must have nSuperSample = 2.
//
HRESULT GenerateMipmaps(LPDIRECT3DTEXTURE8 pTexture,
						UINT iSourceLevel,
						D3DTEXTUREADDRESS WrapU = D3DTADDRESS_WRAP, 
						D3DTEXTUREADDRESS WrapV = D3DTADDRESS_WRAP,
						LPDIRECT3DTEXTURE8 pTextureScratchY = NULL,
						LPDIRECT3DTEXTURE8 pTextureScratchX = NULL,
						MipmapFilter *pFilter = NULL);

//////////////////////////////////////////////////////////////////////
// Create a new texture the same size as the source texture, with the
// same number of mipmap levels, and then copy the source to the
// destination, with a format change.  This function handles swizzled and
// unswizzled textures.
//
HRESULT CompressTexture(LPDIRECT3DTEXTURE8 *ppTextureDst, D3DFORMAT fmtNew, LPDIRECT3DTEXTURE8 pTextureSrc);
