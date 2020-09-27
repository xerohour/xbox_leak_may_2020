//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Compress.h"

bool CCompressTest::DXTnLoad8(void)
{
    HRESULT hr;
    PALETTECOLOR    *pPal = NULL;
    D3DSURFACE_DESC d3dsd;
    PALETTECOLOR    Palette[256];

    hr = pSysDXTnTexture->GetLevelDesc(0, &d3dsd);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
        return false;
    }

    if (d3dsd.Format == D3DFMT_P8)
    {
		ZeroMemory(Palette, sizeof(PALETTECOLOR) * 256);

#ifndef UNDER_XBOX
        hr = m_pSrcDevice8->GetPaletteEntries(0, Palette);

        if (FAILED(hr))
        {
            WriteToLog("Source GetPaletteEntries() failed with HResult = %X.\n",hr);
		    return false;
        }
#else
        CPalette8* pd3dp = NULL;
        D3DCOLOR* pc;
        hr = m_pDevice->GetPalette(0, &pd3dp);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetPalette"))) {
            return false;
        }
        pd3dp->Lock(&pc, 0);
        memcpy(Palette, pc, 256 * sizeof(PALETTECOLOR));
        pd3dp->Unlock();
        pd3dp->Release();
#endif

        pPal = Palette;
    }

/*
    hr = D3DXLoadSurfaceFromSurface(
		     pSysDXTnTexture,
		     pPal, 
             NULL,					    
			 pTexture, 
			 pPal,					    
			 NULL,					   
			 D3DX_FILTER_POINT, 	  
             0);
*/
    hr = CopyTexture(pSysDXTnTexture, pPal, pTexture, pPal, D3DX_FILTER_POINT);

    if (FAILED(hr))
    {
        WriteToLog("Source D3DXLoadSurfaceFromSurface() failed with HResult = %X.\n",hr);
		return false;
    }

#ifndef UNDER_XBOX

    hr = CopyTexture(pDXTnTexture, pPal, pSysDXTnTexture, pPal, D3DX_FILTER_POINT);

    if (FAILED(hr))
    {
		WriteToLog("Source CopyTexture() failed with HResult = %X.\n",hr);
		return false;
    }

#else

    m_pDisplay->EnableRMC(FALSE);

    // Update the Source texture
    hr = m_pSrcDevice8->UpdateTexture(pSysDXTnTexture, pDXTnTexture);

    m_pDisplay->EnableRMC(TRUE);

    if (FAILED(hr))
    {
		WriteToLog("Source UpdateTexture() failed with HResult = %X.\n",hr);
		return false;
    }

    if (!SyncTexture(pDXTnTexture)) {
        return false;
    }

#endif // UNDER_XBOX

/*
    if (NULL != m_pRefTarget)
    {
        if (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat == D3DFMT_P8)
        {
            PALETTEENTRY	Palette[256];
		    ZeroMemory(Palette, sizeof(PALETTEENTRY) * 256);

            hr = m_pRefDevice8->GetPaletteEntries(0, Palette);

            if (FAILED(hr))
            {
                WriteToLog("Reference GetPaletteEntries() failed with HResult = %s.\n",GetHResultString(hr).c_str());
		        return false;
            }

            pPal = Palette;
        }

        hr = D3DXLoadSurfaceFromSurface(
		         pSysDXTnTexture->m_pRefSurface->m_pSurface8,
		         pPal, 
                 NULL,					    
			     pTexture->m_pRefSurface->m_pSurface8, 
			     pPal,					    
			     NULL,					   
    			 D3DX_FILTER_POINT, 	  
                 0);

        if (FAILED(hr))
        {
            WriteToLog("Reference D3DXLoadSurfaceFromSurface() failed with HResult = %s.\n",GetHResultString(hr).c_str());
		    return false;
        }

        // Update the Reference texture
        hr = m_pRefDevice8->UpdateTexture(pSysDXTnTexture->m_pRefTexture8, pDXTnTexture->m_pRefTexture8);

        if (FAILED(hr))
        {
		    WriteToLog("Reference UpdateTexture() failed with HResult = %s.\n",GetHResultString(hr).c_str());
		    return false;
        }
    }
*/
    return true;
}
