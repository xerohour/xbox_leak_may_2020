#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include <assert.h>
#include "Lighting.h"

bool Lighting::GenerateVertices()
{
	DWORD	dwWidth = 320;
	DWORD	dwHeight = 280;
	DWORD	dwRows = ((((dwHeight - 20) / m_iStep) / 2) * 2);
	DWORD	dwColumns = (dwWidth - 20) / m_iStep;
	float	fRadius = float((dwHeight - 40) / 2);
	float	fDistance;
    DWORD   nPointVertices = 0;
    DWORD   nLineVertices = 0;
    DWORD   nTriangleVertices = 0;
	
	// Clean up an old array
	if (m_pSourceVertices != NULL)
	{
		delete [] m_pSourceVertices;
		m_pSourceVertices = NULL;
		m_nVertices = 0;
        m_dwRows = 0;
        m_dwColumns = 0;
	}

	if (m_pReferenceVertices != NULL)
	{
		delete [] m_pReferenceVertices;
		m_pReferenceVertices = NULL;
	}

	if (m_pResultVertices != NULL)
	{
		delete [] m_pResultVertices;
		m_pResultVertices = NULL;
	}

#ifdef TESTTYPE_LIGHTING
    if (m_pPointVertices != NULL)
    {
        delete [] m_pPointVertices;
        m_pPointVertices = NULL;
        m_nPointVertices = 0;
    }
    if (m_pTriangleVertices != NULL)
    {
        delete [] m_pPointVertices;
        m_pTriangleVertices = NULL;
        m_nTriangleVertices = 0;
    }
#endif

	// Allocate memory for new vertices
	m_nVertices = dwRows * dwColumns;
	m_pSourceVertices = new NDSVERTEX [m_nVertices];
	if (m_pSourceVertices == NULL)
	{
		WriteToLog("%s: Do not have enough memory to generate vertices\n", m_szTestName);
		return false;
	}
	m_pReferenceVertices = new D3DTLVERTEX [m_nVertices];
	if (m_pReferenceVertices == NULL)
	{
		WriteToLog("%s: Do not have enough memory to generate vertices\n", m_szTestName);
		return false;
	}
#ifdef TESTTYPE_LIGHTING
    m_pResultVertices = new D3DCOLOR [m_nVertices];
	if (m_pResultVertices == NULL)
	{
		WriteToLog("%s: Do not have enough memory to generate vertices\n", m_szTestName);
		return false;
	}

    m_nPointVertices = m_nVertices / 2;
    m_pPointVertices = new NDSVERTEX [m_nPointVertices];
	if (m_pPointVertices == NULL)
	{
		WriteToLog("%s: Do not have enough memory to generate vertices\n", m_szTestName);
		return false;
	}
    m_nTriangleVertices = m_nVertices / 2 * 3;
    m_pTriangleVertices = new NDSVERTEX [m_nTriangleVertices];
	if (m_pTriangleVertices == NULL)
	{
		WriteToLog("%s: Do not have enough memory to generate vertices\n", m_szTestName);
		return false;
	}
#else
	m_pResultVertices = new D3DTLVERTEX [m_nVertices];
	if (m_pResultVertices == NULL)
	{
		WriteToLog("%s: Do not have enough memory to generate vertices\n", m_szTestName);
		return false;
	}
#endif


	for (DWORD i = 0; i < dwRows; i++)
	{
		for (DWORD j = 0; j < dwColumns; j++)
		{
			DWORD index = i*dwColumns + j;

			// XY net of vertices
			m_pSourceVertices[index].x = float(10 + j*m_iStep) - float(320/2);
			m_pSourceVertices[index].y = float(10 + i*m_iStep) - float(280/2);
			
			// Search for vertices inside a cycle and extend them to form
			// a hemi sphere
			fDistance = (float)sqrt(m_pSourceVertices[index].x*m_pSourceVertices[index].x + 
				m_pSourceVertices[index].y*m_pSourceVertices[index].y);

            int k = (j & 0x01) ? 2 : 1;
			
            if (fDistance < fRadius) 
			{
				m_pSourceVertices[index].z = (-1.f)*(float)sqrt(fRadius*fRadius - fDistance*fDistance);

				m_pSourceVertices[index].nx = m_pSourceVertices[index].x / fRadius;
				m_pSourceVertices[index].ny = m_pSourceVertices[index].y / fRadius;
				m_pSourceVertices[index].nz = m_pSourceVertices[index].z / fRadius;

				m_pSourceVertices[index].diffuse = RGBA_MAKE(150/k, 150/k, 150/k, 150/k);
				m_pSourceVertices[index].specular = RGBA_MAKE(100/k, 100/k, 100/k, 255/k);
			}
			else
			{
				m_pSourceVertices[index].z = 0.f;

//				m_pSourceVertices[index].nx = 0.f;
//				m_pSourceVertices[index].ny = 0.f;
//				m_pSourceVertices[index].nz = -1.f;
                m_pSourceVertices[index].nx = m_pSourceVertices[index].x / fRadius;
                m_pSourceVertices[index].ny = m_pSourceVertices[index].y / fRadius;
				m_pSourceVertices[index].nz = -m_pSourceVertices[index].z / fRadius;
				m_pSourceVertices[index].diffuse = RGBA_MAKE(150/k, 150/k, 150/k, 150/k);
				m_pSourceVertices[index].specular = RGBA_MAKE(100/k, 100/k, 100/k, 255/k);
			}
#ifdef TESTTYPE_LIGHTING
            if ((i & 0x01) == 0)
            {
                // Point vertices
                memcpy(&m_pPointVertices[nPointVertices], &m_pSourceVertices[index], sizeof(NDSVERTEX));
                nPointVertices++;
            }
            else
            {
                // Triangle vertices
                memcpy(&m_pTriangleVertices[nTriangleVertices],   &m_pSourceVertices[index], sizeof(NDSVERTEX));
                memcpy(&m_pTriangleVertices[nTriangleVertices+1], &m_pSourceVertices[index], sizeof(NDSVERTEX));
                memcpy(&m_pTriangleVertices[nTriangleVertices+2], &m_pSourceVertices[index], sizeof(NDSVERTEX));
                m_pTriangleVertices[nTriangleVertices    ].x -= 0.4f;
                m_pTriangleVertices[nTriangleVertices    ].y -= 0.4f;
                m_pTriangleVertices[nTriangleVertices + 1].y += 0.4f;
                m_pTriangleVertices[nTriangleVertices + 2].x += 0.4f;
                m_pTriangleVertices[nTriangleVertices + 2].y -= 0.4f;
                nTriangleVertices += 3;
            }
#endif
		}
	}
#ifdef TESTTYPE_LIGHTING
    assert(m_nPointVertices == nPointVertices);
    assert(m_nTriangleVertices == nTriangleVertices);
#endif

	m_dwRows = dwRows;
	m_dwColumns = dwColumns;
	return true;
}

bool Lighting::InitializeVertexProcessing()
{
#ifdef TESTTYPE_LIGHTING
    return true;
#endif

	D3DVERTEX*	vertices;	
	DWORD		dwSize = 0;
//    DWORD       dwSrcFlags = D3DVBCAPS_DONOTCLIP | D3DVBCAPS_WRITEONLY | D3DVBCAPS_SYSTEMMEMORY;
//    DWORD       dwDstFlags = D3DVBCAPS_DONOTCLIP | D3DVBCAPS_WRITEONLY;

//    if (IID_IDirect3DRGBDevice == m_pAdapter->Devices[m_pMode->nSrcDevice].Guid ||
//        IID_IDirect3DRefDevice == m_pAdapter->Devices[m_pMode->nSrcDevice].Guid)
//        dwDstFlags |= D3DVBCAPS_SYSTEMMEMORY;

	// Create vertex buffers
	m_pSourceVertexBuffer = CreateVertexBuffer(m_pDevice, m_pSourceVertices, m_nVertices * sizeof(NDSVERTEX), 0, D3DFVF_NDSVERTEX);
	if (m_pSourceVertexBuffer == NULL)
	{
		WriteToLog("LightTest: Source vertex buffer was not created\n");
		return false;
	}
//	m_pOptimizedSourceVertexBuffer = CreateVertexBuffer(D3DFVF_NDSVERTEX, m_nVertices, dwSrcFlags, false);
//	if (m_pOptimizedSourceVertexBuffer == NULL)
//	{
//		WriteToLog("LightTest: Optimized source vertex buffer was not created\n");
//		return false;
//	}
	
//	m_pDestinationVertexBuffer = CreateVertexBuffer(D3DFVF_TLVERTEX, m_nVertices, dwDstFlags, false);
//	if (m_pDestinationVertexBuffer == NULL)
//	{
//		WriteToLog("LightTest: Destination vertex buffer for source device was not created\n");
//		return false;
//	}
	
	// Copy vertices from the sphere shepa to the source vertex buffer
//	if (!m_pSourceVertexBuffer->Lock((void**)&vertices, &dwSize))
//	{
//		WriteToLog("LightTest: Source vertex buffer too small\n");
//		return false;
//	}
//	memcpy(vertices, m_pSourceVertices, sizeof(NDSVERTEX)*m_nVertices);
//	m_pSourceVertexBuffer->Unlock();
//	if (!m_pOptimizedSourceVertexBuffer->Lock((void**)&vertices, &dwSize))
//	{
//		WriteToLog("LightTest: Couldn't lock vertex buffer\n");
//		return false;
//	}
//	memcpy(vertices, m_pSourceVertices, sizeof(NDSVERTEX)*m_nVertices);
//	m_pOptimizedSourceVertexBuffer->Unlock();
//    m_pOptimizedSourceVertexBuffer->Optimize(false);

    // Fill strided format structure
//    ZeroMemory(&m_sStridedVertexData, sizeof(m_sStridedVertexData));
//    m_sStridedVertexData.position.lpvData = m_pSourceVertices;
//    m_sStridedVertexData.position.dwStride = sizeof(NDSVERTEX);
//    m_sStridedVertexData.normal.lpvData = (BYTE*)m_pSourceVertices + 3*sizeof(D3DVALUE);
//    m_sStridedVertexData.normal.dwStride = sizeof(NDSVERTEX);
//    m_sStridedVertexData.diffuse.lpvData = (BYTE*)m_pSourceVertices + 6*sizeof(D3DVALUE);
//    m_sStridedVertexData.diffuse.dwStride = sizeof(NDSVERTEX);
//    m_sStridedVertexData.specular.lpvData = (BYTE*)m_pSourceVertices + 6*sizeof(D3DVALUE) + sizeof(DWORD);
//    m_sStridedVertexData.specular.dwStride = sizeof(NDSVERTEX);
    
    return true;
}

bool Lighting::ProcessSourceVertices()
{
    return false;
/*
    if (0x0800 == m_dwVersion)
    {
        switch(GetAPI())
        {
        case RP_DPVB_PV:
        case RP_DIPVB_PV:
            if (!SetStreamSource(0, m_pSourceVertexBuffer, NULL, sizeof(NDSVERTEX), CD3D_SRC) ||
                !SetVertexShader(D3DFVF_NDSVERTEX, 0) ||
                !ProcessVertices(0, 0, m_pSourceVertexBuffer->m_dwVertexCount, m_pDestinationVertexBuffer, 0))
            {
		        WriteToLog("%s: Can't process vertices for source device\n", m_szTestName);
		        Fail();
		        EndTestCase();
		        return false;
            }
            break;
        case RP_DPVB_OPT_PV:
        case RP_DIPVB_OPT_PV:
            if (!SetStreamSource(0, m_pOptimizedSourceVertexBuffer, NULL, sizeof(NDSVERTEX), CD3D_SRC) ||
                !SetVertexShader(D3DFVF_NDSVERTEX, 0) ||
                !ProcessVertices(0, 0, m_pSourceVertexBuffer->m_dwVertexCount, m_pDestinationVertexBuffer, 0))
            {
		        WriteToLog("%s: Can't process vertices for source device\n", m_szTestName);
		        Fail();
		        EndTestCase();
		        return false;
            }
            break;

//        case RP_DPVB_PVS:
//        case RP_DIPVB_PVS:

//            SetVertexShader(D3DFVF_NDSVERTEX, 0);
//            ProcessVertices(0, 0, m_pSourceVertexBuffer->m_dwVertexCount, m_pDestinationVertexBuffer, 0);
//            break;

        default:
            WriteToLog("%s: Incorrect API for ProcessSourceVertices\n", m_szTestName);
            Fail();
            EndTestCase();
            return false;
        }
    }
    else
    {
	    switch(GetAPI())
        {
        case RP_DPVB_PV:
        case RP_DIPVB_PV:
            if (!m_pDestinationVertexBuffer->ProcessVertices(D3DVOP_TRANSFORM | D3DVOP_LIGHT, 0, m_nVertices, m_pSourceVertexBuffer, 0, false, 0))
	        {
		        WriteToLog("%s: Can't process vertices for source device\n", m_szTestName);
		        Fail();
		        EndTestCase();
		        return false;
	        }
            break;
        case RP_DPVB_OPT_PV:
        case RP_DIPVB_OPT_PV:
            if (!m_pDestinationVertexBuffer->ProcessVertices(D3DVOP_TRANSFORM | D3DVOP_LIGHT, 0, m_nVertices, m_pOptimizedSourceVertexBuffer, 0, false, 0))
	        {
		        WriteToLog("%s: Can't process vertices for source device\n", m_szTestName);
		        Fail();
		        EndTestCase();
		        return false;
	        }
            break;
        case RP_DPVB_PVS:
        case RP_DIPVB_PVS:
            if (!m_pDestinationVertexBuffer->ProcessVerticesStrided(D3DVOP_TRANSFORM | D3DVOP_LIGHT, 
                    0, m_nVertices, &m_sStridedVertexData, D3DFVF_NDSVERTEX, false, 0))
	        {
		        WriteToLog("%s: Can't process vertices for source device\n", m_szTestName);
		        Fail();
		        EndTestCase();
		        return false;
	        }
            break;
        default:
            WriteToLog("%s: Incorrect API for ProcessSourceVertices\n", m_szTestName);
            Fail();
            EndTestCase();
            return false;
        }
    }

	D3DTLVERTEX*    pVertices;
    DWORD           dwSize = 0;
    // Copy processed vertices to the output buffer
	if (!m_pDestinationVertexBuffer->Lock((void**)&pVertices, &dwSize))
	{
		WriteToLog("LightTest: Can't lock vertex buffer with processed vertices\n");
		return false;
	}
    memcpy(m_pResultVertices, pVertices, sizeof(D3DTLVERTEX)*m_nVertices);   
    m_pDestinationVertexBuffer->Unlock();

	return true;
*/
}



bool Lighting::GetResultVertices()
{
#ifdef TESTTYPE_LIGHTING
    CSurface8*      pd3ds;
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT  d3dlr;
	void*	pBuffer;
	DWORD	i;
    HRESULT hr;
    
    hr = m_pDevice->GetRenderTarget(&pd3ds);
    if (FAILED(hr)) {
		WriteToLog("IDirect3DDevice8::GetRenderTarget failed\n");
		return false;
    }

    hr = pd3ds->GetDesc(&d3dsd);
    if (FAILED(hr)) {
		WriteToLog("IDirect3DSurface8::GetDesc failed\n");
		return false;
    }

    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
    if (FAILED(hr)) {
		WriteToLog("IDirect3DSurface8::LockRect failed\n");
		return false;
    }

//	pBuffer = m_pSrcTarget->Lock();
//	if (pBuffer ==NULL) 
//	{
//		WriteToLog("LightTest: Can't lock target surface\n");
//		return false;
//	}


	for (i = 0; i < m_nVertices; i++)
	{
		D3DTLVERTEX*	vert = &m_pReferenceVertices[i];
		DWORD			color;
		BYTE			r = 0;
		BYTE			g = 0;
		BYTE			b = 0;
        ARGBPIXELDESC   pixd;

				
		for (int k = -1; k <= 1; k++)
		{
			for (int l = -1; l <= 1; l++)
			{
				switch(FormatToBitDepth(d3dsd.Format))//m_pSrcTarget->m_dwBPP) 
				{
				case 8:
					{
					
					}
					break;
				case 16:
					{
						WORD*	pWBuffer = (WORD*)d3dlr.pBits;//pBuffer;
						int		index = ((int)vert->sy+k)*d3dlr.Pitch/2 + ((int)vert->sx+l);
						color = (DWORD)pWBuffer[index];
					}
					break;
				case 24:
					{
						BYTE*	pBBuffer = (BYTE*)d3dlr.pBits;//pBuffer;
						int		index = (int(vert->sy)+k)*d3dlr.Pitch + (int(vert->sx)+l)*3;
						color = (*((DWORD*)(&pBBuffer[index]))) & 0x00ffffff;
//                      color = ( (((DWORD)(pBBuffer[index+2])) << 16) /*|| (((DWORD)pBBuffer[index+1]) << 8) || ((DWORD)pBBuffer[index]) */);
					}
					break;
				case 32:
					{
						DWORD*	pDWBuffer = (DWORD*)d3dlr.pBits;//pBuffer;
						int		index = ((int)vert->sy+k)*d3dlr.Pitch/4 + ((int)vert->sx+l);
						color = pDWBuffer[index];
					}
					break;
	
				}

                // ##REVIEW: The shift values stored in m_pSrcTarget may not be the same as the ones calculated by GetARGBPixelDesc
                GetARGBPixelDesc(d3dsd.Format, &pixd);

				if (pixd.dwRShift > 0)
					r += (BYTE)((color >> pixd.dwRShift) & pixd.dwRMask);
				else
					r += (BYTE)((color << (-(int)pixd.dwRShift)) & pixd.dwRMask);

				if (pixd.dwGShift > 0)
					g += (BYTE)((color >> pixd.dwGShift) & pixd.dwGMask);
				else
					g += (BYTE)((color << (-(int)pixd.dwGShift)) & pixd.dwGMask);
						
				if (pixd.dwBShift > 0)
					b += (BYTE)((color >> pixd.dwBShift) & pixd.dwBMask);
				else
					b += (BYTE)((color << (-(int)pixd.dwBShift)) & pixd.dwBMask);

			}
		}
				
		m_pResultVertices[i] = RGB_MAKE(r, g, b);
	}
	
//	if (!m_pSrcTarget->Unlock())
    hr = pd3ds->UnlockRect();
    if (FAILED(hr))
	{
		WriteToLog("LightTest: Can't unlock target surface\n");
		return false;
	}

    pd3ds->Release();

#endif
	return true;
}

bool Lighting::GetCompareResult(float fColorVariation, float fColorRange, int iMinPixels)
{
	DWORD	        i;
    ARGBPIXELDESC   pixd;
    CSurface8*      pd3ds;
    D3DSURFACE_DESC d3dsd;
    HRESULT hr;
    
    hr = m_pDevice->GetRenderTarget(&pd3ds);
    if (FAILED(hr)) {
		WriteToLog("IDirect3DDevice8::GetRenderTarget failed\n");
		return false;
    }

    hr = pd3ds->GetDesc(&d3dsd);
    if (FAILED(hr)) {
		WriteToLog("IDirect3DSurface8::GetDesc failed\n");
		return false;
    }

    pd3ds->Release();

    GetARGBPixelDesc(d3dsd.Format, &pixd);

#ifdef TESTTYPE_LIGHTING
	for (i = 0; i < m_nVertices; i++)
	{
		BYTE	bSourceRed = (BYTE)RGBA_GETRED(m_pResultVertices[i]);
		BYTE	bSourceGreen = (BYTE)RGBA_GETGREEN(m_pResultVertices[i]);
		BYTE	bSourceBlue = (BYTE)RGBA_GETBLUE(m_pResultVertices[i]);

		WORD	wRefRed =	(WORD)RGBA_GETRED(m_pReferenceVertices[i].color);
		WORD	wRefGreen = (WORD)RGBA_GETGREEN(m_pReferenceVertices[i].color);
		WORD	wRefBlue =	(WORD)RGBA_GETBLUE(m_pReferenceVertices[i].color);
		WORD	wRefAlpha = (WORD)RGBA_GETALPHA(m_pReferenceVertices[i].color);

        if (m_ModelProperties.bSpecular)
        {
            wRefRed += (WORD)RGBA_GETRED(m_pReferenceVertices[i].specular);
            wRefGreen += (WORD)RGBA_GETGREEN(m_pReferenceVertices[i].specular);
            wRefBlue += (WORD)RGBA_GETBLUE(m_pReferenceVertices[i].specular);
        }

		wRefRed =   (WORD)(((wRefRed < 255)   ? wRefRed   : 255));
		wRefGreen = (WORD)(((wRefGreen < 255) ? wRefGreen : 255));
		wRefBlue =  (WORD)(((wRefBlue < 255)  ? wRefBlue  : 255));

		// Apply alpha to the reference vertices
		wRefRed =   (WORD)(wRefRed * wRefAlpha / 255);
		wRefGreen = (WORD)(wRefGreen * wRefAlpha / 255);
		wRefBlue =  (WORD)(wRefBlue * wRefAlpha / 255);
	
		BYTE	bRefRed = (BYTE)(((wRefRed < 255) ? (BYTE)wRefRed : 255) & pixd.dwRMask);
		BYTE	bRefGreen = (BYTE)(((wRefGreen < 255) ? (BYTE)wRefGreen : 255) & pixd.dwGMask);
		BYTE	bRefBlue = (BYTE)(((wRefBlue < 255) ? (BYTE)wRefBlue : 255) & pixd.dwBMask);

		if (abs(int(bSourceRed) - int(bRefRed)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with red component difference %d\n", m_dwCurrentTestNumber, i, abs(int(bSourceRed) - int(bRefRed)));
            WriteToLog("\tSource red:   %d;\tReference red:   %d", bSourceRed, bRefRed);
            WriteToLog("\tSource green: %d;\tReference green: %d", bSourceGreen, bRefGreen);
            WriteToLog("\tSource blue:  %d;\tReference blue:  %d", bSourceBlue, bRefBlue);
			return false;
		}
		if (abs(int(bSourceGreen) - int(bRefGreen)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with green component difference %d\n", m_dwCurrentTestNumber, i, abs(int(bSourceGreen) - int(bRefGreen)));
            WriteToLog("\tSource red:   %d;\tReference red:   %d", bSourceRed, bRefRed);
            WriteToLog("\tSource green: %d;\tReference green: %d", bSourceGreen, bRefGreen);
            WriteToLog("\tSource blue:  %d;\tReference blue:  %d", bSourceBlue, bRefBlue);
			return false;
		}
		if (abs(int(bSourceBlue) - int(bRefBlue)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with blue component difference %d\n", m_dwCurrentTestNumber, i, abs(int(bSourceBlue) - int(bRefBlue)));
            WriteToLog("\tSource red:   %d;\tReference red:   %d", bSourceRed, bRefRed);
            WriteToLog("\tSource green: %d;\tReference green: %d", bSourceGreen, bRefGreen);
            WriteToLog("\tSource blue:  %d;\tReference blue:  %d", bSourceBlue, bRefBlue);
			return false;
		}
	}

#else

    for (i = 0; i < m_nVertices; i++)
	{
		BYTE	bSrcDiffuseRed = (BYTE)RGBA_GETRED(m_pResultVertices[i].color);
		BYTE	bSrcDiffuseGreen = (BYTE)RGBA_GETGREEN(m_pResultVertices[i].color);
		BYTE	bSrcDiffuseBlue = (BYTE)RGBA_GETBLUE(m_pResultVertices[i].color);
		BYTE	bSrcSpecularRed = (BYTE)RGBA_GETRED(m_pResultVertices[i].specular);
		BYTE	bSrcSpecularGreen = (BYTE)RGBA_GETGREEN(m_pResultVertices[i].specular);
		BYTE	bSrcSpecularBlue = (BYTE)RGBA_GETBLUE(m_pResultVertices[i].specular);

		BYTE	bRefDiffuseRed = (BYTE)RGBA_GETRED(m_pReferenceVertices[i].color);
		BYTE	bRefDiffuseGreen = (BYTE)RGBA_GETGREEN(m_pReferenceVertices[i].color);
		BYTE	bRefDiffuseBlue = (BYTE)RGBA_GETBLUE(m_pReferenceVertices[i].color);
		BYTE	bRefSpecularRed = (BYTE)RGBA_GETRED(m_pReferenceVertices[i].specular);
		BYTE	bRefSpecularGreen = (BYTE)RGBA_GETGREEN(m_pReferenceVertices[i].specular);
		BYTE	bRefSpecularBlue = (BYTE)RGBA_GETBLUE(m_pReferenceVertices[i].specular);

		if (abs(int(bSrcDiffuseRed) - int(bRefDiffuseRed)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with diffuse red component difference %d\n", 
                m_dwCurrentTestNumber, i, abs(int(bSrcDiffuseRed) - int(bRefDiffuseRed)));
			return false;
		}
		if (abs(int(bSrcDiffuseGreen) - int(bRefDiffuseGreen)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with diffuse green component difference %d\n", 
                m_dwCurrentTestNumber, i, abs(int(bSrcDiffuseGreen) - int(bRefDiffuseGreen)));
			return false;
		}
		if (abs(int(bSrcDiffuseBlue) - int(bRefDiffuseBlue)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with diffuse blue component difference %d\n", 
                m_dwCurrentTestNumber, i, abs(int(bSrcDiffuseBlue) - int(bRefDiffuseBlue)));
			return false;
		}

		if (abs(int(bSrcSpecularRed) - int(bRefSpecularRed)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with specular red component difference %d\n", 
                m_dwCurrentTestNumber, i, abs(int(bSrcSpecularRed) - int(bRefSpecularRed)));
			return false;
		}
		if (abs(int(bSrcSpecularGreen) - int(bRefSpecularGreen)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with specular green component difference %d\n", 
                m_dwCurrentTestNumber, i, abs(int(bSrcSpecularGreen) - int(bRefSpecularGreen)));
			return false;
		}
		if (abs(int(bSrcSpecularBlue) - int(bRefSpecularBlue)) > m_iColorTollerance )
		{
			WriteToLog("LightTest: Test %d failed on vertex %d with specular blue component difference %d\n", 
                m_dwCurrentTestNumber, i, abs(int(bSrcSpecularBlue) - int(bRefSpecularBlue)));
			return false;
		}
	}
#endif

	return true;
}

bool Lighting::ProcessReferenceVertices7()
{
	D3DMATRIX	Mv;					// View matrix
	D3DMATRIX	Mwv;				// Modelview matrix
	D3DMATRIX	Mwvt;				// Inverse transposed modelview matrix
	D3DMATRIX	Mcur;				// Full matrix for vertex transformation
	
	// Camera space values
	D3DVECTOR	V;					// Vertex position
	D3DVECTOR	N;					// Normal
	D3DVECTOR	Lp[LT_LIGHTS];		// Positions of light sources
	D3DVECTOR	Ld[LT_LIGHTS];		// Directions of light sources
	D3DVECTOR	Lds[LT_LIGHTS];		// Spot directions of light sources
	D3DVALUE	att[LT_LIGHTS];		// Attenuations of light sources
	D3DVALUE	spot[LT_LIGHTS];	// Spot for light sources
	D3DVALUE	dot[LT_LIGHTS];		// Dot products for light sources
	D3DVALUE	rsa, gsa, bsa;		// Source ambient colors
	D3DVALUE	rsd, gsd, bsd, asd;	// Source diffufe colors
	D3DVALUE	rss, gss, bss;		// Source specular colors
	D3DVALUE	rse, gse, bse;		// Source specular colors
	D3DVALUE	rr, gr, br;			// Result colors
	int			ro, go, bo, ao;		// Output colors
	int			i;

	// Calculate full matrices
	Mv = m_SceneProperties.ViewMatrix;
	Mwv = MatrixMult(Mv, m_SceneProperties.WorldMatrix);
	Mcur = MatrixMult(m_SceneProperties.ProjectionMatrix, Mwv);
	Mwvt = MatrixInverse(Mwv);
	Mwvt = MatrixTranspose(Mwvt);


	for (i = 0; i < LT_LIGHTS; i++)
	{
		// Light position for POINT and SPOT lights
		if (m_LightProperties[i].dwLightType == D3DLIGHT_POINT || m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
		{
			Lp[i].x = Mv._11*m_LightProperties[i].dvLightPosition.x + Mv._21*m_LightProperties[i].dvLightPosition.y + Mv._31*m_LightProperties[i].dvLightPosition.z + Mv._41;
			Lp[i].y = Mv._12*m_LightProperties[i].dvLightPosition.x + Mv._22*m_LightProperties[i].dvLightPosition.y + Mv._32*m_LightProperties[i].dvLightPosition.z + Mv._42;
			Lp[i].z = Mv._13*m_LightProperties[i].dvLightPosition.x + Mv._23*m_LightProperties[i].dvLightPosition.y + Mv._33*m_LightProperties[i].dvLightPosition.z + Mv._43;
		}
		
		// Camera space light directions could be calculated here
		// if light type DIRECTIONAL
		if (m_LightProperties[i].dwLightType == D3DLIGHT_DIRECTIONAL)
		{
			Ld[i].x = (Mv._11*m_LightProperties[i].dvLightDirection.x + Mv._21*m_LightProperties[i].dvLightDirection.y + Mv._31*m_LightProperties[i].dvLightDirection.z)*(-1.f);
			Ld[i].y = (Mv._12*m_LightProperties[i].dvLightDirection.x + Mv._22*m_LightProperties[i].dvLightDirection.y + Mv._32*m_LightProperties[i].dvLightDirection.z)*(-1.f);
			Ld[i].z = (Mv._13*m_LightProperties[i].dvLightDirection.x + Mv._23*m_LightProperties[i].dvLightDirection.y + Mv._33*m_LightProperties[i].dvLightDirection.z)*(-1.f);
			Ld[i] = Normalize(Ld[i]);
		}

		// Camera space direction of a spot of the SPOT light. Needed to calculate rho later
		if (m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
		{
			Lds[i].x = -(Mv._11*m_LightProperties[i].dvLightDirection.x + Mv._21*m_LightProperties[i].dvLightDirection.y + Mv._31*m_LightProperties[i].dvLightDirection.z);
			Lds[i].y = -(Mv._12*m_LightProperties[i].dvLightDirection.x + Mv._22*m_LightProperties[i].dvLightDirection.y + Mv._32*m_LightProperties[i].dvLightDirection.z);
			Lds[i].z = -(Mv._13*m_LightProperties[i].dvLightDirection.x + Mv._23*m_LightProperties[i].dvLightDirection.y + Mv._33*m_LightProperties[i].dvLightDirection.z);
			Lds[i] = Normalize(Lds[i]);
		}
	}

	// Vertex processing
	for (DWORD v = 0; v < m_nVertices; v++)
	{
		NDSVERTEX		*iv = &m_pSourceVertices[v];
		D3DTLVERTEX		*ov = &m_pReferenceVertices[v];
		
		// Coordinates transformation
		ov->sx  = Mcur._11*iv->x + Mcur._21*iv->y + Mcur._31*iv->z + Mcur._41;
		ov->sy  = Mcur._12*iv->x + Mcur._22*iv->y + Mcur._32*iv->z + Mcur._42;
		ov->rhw = Mcur._14*iv->x + Mcur._24*iv->y + Mcur._34*iv->z + Mcur._44;
		
		// W division
		ov->rhw = 1.f / ov->rhw;
		ov->sx = ov->sx * ov->rhw;
		ov->sy = ov->sy * ov->rhw;

		// Viewport transformation
		ov->sx = (0.5f + ov->sx*0.5f) * m_dwWidth;
		ov->sy = (0.5f - ov->sy*0.5f) * m_dwHeight;

		// We do not need real Z transformation for the test.
		// It could be initialized with some value in [0,1] range
		ov->sz = 0.5f;

		if (m_iVertexNumber >= 0 && (int)v != m_iVertexNumber)
		{
			ov->dcColor = 0;
			ov->dcSpecular = 0;
			continue;
		}
		
		// Vertex position in camera space		
		V.x = Mwv._11*iv->x + Mwv._21*iv->y + Mwv._31*iv->z + Mwv._41;
		V.y = Mwv._12*iv->x + Mwv._22*iv->y + Mwv._32*iv->z + Mwv._42;
		V.z = Mwv._13*iv->x + Mwv._23*iv->y + Mwv._33*iv->z + Mwv._43;

		// Calculating normal in camera space
		N.x = Mwvt._11*iv->nx + Mwvt._21*iv->ny + Mwvt._31*iv->nz;
		N.y = Mwvt._12*iv->nx + Mwvt._22*iv->ny + Mwvt._32*iv->nz;
		N.z = Mwvt._13*iv->nx + Mwvt._23*iv->ny + Mwvt._33*iv->nz;
		if (m_ModelProperties.bNormalizeNormals)
			N = Normalize(N);

		// Source colors - Ambient
		if (m_ModelProperties.AmbientSource == D3DMCS_MATERIAL)
		{
			rsa = m_ModelProperties.Material.Ambient.r * 255.f;
			gsa = m_ModelProperties.Material.Ambient.g * 255.f;
			bsa = m_ModelProperties.Material.Ambient.b * 255.f;
		}
		else if (m_ModelProperties.AmbientSource == D3DMCS_COLOR1)
		{
			rsa = (float)RGBA_GETRED(iv->diffuse);
			gsa = (float)RGBA_GETGREEN(iv->diffuse);
			bsa = (float)RGBA_GETBLUE(iv->diffuse);
		}
		else if (m_ModelProperties.AmbientSource == D3DMCS_COLOR2)
		{
			rsa = (float)RGBA_GETRED(iv->specular);
			gsa = (float)RGBA_GETGREEN(iv->specular);
			bsa = (float)RGBA_GETBLUE(iv->specular);
		}
		
		// Diffuse component
		if (m_ModelProperties.DiffuseSource == D3DMCS_MATERIAL)
		{
			rsd = m_ModelProperties.Material.Diffuse.r * 255.f;
			gsd = m_ModelProperties.Material.Diffuse.g * 255.f;
			bsd = m_ModelProperties.Material.Diffuse.b * 255.f;
			asd = m_ModelProperties.Material.Diffuse.a * 255.f;
		}
		else if (m_ModelProperties.DiffuseSource == D3DMCS_COLOR1)
		{
			rsd = (float)RGBA_GETRED(iv->diffuse);
			gsd = (float)RGBA_GETGREEN(iv->diffuse);
			bsd = (float)RGBA_GETBLUE(iv->diffuse);
			asd = (float)RGBA_GETALPHA(iv->diffuse);
		}
		else if (m_ModelProperties.DiffuseSource == D3DMCS_COLOR2)
		{
			rsd = (float)RGBA_GETRED(iv->specular);
			gsd = (float)RGBA_GETGREEN(iv->specular);
			bsd = (float)RGBA_GETBLUE(iv->specular);
			asd = (float)RGBA_GETALPHA(iv->specular);
		}
		
/*		// Diffuse alpha component
		if (m_ModelProperties.AlphaSource == D3DMCS_MATERIAL)
			asd = m_ModelProperties.Material.diffuse.a * 255.f;
		else if (m_ModelProperties.AlphaSource == D3DMCS_COLOR1)
			asd = RGBA_GETALPHA(iv->diffuse);
		else if (m_ModelProperties.AlphaSource == D3DMCS_COLOR2)
			asd = RGBA_GETALPHA(iv->specular);
*/		
		// Speculat component
		if (m_ModelProperties.SpecularSource == D3DMCS_MATERIAL)
		{
			rss = m_ModelProperties.Material.Specular.r * 255.f;
			gss = m_ModelProperties.Material.Specular.g * 255.f;
			bss = m_ModelProperties.Material.Specular.b * 255.f;
		}
		else if (m_ModelProperties.SpecularSource == D3DMCS_COLOR1)
		{
			rss = (float)RGBA_GETRED(iv->diffuse);
			gss = (float)RGBA_GETGREEN(iv->diffuse);
			bss = (float)RGBA_GETBLUE(iv->diffuse);
		}
		else if (m_ModelProperties.SpecularSource == D3DMCS_COLOR2)
		{
			rss = (float)RGBA_GETRED(iv->specular);
			gss = (float)RGBA_GETGREEN(iv->specular);
			bss = (float)RGBA_GETBLUE(iv->specular);
		}
		
		// Emissive component
		if (m_ModelProperties.EmissiveSource == D3DMCS_MATERIAL)
		{
			rse = m_ModelProperties.Material.Emissive.r * 255.f;
			gse = m_ModelProperties.Material.Emissive.g * 255.f;
			bse = m_ModelProperties.Material.Emissive.b * 255.f;
		}
		else if (m_ModelProperties.EmissiveSource == D3DMCS_COLOR1)
		{
			rse = (float)RGBA_GETRED(iv->diffuse);
			gse = (float)RGBA_GETGREEN(iv->diffuse);
			bse = (float)RGBA_GETBLUE(iv->diffuse);
		}
		else if (m_ModelProperties.EmissiveSource == D3DMCS_COLOR2)
		{
			rse = (float)RGBA_GETRED(iv->specular);
			gse = (float)RGBA_GETGREEN(iv->specular);
			bse = (float)RGBA_GETBLUE(iv->specular);
		}
		
		
		// Ambient component
		rr = rsa * m_ModelProperties.dcvAmbientColor.r;
		gr = gsa * m_ModelProperties.dcvAmbientColor.g;
		br = bsa * m_ModelProperties.dcvAmbientColor.b;

		// Emissive component
		rr += rse;
		gr += gse;
		br += bse;
		
		for (i = 0; i < LT_LIGHTS; i++)
		{
			if (m_LightProperties[i].bLightActive)
			{
				// Light direction for POINT and SPOT light types
				if (m_LightProperties[i].dwLightType == D3DLIGHT_POINT || m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
				{
					Ld[i].x = Lp[i].x - V.x;
					Ld[i].y = Lp[i].y - V.y;
					Ld[i].z = Lp[i].z - V.z;
					Ld[i] = Normalize(Ld[i]);
				}
				
				// Attenuation 
				if (m_LightProperties[i].dwLightType == D3DLIGHT_POINT || m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
				{
					D3DVECTOR	D = (D3DXVECTOR3)Lp[i] - (D3DXVECTOR3)V;
					D3DVALUE	d = (float)sqrt(D.x*D.x + D.y*D.y + D.z*D.z);

					if ( (d > 0.f) && (d < m_LightProperties[i].dvRange) )
					{
						att[i] = 1.f / (m_LightProperties[i].dvAttenuation0 + m_LightProperties[i].dvAttenuation1*d + m_LightProperties[i].dvAttenuation2*d*d);
//						if (att[i] > 1.f)	att[i] = 1.f;
					}
					else
						att[i] = 0.f;
				}
				else
					att[i] = 1.f;

				// Spot
				if (m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
				{
					D3DVECTOR	VLp = Normalize((D3DXVECTOR3)Lp[i] - (D3DXVECTOR3)V);
					D3DVALUE	rho = DotProduct(Lds[i], VLp);
					D3DVALUE	cp2 = (float)cos(m_LightProperties[i].dvPhi*0.5f);	// cos(phi/2)
					D3DVALUE	ct2 = (float)cos(m_LightProperties[i].dvTheta*0.5f);	// cos(theta/2)

					if (rho > ct2)			spot[i] = 1.f;
					else if (rho <= cp2)	spot[i] = 0.f;
					else					spot[i] = (float)pow((rho - cp2)/(ct2 - cp2), m_LightProperties[i].dvFalloff);
				}
				else
					spot[i] = 1.f;
				

				D3DVALUE	k = att[i] * spot[i];

				// Light source ambient component
				if (m_LightProperties[i].bLightActive)
				{
					rr += rsa * k * m_LightProperties[i].dcvAmbient.r;
					gr += gsa * k * m_LightProperties[i].dcvAmbient.g;
					br += bsa * k * m_LightProperties[i].dcvAmbient.b;
				}

				// Dot products
				dot[i] = DotProduct(N, Ld[i]);

				if (dot[i] > 0.f)
				{

					rr += rsd * k * dot[i] * m_LightProperties[i].dcvDiffuse.r;
					gr += gsd * k * dot[i] * m_LightProperties[i].dcvDiffuse.g;
					br += bsd * k * dot[i] * m_LightProperties[i].dcvDiffuse.b;
				}
			}
		}
		
		// Diffuse color 
		ro = (int)(rr);
		go = (int)(gr);
		bo = (int)(br);
		ao = (int)(asd);

		if (ro < 0)		ro = 0;
		if (go < 0)		go = 0;
		if (bo < 0)		bo = 0;
		if (ao < 0)		ao = 0;

		if (ro > 255)	ro = 255;
		if (go > 255)	go = 255;
		if (bo > 255)	bo = 255;
		if (ao > 255)	ao = 255;

		ov->dcColor = RGBA_MAKE(ro, go, bo, ao);

		
		// Specular Color
		if (m_ModelProperties.bSpecular)
		{
			D3DVECTOR	VPe;
			if (m_ModelProperties.bLocalViewer)
				VPe = Normalize(cD3DVECTOR(-V.x, -V.y, -V.z));
			else
				VPe = cD3DVECTOR(0.f,0.f,-1.f);

			rr = gr = br = 0;
			for (i = 0; i < LT_LIGHTS; i++)	
			{
				if (m_LightProperties[i].bLightActive && (dot[i] > 0.f))
				{
					D3DVECTOR	h;
					D3DVALUE	sdot;

					h = Normalize((D3DXVECTOR3)VPe + (D3DXVECTOR3)Ld[i]);
					sdot = DotProduct(N,h);

					D3DVALUE	k = att[i] * spot[i] * (float)pow(sdot, m_ModelProperties.Material.Power);
					if (k > 0.f)
					{
						rr += rss * k * m_LightProperties[i].dcvSpecular.r;
						gr += gss * k * m_LightProperties[i].dcvSpecular.g;
						br += bss * k * m_LightProperties[i].dcvSpecular.b;
					}
				}
			}

			ro = (int)(rr);
			go = (int)(gr);
			bo = (int)(br);
			ao = 255;

			if (ro < 0)		ro = 0;
			if (go < 0)		go = 0;
			if (bo < 0)		bo = 0;

			if (ro > 255)	ro = 255;
			if (go > 255)	go = 255;
			if (bo > 255)	bo = 255;
		}
		else
		{
//			ro = bo = go = 0;
			ro = RGBA_GETRED(iv->specular);
            go = RGBA_GETGREEN(iv->specular);
            bo = RGBA_GETBLUE(iv->specular);
			ao = 255;
		}

		ov->dcSpecular = RGBA_MAKE(ro, go, bo, ao);
	}
	return true;
}

#define MATRIXINDX(x, r, c) (*((float*)&x + r * 4 + c))

bool Lighting::ProcessReferenceVertices6()
{
	D3DMATRIX	Mv;					// View matrix
	D3DMATRIX	Mvr;				// View matrix (rotation part)
	D3DMATRIX	Mvt;				// View matrix (translation part)
	D3DMATRIX	Mv_1;				// Inverse view matrix
	D3DMATRIX	Mvr_t;				// Transposed view matrix (rotation part)
	D3DMATRIX	Mvt_1;				// Inverse view matrix (translation part)

	D3DMATRIX	Mw;					// World matrix
	D3DMATRIX	Mwr;				// World matrix (rotation part)
	D3DMATRIX	Mwt;				// World matrix (translation part)
	D3DMATRIX	Mw_1;				// Inverse world matrix
	D3DMATRIX	Mwr_t;				// Transposed world matrix (rotation part)
	D3DMATRIX	Mwt_1;				// Inverse world matrix (translation part)

	D3DMATRIX	Mwv;				// Modelview matrix
	D3DMATRIX	Mwv_1;				// Inverted modelview matrix
	D3DMATRIX	Mcur;				// Full matrix for vertex transformation

	D3DVECTOR	S;					// Scale vector
	float		d;					// Determinant of the rotation part of the world matrix
	float		j;

	// Model space values
	D3DVECTOR	Lp[LT_LIGHTS];		// Positions of light sources
	D3DVECTOR	Ld[LT_LIGHTS];		// Directions of light sources
	D3DVECTOR	Lds[LT_LIGHTS];		// Spot directions of light sources
	D3DVECTOR	Pe;					// Camera position
	
	D3DVECTOR	V;					// Vertex position
	D3DVECTOR	N;					// Normal
	D3DVALUE	att[LT_LIGHTS];		// Attenuations of light sources
	D3DVALUE	spot[LT_LIGHTS];	// Spot for light sources
	D3DVALUE	dot[LT_LIGHTS];		// Dot products for light sources
	D3DVALUE	rsa, gsa, bsa;		// Source ambient colors
	D3DVALUE	rsd, gsd, bsd, asd;	// Source diffufe colors
	D3DVALUE	rss, gss, bss;		// Source specular colors
	D3DVALUE	rse, gse, bse;		// Source emissive colors
	D3DVALUE	rr, gr, br;			// Result colors
	int			ro, go, bo, ao;		// Output colors
	int			i, k;


	// Inverse world matrix
	Mw = m_SceneProperties.WorldMatrix;
	Mwr = IdentityMatrix();
	for (i = 0; i < 3; i++)
		for (k = 0; k < 3; k++)
			MATRIXINDX(Mwr, i,k) = MATRIXINDX(Mw, i,k);
	Mwt = IdentityMatrix();
	for (i = 0; i < 3; i++)
		MATRIXINDX(Mwt, 3,i) = MATRIXINDX(Mw, 3,i);
	Mwr_t = MatrixTranspose(Mwr);
	Mwt_1 = MatrixInverse(Mwt);
	Mw_1 = MatrixMult(Mwt_1, Mwr_t);
	
	// Inverse view matrix
	Mv = m_SceneProperties.ViewMatrix;
	Mvr = IdentityMatrix();
	for (i = 0; i < 3; i++)
		for (k = 0; k < 3; k++)
			MATRIXINDX(Mvr,i,k) = MATRIXINDX(Mv,i,k);
	Mvt = IdentityMatrix();
	for (i = 0; i < 3; i++)
		MATRIXINDX(Mvt,3,i) = MATRIXINDX(Mv,3,i);
	Mvr_t = MatrixTranspose(Mvr);
	Mvt_1 = MatrixInverse(Mvt);
	Mv_1 = MatrixMult(Mvt_1, Mvr_t);
	
	// Full matrix for coordinate transformation
	Mwv = MatrixMult(Mw, Mv);
	Mcur = MatrixMult(Mwv, m_SceneProperties.ProjectionMatrix);
	Mwv_1 = MatrixMult(Mv_1, Mw_1);

	// values to calculate scale vector
	d = MATRIXINDX(Mwr,1,1)*MATRIXINDX(Mwr,2,2)*MATRIXINDX(Mwr,3,3) + MATRIXINDX(Mwr,1,3)*MATRIXINDX(Mwr,2,1)*MATRIXINDX(Mwr,3,2) + MATRIXINDX(Mwr,1,2)*MATRIXINDX(Mwr,2,3)*MATRIXINDX(Mwr,3,1) -
		MATRIXINDX(Mwr,1,3)*MATRIXINDX(Mwr,2,2)*MATRIXINDX(Mwr,3,1) - MATRIXINDX(Mwr,1,2)*MATRIXINDX(Mwr,2,1)*MATRIXINDX(Mwr,3,3) - MATRIXINDX(Mwr,1,1)*MATRIXINDX(Mwr,2,3)*MATRIXINDX(Mwr,3,2);
	if (d < 0.f)		j = -1.f;
	else				j = 1.f;

	// Camera position
	Pe.x = MATRIXINDX(Mwv_1,3,0);
	Pe.y = MATRIXINDX(Mwv_1,3,1);
	Pe.z = MATRIXINDX(Mwv_1,3,2);

	for (i = 0; i < LT_LIGHTS; i++)
	{
		if (m_LightProperties[i].bUseLight2)
		{
		// Light2
			// Scale vector
			S.x = (float)sqrt( MATRIXINDX(Mw,0,0)*MATRIXINDX(Mw,0,0) + MATRIXINDX(Mw,0,1)*MATRIXINDX(Mw,0,1) + MATRIXINDX(Mw,0,2)*MATRIXINDX(Mw,0,2) );
			S.y = (float)sqrt( MATRIXINDX(Mw,1,0)*MATRIXINDX(Mw,1,0) + MATRIXINDX(Mw,1,1)*MATRIXINDX(Mw,1,1) + MATRIXINDX(Mw,1,2)*MATRIXINDX(Mw,1,2) );		
			S.z = (float)sqrt( MATRIXINDX(Mw,2,0)*MATRIXINDX(Mw,2,0) + MATRIXINDX(Mw,2,1)*MATRIXINDX(Mw,2,1) + MATRIXINDX(Mw,2,2)*MATRIXINDX(Mw,2,2) );
			
			if(S.x < 0.001f)	S.x = 0.001f;
			if(S.y < 0.001f)	S.y = 0.001f;
			if(S.z < 0.001f)	S.z = 0.001f;

			S.x *= j;
			S.y *= j;
			S.z *= j;
		
			// Light position for POINT, PARALLELPOINT and SPOT lights
			if (m_LightProperties[i].dwLightType != D3DLIGHT_DIRECTIONAL)
			{
				Lp[i].x = (Mw_1._11*m_LightProperties[i].dvLightPosition.x + Mw_1._21*m_LightProperties[i].dvLightPosition.y + Mw_1._31*m_LightProperties[i].dvLightPosition.z + Mw_1._41) / S.x;
				Lp[i].y = (Mw_1._12*m_LightProperties[i].dvLightPosition.x + Mw_1._22*m_LightProperties[i].dvLightPosition.y + Mw_1._32*m_LightProperties[i].dvLightPosition.z + Mw_1._42) / S.y;
				Lp[i].z = (Mw_1._13*m_LightProperties[i].dvLightPosition.x + Mw_1._23*m_LightProperties[i].dvLightPosition.y + Mw_1._33*m_LightProperties[i].dvLightPosition.z + Mw_1._43) / S.z;
			}

			// Light direction
//			if (m_LightProperties[i].dwLightType == D3DLIGHT_PARALLELPOINT)
//			{
//				Ld[i] = Normalize(Lp[i]);
//			}
			if (m_LightProperties[i].dwLightType == D3DLIGHT_DIRECTIONAL)
			{
				Ld[i].x = (Mwr_t._11*m_LightProperties[i].dvLightDirection.x + Mwr_t._21*m_LightProperties[i].dvLightDirection.y + Mwr_t._31*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Ld[i].y = (Mwr_t._12*m_LightProperties[i].dvLightDirection.x + Mwr_t._22*m_LightProperties[i].dvLightDirection.y + Mwr_t._32*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Ld[i].z = (Mwr_t._13*m_LightProperties[i].dvLightDirection.x + Mwr_t._23*m_LightProperties[i].dvLightDirection.y + Mwr_t._33*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Ld[i] = Normalize(Ld[i]);
				Ld[i].x /= S.x;
				Ld[i].y /= S.y;
				Ld[i].z /= S.z;
				Ld[i] = Normalize(Ld[i]);
			}
			
			// Spot direction
			if (m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
			{
				Lds[i].x = (Mwr_t._11*m_LightProperties[i].dvLightDirection.x + Mwr_t._21*m_LightProperties[i].dvLightDirection.y + Mwr_t._31*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Lds[i].y = (Mwr_t._12*m_LightProperties[i].dvLightDirection.x + Mwr_t._22*m_LightProperties[i].dvLightDirection.y + Mwr_t._32*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Lds[i].z = (Mwr_t._13*m_LightProperties[i].dvLightDirection.x + Mwr_t._23*m_LightProperties[i].dvLightDirection.y + Mwr_t._33*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Lds[i] = Normalize(Lds[i]);
				Lds[i].x /= S.x;
				Lds[i].y /= S.y;
				Lds[i].z /= S.z;
				Lds[i] = Normalize(Lds[i]);
			}
		}
		else
		{
		// Light1
			S.x = 1.f / ( MATRIXINDX(Mw,0,0)*MATRIXINDX(Mw,0,0) + MATRIXINDX(Mw,0,1)*MATRIXINDX(Mw,0,1) + MATRIXINDX(Mw,0,2)*MATRIXINDX(Mw,0,2) );
			S.y = 1.f / ( MATRIXINDX(Mw,1,0)*MATRIXINDX(Mw,1,0) + MATRIXINDX(Mw,1,1)*MATRIXINDX(Mw,1,1) + MATRIXINDX(Mw,1,2)*MATRIXINDX(Mw,1,2) );
			S.z = 1.f / ( MATRIXINDX(Mw,2,0)*MATRIXINDX(Mw,2,0) + MATRIXINDX(Mw,2,1)*MATRIXINDX(Mw,2,1) + MATRIXINDX(Mw,2,2)*MATRIXINDX(Mw,2,2) );

			// Light position for POINT, PARALLELPOINT and SPOT lights
			if (m_LightProperties[i].dwLightType != D3DLIGHT_DIRECTIONAL)
			{
				Lp[i].x = (Mw_1._11*m_LightProperties[i].dvLightPosition.x + Mw_1._21*m_LightProperties[i].dvLightPosition.y + Mw_1._31*m_LightProperties[i].dvLightPosition.z + Mw_1._41) * S.x;
				Lp[i].y = (Mw_1._12*m_LightProperties[i].dvLightPosition.x + Mw_1._22*m_LightProperties[i].dvLightPosition.y + Mw_1._32*m_LightProperties[i].dvLightPosition.z + Mw_1._42) * S.y;
				Lp[i].z = (Mw_1._13*m_LightProperties[i].dvLightPosition.x + Mw_1._23*m_LightProperties[i].dvLightPosition.y + Mw_1._33*m_LightProperties[i].dvLightPosition.z + Mw_1._43) * S.z;
			}

			// Light direction
//			if (m_LightProperties[i].dwLightType == D3DLIGHT_PARALLELPOINT)
//			{
//				Ld[i] = Normalize(Lp[i]);
//			}
			if (m_LightProperties[i].dwLightType == D3DLIGHT_DIRECTIONAL)
			{
				Ld[i].x = (Mwr_t._11*m_LightProperties[i].dvLightDirection.x + Mwr_t._21*m_LightProperties[i].dvLightDirection.y + Mwr_t._31*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Ld[i].y = (Mwr_t._12*m_LightProperties[i].dvLightDirection.x + Mwr_t._22*m_LightProperties[i].dvLightDirection.y + Mwr_t._32*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Ld[i].z = (Mwr_t._13*m_LightProperties[i].dvLightDirection.x + Mwr_t._23*m_LightProperties[i].dvLightDirection.y + Mwr_t._33*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Ld[i] = Normalize(Ld[i]);
			}
			
			// Spot direction
			if (m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
			{
				Lds[i].x = (Mwr_t._11*m_LightProperties[i].dvLightDirection.x + Mwr_t._21*m_LightProperties[i].dvLightDirection.y + Mwr_t._31*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Lds[i].y = (Mwr_t._12*m_LightProperties[i].dvLightDirection.x + Mwr_t._22*m_LightProperties[i].dvLightDirection.y + Mwr_t._32*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Lds[i].z = (Mwr_t._13*m_LightProperties[i].dvLightDirection.x + Mwr_t._23*m_LightProperties[i].dvLightDirection.y + Mwr_t._33*m_LightProperties[i].dvLightDirection.z)*(-1.f);
				Lds[i] = Normalize(Lds[i]);
			}
		}
	}

	// Vertex processing
	for (DWORD v = 0; v < m_nVertices; v++)
	{
		NDSVERTEX		*iv = &m_pSourceVertices[v];
		D3DTLVERTEX		*ov = &m_pReferenceVertices[v];
		
		// Coordinates transformation
		ov->sx  = Mcur._11*iv->x + Mcur._21*iv->y + Mcur._31*iv->z + Mcur._41;
		ov->sy  = Mcur._12*iv->x + Mcur._22*iv->y + Mcur._32*iv->z + Mcur._42;
		ov->rhw = Mcur._14*iv->x + Mcur._24*iv->y + Mcur._34*iv->z + Mcur._44;
		
		// W division
		ov->rhw = 1.f / ov->rhw;
		ov->sx = ov->sx * ov->rhw;
		ov->sy = ov->sy * ov->rhw;

		// Viewport transformation
		ov->sx = (0.5f + ov->sx*0.5f) * m_dwWidth;
		ov->sy = (0.5f - ov->sy*0.5f) * m_dwHeight;

		// We do not need real Z transformation for the test.
		// It could be initialized with some value in [0,1] range
		ov->sz = 0.5f;

		if (m_iVertexNumber >= 0 && (int)v != m_iVertexNumber)
		{
			ov->dcColor = 0;
			ov->dcSpecular = 0;
			continue;
		}

			// Source colors - Ambient
			rsa = m_ModelProperties.Material.Ambient.r * 255.f;
			gsa = m_ModelProperties.Material.Ambient.g * 255.f;
			bsa = m_ModelProperties.Material.Ambient.b * 255.f;
			
			// Emissive component
			rse = m_ModelProperties.Material.Emissive.r * 255.f;
			gse = m_ModelProperties.Material.Emissive.g * 255.f;
			bse = m_ModelProperties.Material.Emissive.b * 255.f;
		

		// Lighting calculations
		N.x = iv->nx;
		N.y = iv->ny;
		N.z = iv->nz;

		// Ambient component
		rr = rsa * m_ModelProperties.dcvAmbientColor.r;
		gr = gsa * m_ModelProperties.dcvAmbientColor.g;
		br = bsa * m_ModelProperties.dcvAmbientColor.b;

		// Emissive component
		rr += rse;
		gr += gse;
		br += bse;
		
		for (i = 0; i < LT_LIGHTS; i++)
		{
			// Source colors - Diffuse component
			if (m_LightProperties[i].bUseLight2 && m_ModelProperties.bColorVertex)
			{
				rsd = (float)RGBA_GETRED(iv->diffuse);
				gsd = (float)RGBA_GETGREEN(iv->diffuse);
				bsd = (float)RGBA_GETBLUE(iv->diffuse);
			}
			else
			{
				rsd = m_ModelProperties.Material.Diffuse.r * 255.f;
				gsd = m_ModelProperties.Material.Diffuse.g * 255.f;
				bsd = m_ModelProperties.Material.Diffuse.b * 255.f;
			}

			// Source colors - Diffuse alha component
			if (m_ModelProperties.bColorVertex)
			{
				asd = (float)RGBA_GETALPHA(iv->diffuse);
			}
			else
			{
				asd = m_ModelProperties.Material.Diffuse.a * 255.f;
			}

			if (m_LightProperties[i].bLightActive)
			{
				if (m_LightProperties[i].bUseLight2)
				{
				// Light 2
					// Vertex position in camera space		
					V.x = iv->x * S.x;
					V.y = iv->y * S.y;
					V.z = iv->z * S.z;
				}
				else
				{
				// Light1
					// Vertex position in camera space		
					V.x = iv->x;
					V.y = iv->y;
					V.z = iv->z;
				}
				// Light direction for POINT and SPOT light types
				if (m_LightProperties[i].dwLightType == D3DLIGHT_POINT || m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
				{
					Ld[i].x = Lp[i].x - V.x;
					Ld[i].y = Lp[i].y - V.y;
					Ld[i].z = Lp[i].z - V.z;
					Ld[i] = Normalize(Ld[i]);
				}
				
				// Dot products
				dot[i] = DotProduct(N, Ld[i]);

				if (dot[i] > 0.f)
				{
					// Attenuation 
					if (m_LightProperties[i].dwLightType == D3DLIGHT_POINT || m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
					{
						D3DVECTOR	D = (D3DXVECTOR3)Lp[i] - (D3DXVECTOR3)V;
						D3DVALUE	d = (float)sqrt(D.x*D.x + D.y*D.y + D.z*D.z);

						if ( (d > 0.f) && (d < m_LightProperties[i].dvRange) )
						{
							if (m_LightProperties[i].bUseLight2)
							{
								float k = (m_LightProperties[i].dvRange - d) / m_LightProperties[i].dvRange;
								att[i] = m_LightProperties[i].dvAttenuation0 + m_LightProperties[i].dvAttenuation1*k + m_LightProperties[i].dvAttenuation2*k*k;
								if (att[i] > 1.f)	att[i] = 1.f;
							}
							else
							{
								att[i] = 1.f / (m_LightProperties[i].dvAttenuation0 + m_LightProperties[i].dvAttenuation1*d + m_LightProperties[i].dvAttenuation2*d*d);
								if (att[i] > 1.f)	att[i] = 1.f;
							}
						}
						else
							att[i] = 0.f;
					}
					else
						att[i] = 1.f;

					// Spot
					if (m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
					{
						D3DVECTOR	VLp = Normalize((D3DXVECTOR3)Lp[i] - (D3DXVECTOR3)V);
						D3DVALUE	rho = DotProduct(Lds[i], VLp);
						D3DVALUE	cp2 = (float)cos(m_LightProperties[i].dvPhi*0.5f);	// cos(phi/2)
						D3DVALUE	ct2 = (float)cos(m_LightProperties[i].dvTheta*0.5f);	// cos(theta/2)

						if (rho > ct2)			spot[i] = 1.f;
						else if (rho <= cp2)	spot[i] = 0.f;
						else
						{
							if (m_LightProperties[i].bUseLight2)
							{
								spot[i] = (float)pow((rho - cp2)/(ct2 - cp2), m_LightProperties[i].dvFalloff);
							}
							else
							{
								spot[i] = 1.f - ((ct2 - rho)/(ct2-cp2));
							}
						}
					}
					else
						spot[i] = 1.f;
					

					D3DVALUE	k = att[i] * spot[i] * dot[i];

					rr += rsd * k * m_LightProperties[i].dcvDiffuse.r;
					gr += gsd * k * m_LightProperties[i].dcvDiffuse.g;
					br += bsd * k * m_LightProperties[i].dcvDiffuse.b;
				}
			}
		}
		
		// Diffuse color 
		ro = (int)(rr);
		go = (int)(gr);
		bo = (int)(br);
		ao = (int)(asd);

		if (ro < 0)		ro = 0;
		if (go < 0)		go = 0;
		if (bo < 0)		bo = 0;
		if (ao < 0)		ao = 0;

		if (ro > 255)	ro = 255;
		if (go > 255)	go = 255;
		if (bo > 255)	bo = 255;
		if (ao > 255)	ao = 255;

		ov->dcColor = RGBA_MAKE(ro, go, bo, ao);

		
		// Specular Color
		rr = gr = br = 0;
		
		if (m_ModelProperties.bSpecular)
		{
			for (i = 0; i < LT_LIGHTS; i++)	
			{
				// Source color - specular component
				if (/*m_LightProperties[i].bUseLight2 &&*/ m_ModelProperties.bColorVertex)
				{
					rss = (float)RGBA_GETRED(iv->specular);
					gss = (float)RGBA_GETGREEN(iv->specular);
					bss = (float)RGBA_GETBLUE(iv->specular);
				}
				else
				{
					rss = m_ModelProperties.Material.Specular.r * 255.f;
					gss = m_ModelProperties.Material.Specular.g * 255.f;
					bss = m_ModelProperties.Material.Specular.b * 255.f;
				}

				if (m_LightProperties[i].bLightActive && (dot[i] > 0.f))
				{
					D3DVECTOR	h;
					if (m_LightProperties[i].bUseLight2)
						h = Normalize( (D3DXVECTOR3)Normalize((D3DXVECTOR3)Pe - (D3DXVECTOR3)V) + (D3DXVECTOR3)Ld[i] );
					else if (m_LightProperties[i].dwLightType == D3DLIGHT_DIRECTIONAL)
						h = Normalize( (D3DXVECTOR3)Normalize(Pe) + (D3DXVECTOR3)Ld[i] );
					else
						h = Normalize( (D3DXVECTOR3)Normalize(Pe) + (D3DXVECTOR3)Normalize(Lp[i]) );

					D3DVALUE	kdist;
					if (!m_LightProperties[i].bUseLight2 && (m_LightProperties[i].dwLightType == D3DLIGHT_POINT || (m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)))
						kdist = 1.f/Magnitude((D3DXVECTOR3)V - (D3DXVECTOR3)Lp[i]);
					else
						kdist = 1.f;

					float sdot = DotProduct(N, h);

					D3DVALUE	k1 = att[i] * spot[i] * kdist;
					D3DVALUE	k2 = (float)pow(sdot, m_ModelProperties.Material.Power);
					D3DVALUE	k = att[i] * spot[i] * kdist * (float)pow(sdot, m_ModelProperties.Material.Power);
					if (k > 0.f)
					{
						rr += rss * k * m_LightProperties[i].dcvSpecular.r;
						gr += gss * k * m_LightProperties[i].dcvSpecular.g;
						br += bss * k * m_LightProperties[i].dcvSpecular.b;
					}
				}
			}

			ro = (int)(rr);
			go = (int)(gr);
			bo = (int)(br);
			ao = 255;

			if (ro < 0)		ro = 0;
			if (go < 0)		go = 0;
			if (bo < 0)		bo = 0;

			if (ro > 255)	ro = 255;
			if (go > 255)	go = 255;
			if (bo > 255)	bo = 255;
		}
		else
		{
			ro = go = bo = 0;
			ao = 255;
		}

		ov->dcSpecular = RGBA_MAKE(ro, go, bo, ao);
	}
	return true;
}