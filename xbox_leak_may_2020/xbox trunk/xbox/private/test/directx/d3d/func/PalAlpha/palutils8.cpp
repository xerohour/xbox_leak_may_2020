#define BURN_IN_HELL
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PalAlpha.h"


CTexture8* CPalTest::CreateTexture8(DWORD dwWidth, DWORD dwHeight) {
	CTexture8*	pTex = NULL;
	CTexture8*	pSysTex = NULL;
    CSurface8*  pSur = NULL;
    CSurface8*  pSysSur = NULL;
    D3DFORMAT   Format = D3DFMT_UNKNOWN;
    D3DSURFACE_DESC     SDesc;
    D3DLOCKED_RECT      d3dlr;
    BYTE        *p;
    int         i, j;
    HRESULT     hr;

    // Check allocation
//    if (pTex == NULL || pSysTex == NULL)
//    {
//        WriteToLog("PalAlpha: Not enough memory to create texture object\n");
//        goto error;
//    }

    // Check texture format
    if (dwPaletteType != 3)
    {
        WriteToLog("PalAlpha: 4 bit palettized textures are not supported\n");
        goto error;
    }

    // Source textures
    if (m_pSrcDevice8 != NULL)
    {
        // Source texture
#ifndef UNDER_XBOX
        if (!SetLastError(m_pSrcDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, D3DPOOL_DEFAULT, &pTex)))
#else
        if (!SetLastError(m_pSrcDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, POOL_DEFAULT, &pTex)))
#endif // UNDER_XBOX
            goto error;
//        pTex->m_pSrcSurface = new CDirectDrawSurface;
//        if (pTex->m_pSrcSurface == NULL)
//        {
//            WriteToLog("PalAlpha: Not enough memory to create surface object\n");
//            goto error;
//        }
//        pTex->m_pSrcSurface->Initialize(this);
        if (!SetLastError(pTex->GetSurfaceLevel(0, &pSur)))
        {
            WriteToLog("PalAlpha: GetSurfaceLevel failed\n");
            goto error;
        }
        hr = pSur->GetDesc(&SDesc);
        if (FAILED(hr))
//        if (pTex->m_pSrcSurface->GetSurfaceDescription(&SDesc))
//            pTex->m_pSrcSurface->ProcessSurfaceDescription(&SDesc);
//        else
        {
            DebugString(TEXT("CDirect3D: Failed to get texture surface description"));
            RELEASE(pSur);
            RELEASE(pTex);
            return NULL;
        }
		
        // Source sysmem texture
#ifndef UNDER_XBOX
        if (!SetLastError(m_pSrcDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, D3DPOOL_SYSTEMMEM, &pSysTex)))
#else
        if (!SetLastError(m_pSrcDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, POOL_DEFAULT, &pSysTex)))
#endif // UNDER_XBOX
            goto error;
//        pSysTex->m_pSrcSurface = new CDirectDrawSurface;
//        if (pSysTex->m_pSrcSurface == NULL)
//        {
//            WriteToLog("PalAlpha: Not enough memory to create surface object\n");
//            goto error;
//        }
//        pSysTex->m_pSrcSurface->Initialize(this);
        if (!SetLastError(pSysTex->GetSurfaceLevel(0, &pSysSur)))
        {
            WriteToLog("PalAlpha: GetSurfaceLevel failed\n");
            goto error;
        }
        hr = pSysSur->GetDesc(&SDesc);
        if (FAILED(hr))
//        if (pSysTex->m_pSrcSurface->GetSurfaceDescription(&SDesc))
//            pSysTex->m_pSrcSurface->ProcessSurfaceDescription(&SDesc);
//        else
        {
            DebugString(TEXT("PalAlpha: Failed to get texture surface description."));
            RELEASE(pSysSur);
            RELEASE(pSysTex);
            RELEASE(pSur);
            RELEASE(pTex);
            return NULL;
        }
    
        // Fill source sysmem texture
//        p = (BYTE*)(pSysTex->m_pSrcSurface->Lock());
        hr = pSysSur->LockRect(&d3dlr, NULL, 0);
        if (FAILED(hr)) {
            goto error;
        }
        p = (BYTE*)d3dlr.pBits;
		if (p == NULL)
			goto error;
		for (i = 0; i < (int)SDesc.Height; i++, p += d3dlr.Pitch) {
			BYTE	color = (BYTE)(i*255/SDesc.Height);
			for (j = 0; j < (int)SDesc.Width; j++)
				p[j] = color;
		}
//		if (!pSysTex->m_pSrcSurface->Unlock())
        hr = pSysSur->UnlockRect();
        if (FAILED(hr))
			goto error;

        // Copy image to source texture
        SetLastError(m_pSrcDevice8->UpdateTexture(pSysTex, pTex));
    }
/*
	// Reference texture
    if (m_pRefDevice8 != NULL)
    {
        // Reference texture
        if (!SetLastError(m_pRefDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, D3DPOOL_DEFAULT, &pTex->m_pRefTexture8)))
            goto error;
        pTex->m_pRefSurface = new CDirectDrawSurface;
        if (pTex->m_pRefSurface == NULL)
        {
            WriteToLog("PalAlpha: Not enough memory to create surface object\n");
            goto error;
        }
        pTex->m_pRefSurface->Initialize(this);
        if (!SetLastError(pTex->m_pRefTexture8->GetSurfaceLevel(0, &pTex->m_pRefSurface->m_pSurface8)))
        {
            WriteToLog("PalAlpha: GetSurfaceLevel failed\n");
            goto error;
        }
        if (pTex->m_pRefSurface->GetSurfaceDescription(&SDesc))
            pTex->m_pRefSurface->ProcessSurfaceDescription(&SDesc);
        else
        {
            DebugOut.Write(1, "CDirect3D: Failed to get texture surface description.\n");
            delete pTex;
            return NULL;
        }
	
        // Reference sysmem texture
        if (!SetLastError(m_pRefDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, D3DPOOL_SYSTEMMEM, &pSysTex->m_pRefTexture8)))
            goto error;
        pSysTex->m_pRefSurface = new CDirectDrawSurface;
        if (pSysTex->m_pRefSurface == NULL)
        {
            WriteToLog("PalAlpha: Not enough memory to create surface object\n");
            goto error;
        }
        pSysTex->m_pRefSurface->Initialize(this);
        if (!SetLastError(pSysTex->m_pRefTexture8->GetSurfaceLevel(0, &pSysTex->m_pRefSurface->m_pSurface8)))
        {
            WriteToLog("PalAlpha: GetSurfaceLevel failed\n");
            goto error;
        }
        if (pSysTex->m_pRefSurface->GetSurfaceDescription(&SDesc))
            pSysTex->m_pRefSurface->ProcessSurfaceDescription(&SDesc);
        else
        {
            DebugOut.Write(1, "PalAlpha: Failed to get texture surface description.\n");
            delete pTex;
            return NULL;
        }

        // Fill source sysmem texture
        p = (BYTE*)(pSysTex->m_pRefSurface->Lock());
		if (p == NULL)
			goto error;
		for (i = 0; i < pSysTex->m_pRefSurface->m_dwHeight; i++, p += pSysTex->m_pRefSurface->m_lPitch) {
			BYTE	color = (BYTE)(i*255/pSysTex->m_pRefSurface->m_dwHeight);
			for (j = 0; j < pSysTex->m_pRefSurface->m_dwWidth; j++)
				p[j] = color;
		}
		if (!pSysTex->m_pRefSurface->Unlock())
			goto error;

        // Copy image to source texture
        SetLastError(m_pRefDevice8->UpdateTexture(pSysTex->m_pRefTexture8, pTex->m_pRefTexture8));
    }
*/	
    RELEASE(pSysSur);
    RELEASE(pSur);
    RELEASE(pSysTex);
    return pTex;

error:
    RELEASE(pSysSur);
    RELEASE(pSur);
    RELEASE(pSysTex);
    RELEASE(pTex);
    WriteToLog("PalAlpha: CreateTexture failed\n");
    return NULL;
}

bool CPalTest::CreateTexturePalette8(DWORD dwPalette, bool bAlphaPalette, DWORD dwParam) {
	PALETTEENTRY		pal[256];

	FillPaletteEntries(pal, dwPaletteType, bAlphaPalette, dwParam, false);

	if (!SetLastError(m_pSrcDevice8->SetPaletteEntries(dwPalette, pal))) {
		WriteToLog("PalAlpha: SetPaletteEntries failed\n");
		return false;
	}
//	if (!SetLastError(m_pRefDevice8->SetPaletteEntries(dwPalette, pal))) {
//		WriteToLog("PalAlpha: SetPaletteEntries failed\n");
//		return false;
//	}

	return true;
}

bool CPalTest::SetCurrentTexturePalette8(DWORD dwPalette) {
	if (!SetLastError(m_pSrcDevice8->SetCurrentTexturePalette(dwPalette))) {
		WriteToLog("PalAlpha: SetCurrentTexturePalette failed\n");
		return false;
	}
//	if (!SetLastError(m_pRefDevice8->SetCurrentTexturePalette(dwPalette))) {
//		WriteToLog("PalAlpha: SetCurrentTexturePalette failed\n");
//		return false;
//	}

	return true;
}

