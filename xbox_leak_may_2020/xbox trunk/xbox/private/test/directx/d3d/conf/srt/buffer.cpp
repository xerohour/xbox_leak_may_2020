#include "d3dlocus.h"
#include "cd3dtest.h"
#include "srt.h"

bool CSRT::CreateDestBuffer(bool bCreateZ, CSurface8 *& pSrcZ, CSurface8 *& pRefZ)
{
    HRESULT hr;
    D3DSURFACE_DESC Desc;

/*
    // Create a src surface for rendering
    m_pOffSrcSurf = new CDirectDrawSurface(this);

    if (m_dwVersion <= 0x0700)
    {
    	DWORD dwSurface;

        if (m_pAdapter->Devices[m_pMode->nSrcDevice].fHardware)
		    dwSurface = CDDS_SURFACE_VIDEO | CDDS_SURFACE_OFFSCREEN;
	    else
		    dwSurface = CDDS_SURFACE_SYSTEM | CDDS_SURFACE_OFFSCREEN;

	    if (!m_pOffSrcSurf->CreateSurface(m_pSrcTarget->m_dwWidth, m_pSrcTarget->m_dwHeight, (LPDDPIXELFORMAT)&m_pCommonTextureFormats[m_nTexFormat].ddpfPixelFormat, dwSurface))
	    {
		    WriteToLog("Src CreateSurface failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		    RELEASE(m_pOffSrcSurf);
		    return false;
	    }
    }
    else // m_dwVersion >= 0x0800
    {
*/
        hr = m_pSrcDevice8->CreateRenderTarget(m_pDisplay->GetWidth(), 
                                               m_pDisplay->GetHeight(),
                                               m_fmtrCommon[m_nTexFormat],
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
    	    DWORD dwSurface;

	        if (m_pAdapter->Devices[m_pMode->nRefDevice].fHardware)
		        dwSurface = CDDS_SURFACE_VIDEO | CDDS_SURFACE_OFFSCREEN;
	        else
		        dwSurface = CDDS_SURFACE_SYSTEM | CDDS_SURFACE_OFFSCREEN;

	        if (!m_pOffRefSurf->CreateSurface(m_pRefTarget->m_dwWidth, m_pRefTarget->m_dwHeight, (LPDDPIXELFORMAT)&m_pCommonTextureFormats[m_nTexFormat].ddpfPixelFormat, dwSurface))
	        {
		        WriteToLog("Ref CreateSurface failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		        RELEASE(m_pOffRefSurf);
		        return false;
	        }
        }
        else // m_dwVersion >= 0x0800
        {
            hr = m_pRefDevice8->CreateRenderTarget(m_pRefTarget->m_dwWidth, 
                                                   m_pRefTarget->m_dwHeight,
                                                   m_pCommonTextureFormats[m_nTexFormat].d3dfFormat,
                                                   D3DMULTISAMPLE_NONE,
                                                   true,
                                                   &m_pOffRefSurf->m_pSurface8); 
            if (FAILED(hr))
            {
	            WriteToLog("Ref CreateRenderTarget() failed with HResult = %s.\n",GetHResultString(hr).c_str());
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
    // Create the depth buffer
    if (bCreateZ)
    {
/*
        CDirectDrawSurface * pZSurface = new CDirectDrawSurface(this);

        if (m_dwVersion <= 0x0700)
        {
            DWORD dwSurface; 
        
            if (m_pAdapter->Devices[m_pMode->nSrcDevice].fHardware)
                dwSurface = CDDS_SURFACE_VIDEO;
            else
                dwSurface = CDDS_SURFACE_SYSTEM;

            if (!pZSurface->CreateZBuffer(m_pSrcZBuffer->m_dwWidth, m_pSrcZBuffer->m_dwHeight, (LPDDPIXELFORMAT)&m_pCommonZBufferFormats[m_nZFormat].ddpfPixelFormat, dwSurface))
            {
		        WriteToLog("Src CreateZBuffer failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
                RELEASE(pZSurface);
                return false;            
            }
        } 
        else // m_dwVersion >= 0x0800
        {
*/
            hr = m_pSrcDevice8->CreateDepthStencilSurface(m_pDisplay->GetWidth(), 
                                                          m_pDisplay->GetHeight(), 
                                                          m_fmtdCommon[m_nZFormat], 
                                                          D3DMULTISAMPLE_NONE, 
                                                          &pSrcZ);
            if (FAILED(hr))
            {
    	        WriteToLog("Src CreateDepthStencilSurface() failed with HResult = %X.\n",hr);
//                RELEASE(pZSurface);
                return false;
            }
/*
        }

        pSrcZ = pZSurface;

        if (m_pRefTarget != NULL)
        {
            pZSurface = new CDirectDrawSurface(this);
            
            if (m_dwVersion <= 0x0700)
            {
                DWORD dwSurface; 
        
                if (m_pAdapter->Devices[m_pMode->nRefDevice].fHardware)
                    dwSurface = CDDS_SURFACE_VIDEO;
                else
                    dwSurface = CDDS_SURFACE_SYSTEM;

                if (!pZSurface->CreateZBuffer(m_pRefZBuffer->m_dwWidth, m_pRefZBuffer->m_dwHeight, (LPDDPIXELFORMAT)&m_pCommonZBufferFormats[m_nZFormat].ddpfPixelFormat, dwSurface))
                {
		            WriteToLog("Ref CreateZBuffer failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
                    RELEASE(pZSurface);
                    return false;            
                }
            } 
            else // m_dwVersion >= 0x0800
            {
                hr = m_pRefDevice8->CreateDepthStencilSurface(m_pRefTarget->m_dwWidth, 
                                                              m_pRefTarget->m_dwHeight, 
                                                              m_pCommonZBufferFormats[m_nZFormat].d3dfFormat, 
                                                              D3DMULTISAMPLE_NONE, 
                                                              &pZSurface->m_pSurface8);
                if (FAILED(hr))
                {
    	            WriteToLog("Ref CreateDepthStencilSurface() failed with HResult = %s.\n",GetHResultString(hr).c_str());
                    RELEASE(pSrcZ);
                    RELEASE(pZSurface);
                    return false;
                }
            }

            pRefZ = pZSurface;
        }
*/
    }
    
    return true;
}

bool CSRT::CopyDestBuffer(void)
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
//        CSurface8* pd3ds;
        RECT Rect;

        Rect.left = 0;
	    Rect.top  = 0;
	    Rect.right  = m_RTWidth;
	    Rect.bottom = m_RTHeight;

//        hr = m_pTextureRT->GetSurfaceLevel(0, &pd3ds);
//        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
//            return false;
//        }

        hr = m_pSrcDevice8->CopyRects(m_pOffSrcSurf, &Rect, 1, m_pTextureRTS, NULL);

//        pd3ds->Release();

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
