//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Cubemap.h"

bool CCubemap::LoadCubicTexture8(void)
{
/*
    HRESULT hr;

    if (bCopy)
    {
        RECT Rect;

	    Rect.left = 0;
	    Rect.top  = 0;

	    for (int i=0;i<6;i++) 
        {
    		for (int j=0;j<m_paSysTexture->m_pSrcSurface->GetNumAttachedSurfaces();j++) 
            {
                CDirectDrawSurface * pSurface = m_paTexture->m_pSrcSurface->GetCubicSurface(i)->GetAttachedSurface(j);
                CDirectDrawSurface * pSysSurface = m_paSysTexture->m_pSrcSurface->GetCubicSurface(i)->GetAttachedSurface(j);

	            Rect.right  = pSurface->m_dwWidth;
	            Rect.bottom = pSurface->m_dwHeight;

                // Use copyrect method
                hr = m_pSrcDevice8->CopyRects(pSysSurface->m_pSurface8, &Rect, 1, pSurface->m_pSurface8, NULL);

                if (FAILED(hr))
	            {
		            WriteToLog("Source CopyRects(%d) failed with HResult = %s.",i,GetHResultString(hr).c_str());
                    RELEASE(pSurface);
                    RELEASE(pSysSurface);
		            return false;
	            }

                RELEASE(pSurface);
                RELEASE(pSysSurface);

	            if (NULL != m_pRefTarget)
	            {
                    CDirectDrawSurface * pSurface = m_paTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);
                    CDirectDrawSurface * pSysSurface = m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);

    	            Rect.right  = pSurface->m_dwWidth;
	                Rect.bottom = pSurface->m_dwHeight;

                    // Use copyrect method
                    hr = m_pRefDevice8->CopyRects(pSysSurface->m_pSurface8, &Rect, 1, pSurface->m_pSurface8, NULL);

                    if (FAILED(hr))
		            {
			            WriteToLog("Reference CopyRects(%d) failed with HResult = %s.",i,GetHResultString(hr).c_str());
                        RELEASE(pSurface);
                        RELEASE(pSysSurface);
			            return false;
		            }

                    RELEASE(pSurface);
                    RELEASE(pSysSurface);
	            }
            }
        }
    }

    if (bUpdate)
    {
        // Update the Source texture
        hr = m_pSrcDevice8->UpdateTexture(m_paSysTexture->m_pSrcCubeTexture8, m_paTexture->m_pSrcCubeTexture8);

        if (FAILED(hr))
        {
		    WriteToLog("Source UpdateTexture() failed with HResult = %s.\n",GetHResultString(hr).c_str());
		    return false;
        }

        if (NULL != m_pRefTarget)
        {
            // Update the Reference texture
            hr = m_pRefDevice8->UpdateTexture(m_paSysTexture->m_pRefCubeTexture8, m_paTexture->m_pRefCubeTexture8);

            if (FAILED(hr))
            {
		        WriteToLog("Reference UpdateTexture() failed with HResult = %s.\n",GetHResultString(hr).c_str());
		        return false;
            }
        }
    }
*/
    return true;
}

bool CCubemap::CheckMipCubeCap8(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Check the caps we are interested in
	DWORD dwTextureCaps = m_d3dcaps.TextureCaps;

	if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP))
	{
		WriteToLog("\nDevice capability not supported: Texture MipCubeMap.");
		return false;
    }

    return true;
}

bool CCubemap::CheckCubeFilterCaps8(DWORD dwMin, DWORD dwMag, DWORD dwMip)
{
    DWORD dwCubeTextureFilterCaps;
//    UINT n = m_pMode->nSrcDevice;

	// Check the caps we are interested in
    if (m_dwVersion >= 0x800)
    	dwCubeTextureFilterCaps = m_d3dcaps.CubeTextureFilterCaps;
//    else
//    	dwCubeTextureFilterCaps = m_pAdapter->Devices[n].Desc.dwTextureFilterCaps;

    if (dwMin)
    {
	    if (!(dwCubeTextureFilterCaps & dwMin))
		    return false;
    }

    if (dwMag)
    {
        if (!(dwCubeTextureFilterCaps & dwMag))
		    return false;
    }
	
    if (dwMip)
    {
        if (!(dwCubeTextureFilterCaps & dwMip))
		    return false;
    }

    return true;
}
