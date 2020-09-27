#define BURN_IN_HELL
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PalAlpha.h"

/*
CTexture* CPalTest::CreateTexture(DWORD dwWidth, DWORD dwHeight, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill) {
	CTexture*	pTex = new CTexture;
	HRESULT		hrLastError = 0;
	DWORD		dwFlags = 0;

	// Source texture
	pTex->m_pSrcSurface = new CDirectDrawSurface;
	if (pTex->m_pSrcSurface == NULL)
	{
		delete pTex;
		return NULL;
	}
    pTex->m_pSrcSurface->Initialize(this);
    if (!m_pAdapter->Devices[m_pMode->nSrcDevice].fHardware)
		dwFlags = DDSCAPS_SYSTEMMEMORY;
	if (!CreatePalettedTexture(pTex->m_pSrcSurface, dwWidth, dwHeight, dwFlags, dwPaletteType, bFillTexture, bColorKeyFill))
	{
		delete pTex;
		return NULL;
	}

	// Reference texture
	if (m_pRefTarget != NULL)
	{
		pTex->m_pRefSurface = new CDirectDrawSurface;
		if (pTex->m_pRefSurface == NULL)
		{
			delete pTex;
			return NULL;
		}
		pTex->m_pRefSurface->Initialize(this);
		dwFlags = DDSCAPS_SYSTEMMEMORY;
		if (!CreatePalettedTexture(pTex->m_pRefSurface, dwWidth, dwHeight, dwFlags, dwPaletteType, bFillTexture, bColorKeyFill))
		{
			delete pTex;
			return NULL;
		}
	}
	return pTex;
}


bool CPalTest::CreatePalettedTexture(CDirectDrawSurface *pSurface, DWORD dwWidth, DWORD dwHeight, DWORD dwFlags, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill) {
	DDSURFACEDESC2			descPal;
	DWORD					i, j;
	BYTE					*p;

	// check for correct palette type
	if ( !(dwPaletteType == 3 || dwPaletteType == 2) )
		return false;

	// protection for lazy programming
	if (pSurface == NULL)
		return false;
	
	// create texture surface
	ZeroMemory(&descPal, sizeof(descPal));
	descPal.dwSize = sizeof(descPal);
	descPal.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_CAPS;
	descPal.dwWidth = dwWidth;
	descPal.dwHeight = dwHeight;
	descPal.ddpfPixelFormat.dwSize = sizeof(descPal.ddpfPixelFormat);
	switch(dwPaletteType) {
	case 3:
		descPal.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8;
		descPal.ddpfPixelFormat.dwRGBBitCount = 8;
		break;
	case 2:
		descPal.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED4;
		descPal.ddpfPixelFormat.dwRGBBitCount = 4;
		break;
	}
    descPal.ddsCaps.dwCaps = DDSCAPS_TEXTURE | dwFlags;
	if (!bFillTexture)
		descPal.ddsCaps.dwCaps |= DDSCAPS_ALLOCONLOAD;

	if (!pSurface->CreateSurface(&descPal))
		return false;

	if (bFillTexture) {
		// fill texture
		p = (unsigned char*)(pSurface->Lock());
		if (p == NULL)
			return false;

		if (dwPaletteType == 3) {
			for (i = 0; i < pSurface->m_dwHeight; i++, p += pSurface->m_lPitch) {
				BYTE	color = (BYTE)(i*255/pSurface->m_dwHeight);
                if (bColorKeyFill)
                    color = color - (color % 20);
				for (j = 0; j < pSurface->m_dwWidth; j++)
					p[j] = color;
			}
		}
		else if (dwPaletteType == 2) {
			for (i = 0; i < pSurface->m_dwHeight; i++, p += pSurface->m_lPitch) {
				BYTE	color = (BYTE)(i*15/pSurface->m_dwHeight);
				color |= (color << 4);
				for (j = 0; j < pSurface->m_dwWidth/2; j++)
					p[j] = color;
			}
		}
		
		if (!pSurface->Unlock()) {
			return false;
		}
	}
	return true;
}


bool CPalTest::CreateTexturePalette(CDirectDrawSurface* pSurface, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill) {
	DWORD				flags;
	PALETTEENTRY		pal[256];
	HRESULT				hr;


	if (pSurface == NULL)
		return FALSE;

	if (pPalette != NULL) {
		pPalette->Release();
		pPalette = NULL;
	}

	if (dwPaletteType == 3) {
		flags = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
		if (bAlphaPalette)
			flags |= DDPCAPS_ALPHA;
	}
	else {
		flags = DDPCAPS_4BIT;
		if (bAlphaPalette)
			flags |= DDPCAPS_ALPHA;
	}

	FillPaletteEntries(pal, dwPaletteType, bAlphaPalette, dwParam, bColorKeyFill);

	if (m_dwVersion == 0x0700 && m_pDDraw7 != NULL)
		hr = m_pDDraw7->CreatePalette(flags, pal, &pPalette, NULL);
	else if (m_dwVersion == 0x0600 && m_pDDraw4 != NULL)
		hr = m_pDDraw4->CreatePalette(flags, pal, &pPalette, NULL);
    else 
    {
		WriteToLog("$rError: $yCan't create palette\n");
		pPalette = NULL;
        return FALSE;
    }
	if (hr != DD_OK) {
		WriteToLog("$rError: $yCan't create palette\n");
		SetLastError(hr);
		pPalette = NULL;
		return FALSE;
	}

	// attach the palette to the texture surface
	if (m_dwVersion == 0x0700 && pSurface->m_pSurface7 != NULL)
		hr = pSurface->m_pSurface7->SetPalette(pPalette);
	else if (m_dwVersion == 0x0600 && pSurface->m_pSurface4 != NULL)
		hr = pSurface->m_pSurface4->SetPalette(pPalette);
    else
    {
		WriteToLog("$rError: $yCan't set palette\n");
		pPalette->Release();
		pPalette = NULL;
        return FALSE;
    }
	if (hr != DD_OK) {
		WriteToLog("$rError: $yCan't set palette\n");
		SetLastError(hr);
		pPalette->Release();
		pPalette = NULL;
		return FALSE;
	}

	pPalette->Release();
	pPalette = NULL;

	return TRUE;
}

bool CPalTest::ChangeTexturePalette(CDirectDrawSurface* pSurface, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill) {
	HRESULT				hr;
	PALETTEENTRY		pal[256];
	LPDIRECTDRAWPALETTE	pPal;
	DWORD				entries = 0;

	if (pSurface == NULL)
		return FALSE;

	if (dwPaletteType ==  3)
		entries = 256;
	else if (dwPaletteType == 2)
		entries = 16;

	if (m_dwVersion == 0x0700)
		hr = pSurface->m_pSurface7->GetPalette(&pPal);
	else if (m_dwVersion == 0x0600)
		hr = pSurface->m_pSurface4->GetPalette(&pPal);
    else
        return FALSE;
	if (hr != DD_OK) {
		WriteToLog("$rError: $yCan't get texture surface palette\n");
		SetLastError(hr);
		return FALSE;
	}

	hr = pPal->GetEntries(0, 0, entries, pal);
	if (hr != DD_OK) {
		WriteToLog("$Error: $yCan't retrieve palette entries\n");
		SetLastError(hr);
		pPal->Release();
		return FALSE;
	}

	FillPaletteEntries(pal, dwPaletteType, bAlphaPalette, dwParam, bColorKeyFill);

	hr = pPal->SetEntries(0, 0, entries, pal);
	if (hr != DD_OK) {
		WriteToLog("$Error: $yCan't set palette entries\n");
		SetLastError(hr);
		pPal->Release();
	}

	pPal->Release();
	return TRUE;
}
*/

void CPalTest::FillPaletteEntries(PALETTEENTRY *pal, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill) {
	DWORD			entries = 0;
	DWORD			i;
//	unsigned char	mask = (unsigned char) (cParam >> 5);
//	unsigned char	value = (unsigned char) ((cParam & 0x1f) << 3);

	if (dwPaletteType ==  3)
		entries = 256;
	else if (dwPaletteType == 2)
		entries = 16;

	for(i = 0; i < entries; i++) {
		// Color base value
		unsigned long a = i*256/entries + dwParam*256*8/dwSameFramesNumber + 128;
		unsigned char b = unsigned char(a&0x00ff);
		unsigned char mask = unsigned char( (a>>8)&0x07 );
        unsigned char base;
        
        // Base color
        if (bColorKeyFill) 
        {
            base = b;
            mask = (mask == 0) ? 0x07 : mask;
        }
        else
            base = (unsigned char) (255.* (0.5 - 0.5*cos(6.28*double(b)/256.)));

		// Alpha component
		if (bAlphaPalette)
			pal[i].peFlags = base;
		else
			pal[i].peFlags = 0; //D3DPAL_RESERVED;
		// Red component
		if (mask & 0x04)
			pal[i].peRed = base;
		else
			pal[i].peRed = 0;
		// Green component
		if (mask & 0x02)
			pal[i].peGreen = base;
		else
			pal[i].peGreen = 0;
		// Blue component
		if (mask & 0x01)
			pal[i].peBlue = base;
		else
			pal[i].peBlue = 0;
	}
}

/*

bool CPalTest::ColorSurface(LPDIRECTDRAWSURFACE4 pSurf) {
	HRESULT			hr;
	DDSURFACEDESC2	ddsd;
	BYTE			*p;
	DWORD			dwRedMask, dwGreenMask, dwBlueMask;
	DWORD			dwRedShift, dwGreenShift, dwBlueShift;

	if (pSurf == NULL)
		return FALSE;

	// fill background
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
	hr = pSurf->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
	if (hr == DD_OK) {
		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16) {
			int		s, m;
			for (s = 0, m = ddsd.ddpfPixelFormat.dwRBitMask; !(m & 1) && m; s++, m >>= 1);
			dwRedShift = s;
			dwRedMask = 255 / (ddsd.ddpfPixelFormat.dwRBitMask >> s);
			for (s = 0, m = ddsd.ddpfPixelFormat.dwGBitMask; !(m & 1) && m; s++, m >>= 1);
			dwGreenShift = s;
			dwGreenMask = 255 / (ddsd.ddpfPixelFormat.dwGBitMask >> s);
			for (s = 0, m = ddsd.ddpfPixelFormat.dwBBitMask; !(m & 1) && m; s++, m >>= 1);
			dwBlueShift = s;
			dwBlueMask = 255 / (ddsd.ddpfPixelFormat.dwBBitMask >> s);
		}
		p = (BYTE*)ddsd.lpSurface;
		for (DWORD i = 0; i < ddsd.dwHeight; i++, p += ddsd.lPitch) {
			for (DWORD j = 0; j < ddsd.dwWidth; j++) {
				int r = i * 255 / ddsd.dwHeight;
				int g = j * 255 / ddsd.dwWidth;
				int b = 255 - abs(r - g);
//				if (bPalette) {
//					DWORD	dwRGB = RGBA_MAKE(r, g, b, 0);
//					BYTE	color = (BYTE)GetNearestColor(dwRGB);
//					p[j] = color;
//				}
				if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16) {
					unsigned short color;
					r /= dwRedMask;
					g /= dwGreenMask;
					b /= dwBlueMask;
					color = (unsigned short)((r << dwRedShift) | (g << dwGreenShift) | (b << dwBlueShift));
					*(unsigned short*)(&p[j*2]) = color;
				}
				else if (ddsd.ddpfPixelFormat.dwRGBBitCount == 24) {
					p[j*3  ] = (BYTE)r;
					p[j*3+1] = (BYTE)g;
					p[j*3+2] = (BYTE)b;
				}
				else if (ddsd.ddpfPixelFormat.dwRGBBitCount == 32) {
					p[j*4  ] = (BYTE)r;
					p[j*4+1] = (BYTE)g;
					p[j*4+2] = (BYTE)b;
				}
			}
		}
		hr = pSurf->Unlock(NULL);
	}
	else {
		WriteToLog("$Error: $yCan't lock back buffer\n");
		SetLastError(hr);
		return FALSE;
	}

	return TRUE;
}
*/
