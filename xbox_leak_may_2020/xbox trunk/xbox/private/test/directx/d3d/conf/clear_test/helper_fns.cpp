#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "clear_test.h"

bool CClearTest::CreateOffScrRendTrgts(int nTexFormat)
{
    HRESULT hr;
    D3DSURFACE_DESC Desc;

/*
    // Create a src surface for rendering
    m_pOffSrcSurf = new CDirectDrawSurface(this);

    if (m_dwVersion <= 0x0700)
    {
    	DWORD dwSurfaceCrFlags;

        if (m_pAdapter->Devices[m_pMode->nSrcDevice].fHardware)
		    dwSurfaceCrFlags = CDDS_SURFACE_VIDEO | CDDS_SURFACE_OFFSCREEN;
	    else
		    dwSurfaceCrFlags = CDDS_SURFACE_SYSTEM | CDDS_SURFACE_OFFSCREEN;

	    if (!m_pOffSrcSurf->CreateSurface(IMGWIDTH, IMGHEIGHT, (LPDDPIXELFORMAT)&m_pCommonTextureFormats[nTexFormat].ddpfPixelFormat, dwSurfaceCrFlags))
	    {
		    WriteToLog("Src CreateSurface failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		    RELEASE(m_pOffSrcSurf);
		    return false;
	    }
    }
    else // m_dwVersion >= 0x0800
    {
*/
        hr = m_pSrcDevice8->CreateRenderTarget(IMGWIDTH, 
                                               IMGHEIGHT,
                                               m_fmtCommon[nTexFormat],
                                               D3DMULTISAMPLE_NONE,
                                               TRUE,
                                               &m_pOffSrcSurf);
        if (FAILED(hr))
        {
	        WriteToLog("Src CreateRenderTarget() failed with HResult = %X.\n",hr);
//		    RELEASE(m_pOffSrcSurf);
		    return false;
	    }
/*
        // Get and process surface description 
        if (m_pOffSrcSurf->GetSurfaceDescription(&Desc))
            m_pOffSrcSurf->ProcessSurfaceDescription(&Desc);
        else
        {
	        WriteToLog("Failed to get src surface description.\n");
		    RELEASE(m_pOffSrcSurf);
		    return false;
        }
    }

    if (m_pRefTarget != NULL)
    {
        // Create a ref surface for rendering
        m_pOffRefSurf = new CDirectDrawSurface(this);

        if (m_dwVersion <= 0x0700)
        {
    	    DWORD dwSurfaceCrFlags;

	        if (m_pAdapter->Devices[m_pMode->nRefDevice].fHardware)
		        dwSurfaceCrFlags = CDDS_SURFACE_VIDEO | CDDS_SURFACE_OFFSCREEN;
	        else
		        dwSurfaceCrFlags = CDDS_SURFACE_SYSTEM | CDDS_SURFACE_OFFSCREEN;

	        if (!m_pOffRefSurf->CreateSurface(IMGWIDTH, IMGHEIGHT, (LPDDPIXELFORMAT)&m_pCommonTextureFormats[nTexFormat].ddpfPixelFormat, dwSurfaceCrFlags))
	        {
		        WriteToLog("Ref CreateSurface failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		        RELEASE(m_pOffRefSurf);
		        return false;
	        }
        }
        else // m_dwVersion >= 0x0800
        {
            hr = m_pRefDevice8->CreateRenderTarget(IMGWIDTH, 
												   IMGHEIGHT,
                                                   m_pCommonTextureFormats[nTexFormat].d3dfFormat,
                                                   D3DMULTISAMPLE_NONE,
                                                   true,
                                                   &m_pOffRefSurf->m_pSurface8); 
            if (FAILED(hr))
            {
	            WriteToLog("Ref CreateRenderTarget() failed with HResult = %s.\n",GetHResultString(hr).c_str());
		        RELEASE(m_pOffSrcSurf);
		        RELEASE(m_pOffRefSurf);
		        return false;
	        }

            // Get and process surface description 
            if (m_pOffRefSurf->GetSurfaceDescription(&Desc))
                m_pOffRefSurf->ProcessSurfaceDescription(&Desc);
            else
            {
	            WriteToLog("Failed to get ref surface description.\n");
		        RELEASE(m_pOffRefSurf);
		        return false;
            }
        }
    }
*/

	return true;
}










bool CClearTest::CreateZStencil(int nZFormat)
{
	HRESULT hr;

//	CDirectDrawSurface * pZSurface = new CDirectDrawSurface(this);
/*
	if (m_dwVersion <= 0x0700)
	{
		DWORD dwSurfaceCrFlags; 

		if (m_pAdapter->Devices[m_pMode->nSrcDevice].fHardware)
			dwSurfaceCrFlags = CDDS_SURFACE_VIDEO;
		else
			dwSurfaceCrFlags = CDDS_SURFACE_SYSTEM;

		if (!pZSurface->CreateZBuffer(IMGWIDTH, IMGHEIGHT, (LPDDPIXELFORMAT)&m_pCommonZBufferFormats[nZFormat].ddpfPixelFormat, dwSurfaceCrFlags))
		{
			WriteToLog("Src CreateZBuffer failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
			RELEASE(pZSurface);
			return false;            
		}
	} 
	else // m_dwVersion >= 0x0800
	{
*/
		hr = m_pSrcDevice8->CreateDepthStencilSurface(IMGWIDTH, 
													  IMGHEIGHT, 
													  m_fmtdCommon[nZFormat], 
													  D3DMULTISAMPLE_NONE, 
													  &m_pSrcZ);
		if (FAILED(hr))
		{
    		WriteToLog("Src CreateDepthStencilSurface() failed with HResult = %X.\n",hr);
//			RELEASE(pZSurface);
			return false;
		}
/*
	}

	m_pSrcZ = pZSurface;

	if (m_pRefTarget != NULL)
	{
		pZSurface = new CDirectDrawSurface(this);
    
		if (m_dwVersion <= 0x0700)
		{
			DWORD dwSurfaceCrFlags; 

			if (m_pAdapter->Devices[m_pMode->nRefDevice].fHardware)
				dwSurfaceCrFlags = CDDS_SURFACE_VIDEO;
			else
				dwSurfaceCrFlags = CDDS_SURFACE_SYSTEM;

			if (!pZSurface->CreateZBuffer(IMGWIDTH, IMGHEIGHT, (LPDDPIXELFORMAT)&m_pCommonZBufferFormats[nZFormat].ddpfPixelFormat, dwSurfaceCrFlags))
			{
				WriteToLog("Ref CreateZBuffer failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
				RELEASE(pZSurface);
				return false;            
			}
		} 
		else // m_dwVersion >= 0x0800
		{
			hr = m_pRefDevice8->CreateDepthStencilSurface(IMGWIDTH, 
														  IMGHEIGHT, 
														  m_pCommonZBufferFormats[nZFormat].d3dfFormat, 
														  D3DMULTISAMPLE_NONE, 
														  &pZSurface->m_pSurface8);
			if (FAILED(hr))
			{
    			WriteToLog("Ref CreateDepthStencilSurface() failed with HResult = %s.\n",GetHResultString(hr).c_str());
				RELEASE(m_pSrcZ);
				RELEASE(pZSurface);
				return false;
			}
		}

		m_pRefZ = pZSurface;
	}
*/
	return true;
}



bool CClearTest::CopyToTexture(void)
{
    HRESULT hr;
/*
    if (m_dwVersion <= 0x0700)
    {
       POINT Pt = {0,0};

        if (!m_pTextureRT->m_pSrcSurface->Blt(m_pOffSrcSurf,Pt))
	    {
		    WriteToLog("Src Blt() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		    return false;
	    }

	    if (NULL != m_pRefTarget)
	    {
		    if (!m_pTextureRT->m_pRefSurface->Blt(m_pOffRefSurf,Pt))
		    {
			    WriteToLog("Ref Blt() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
			    return false;
		    }
	    }
    }
    else // m_dwVersion >= 0x0800
    {
*/
        CSurface8* pd3ds;
        RECT Rect;

        Rect.left = 0;
	    Rect.top  = 0;
	    Rect.right  = IMGWIDTH;
	    Rect.bottom = IMGHEIGHT;

        hr = m_pTextureRT->GetSurfaceLevel(0, &pd3ds);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            return false;
        }

#ifndef UNDER_XBOX
        hr = m_pSrcDevice8->CopyRects(m_pOffSrcSurf, &Rect, 1, pd3ds, NULL);
#else
        hr = m_pSrcDevice8->CopyRects(m_pOffSrcSurf, NULL, 0, pd3ds, NULL);
#endif

        pd3ds->Release();

        if (FAILED(hr))
        {
            WriteToLog("Src CopyRects() failed with HResult = %X.\n",hr);
            return false;
        }
/*
        if (m_pRefTarget)
        {
            hr = m_pRefDevice8->CopyRects(m_pOffRefSurf->m_pSurface8, &Rect, 1, m_pTextureRT->m_pRefSurface->m_pSurface8, NULL);

            if (FAILED(hr))
            {
                WriteToLog("Ref CopyRects() failed with HResult = %s.\n",GetHResultString(hr).c_str());
                return false;
            }
        }
    }
*/
    return true;
}
