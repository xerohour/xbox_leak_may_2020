#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "VBBatch.h"

bool VBBatch::ResolveTestNumber(void)
{
    DWORD   n = m_dwCurrentTestNumber - 1;
    DWORD   tmp;

    // Base vertex format
    m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3;
    m_dwDstFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX3;


    // Set the batch size; combinations: 15; skip: 1
    tmp = n % 15;
    n /= 15;
    m_dwBatchSize = (UINT)pow(2, tmp) * 3;
    m_dwBatchSize = ((int)m_dwBatchSize <= m_pShape->m_nIndices) ? m_dwBatchSize : m_pShape->m_nIndices;

    // API; combinations: 2; skip: 15
    if (n & 0x01)
        m_dwAPI = DIPVB_API;
    else
        m_dwAPI = DPVB_API;
    n >>= 1;
    
    // System memory vertex buffers; combinations: 2; skip: 30
//    if (n & 0x01)
//        m_bForceSystemMemory = true;
//    else 
//        m_bForceSystemMemory = false;
    n >>= 1;

    // Optimized vertex buffers; combinations: 2; skip: 60
//    if (n & 0x01)
//        m_bSrcOptimize = true;
//    else
//        m_bSrcOptimize = false;
    n >>= 1;

    // Input specular color; combinations: 2; skip: 120 
    if (n & 0x01)
    {
        m_dwFVF |= D3DFVF_SPECULAR;
        m_dwDstFVF |= D3DFVF_SPECULAR;
    }
    n >>= 1;
    
    // Lighting; combinations: 2; skip: 240
    if (n & 0x01)
        m_bLighting = true;
    else
        m_bLighting = false;
    n >>= 1;

    // Specular; combinations: 2; skip:480
    if (n & 0x01)
    {
        m_bSpecular = true;
        if(!KeySet("BUG30301"))
			m_dwDstFVF |= D3DFVF_SPECULAR;
    }
    else
        m_bSpecular = false;
    n >>= 1;

    // Use ProcessVertices; combinations: 2; skip: 960
//    if (n & 0x01)
//        m_bProcessVertices = true;
//    else
//        m_bProcessVertices = false;
    
    // Skip ProcessVertices tests on devices that could not do it
//    if (m_dwVersion > 0x0700 && (
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DPureHalDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DTnLHalDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DMHWHalDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DPureRefDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DTnLRefDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DMHWRefDevice))
//    {
        SkipTests(960 - ((m_dwCurrentTestNumber-1)%960));
        return false;
//    }
    
    // Skip system memory optimized vertex buffer tests on hardware processing
//    if (!m_bProcessVertices && m_bForceSystemMemory && m_bSrcOptimize && (
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DTnLHalDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DMHWHalDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DPureHalDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DTnLRefDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DMHWRefDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DPureRefDevice))
//    {
//        SkipTests(30 - ((m_dwCurrentTestNumber-1)%30));
//        return false;
//    }

    // Skip system memory optimization on DX7 reference device
//    if (m_dwVersion == 0x0700 && !m_bProcessVertices && m_bForceSystemMemory && m_bSrcOptimize &&
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DRefDevice)
//    {
//        SkipTests(30 - ((m_dwCurrentTestNumber-1)%30));
//        return false;
//    }

    // Skip video memory optimized vertex buffer tests on software processing
//    if (!m_bForceSystemMemory && m_bSrcOptimize && (
//        m_bProcessVertices ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DHALDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DMSWHalDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DRefDevice ||
//        m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DMSWRefDevice))
//    {
//        SkipTests(30 - ((m_dwCurrentTestNumber-1)%30));
//        return false;
//    }

    return true;
}

bool VBBatch::SetProperties(void)
{
    // Add some rotation throw the world matrix
    D3DMATRIX   Matrix = MatrixMult(RotateZMatrix(-0.2f), RotateYMatrix(float(m_dwCurrentTestNumber)*0.1f));
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
        return false;

    // Reset texture stages
    DWORD dwTex = (m_dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    dwTex = (dwTex < m_wMaxSimultaneousTextures) ? dwTex : m_wMaxSimultaneousTextures; 
    for (DWORD i = 0; i < dwTex; i++)
    {
        SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, (DWORD)i);
		SetTextureStageState(i, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		if (i) 
		{
			SetTextureStageState(i, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
			SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_ADD);
		}
		else
		{
			SetTextureStageState(i, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
			SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
		}

        // Set texture wrapping
        SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRENDERSTATE_WRAP0 + i), (DWORD)(D3DWRAPCOORD_0 | D3DWRAPCOORD_1));
    }
    if (i <= m_wMaxTextureBlendStages)
    {
        SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
    }


    // Lighting render states
    SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)((m_bLighting) ? TRUE : FALSE));
    SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD)((m_bSpecular) ? TRUE : FALSE));

    return true;
}

bool VBBatch::CreateVertexBuffers(void)
{
    // Small check for the input parameters
    if (m_dwFVF == 0)
    {
        WriteToLog("Incorrect FVF specified for the test");
        return false;
    }

    // Check vertex buffer parameters to see if we need to recreate
    // source/reference vertex buffers
    if (m_bForceSystemMemory != m_bSrcVBForceSystemMemory ||
        m_dwFVF != m_dwSrcVBFVF ||
        m_dwAPI != m_dwSrcVBAPI ||
        m_bSrcOptimize != m_bSrcVBOptimize)
    {
        DWORD   dwFlags = D3DUSAGE_WRITEONLY;//D3DVBCAPS_WRITEONLY;
        DWORD   dwSize;
        
        if (DIPVB_API == m_dwAPI)
            m_dwVertices = m_pShape->m_nVertices;
        else if (DPVB_API == m_dwAPI)
            m_dwVertices = m_pShape->m_nIndices;
        else
        {
            WriteToLog("Incorrect API specified for the test");
            return false;
        }
        m_dwIndices = m_pShape->m_nIndices;

//        if (m_bForceSystemMemory || m_bProcessVertices ||
//            m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DHALDevice ||
//            m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DRGBDevice ||
//            m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DMMXDevice ||
//            m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DRefDevice)
//        {
//            dwFlags |= D3DVBCAPS_SYSTEMMEMORY;
//        }

        dwSize = GetVertexSize(m_dwFVF);

        // Source vertex buffer
        ReleaseVertexBuffer(m_pSrcVertexBuffer);
        m_pSrcVertexBuffer = CreateVertexBuffer(m_pDevice, NULL, m_dwVertices * dwSize, dwFlags, m_dwFVF);
        if (NULL == m_pSrcVertexBuffer)
        {
            WriteToLog("Error creating source vertex buffer");
            m_dwSrcVBFVF = 0;
            m_dwSrcVBAPI = 0;
            return false;
        }
        if (!FillVertexBuffer(m_pSrcVertexBuffer, dwSize, m_dwFVF, m_dwAPI))
            return false;
//        if (m_bSrcOptimize)
//            m_pSrcVertexBuffer->Optimize(false);
/*
        // Reference vertex buffer
        RELEASE(m_pRefVertexBuffer);
        dwFlags |= D3DVBCAPS_SYSTEMMEMORY;
        m_pRefVertexBuffer = CreateVertexBuffer(m_dwFVF, m_dwVertices, dwFlags, true);
        if (NULL == m_pRefVertexBuffer)
        {
            WriteToLog("Error creating reference vertex buffer");
            m_dwSrcVBFVF = 0;
            m_dwSrcVBAPI = 0;
            return false;
        }
        if (!FillVertexBuffer(m_pRefVertexBuffer, m_dwFVF, m_dwAPI))
            return false;
*/        
        // Record new source vertex buffer parameters
        m_bSrcVBForceSystemMemory = m_bForceSystemMemory;
        m_dwSrcVBFVF = m_dwFVF;
        m_dwSrcVBAPI = m_dwAPI;
        m_bSrcVBOptimize = m_bSrcOptimize;
    }

/*
    // Checking destination vertex buffer parameters
    if (m_bProcessVertices && (
        m_bForceSystemMemory != m_bSrcVBForceSystemMemory ||
        m_dwDstFVF != m_dwDstVBFVF ||
        m_dwAPI != m_dwDstVBAPI))
    {
        DWORD   dwFlags = D3DVBCAPS_WRITEONLY;
        
        if (m_bForceSystemMemory ||
            m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DRGBDevice ||
            m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DMMXDevice ||
            m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DRefDevice)
        {
            dwFlags |= D3DVBCAPS_SYSTEMMEMORY;
        }

        
        RELEASE(m_pDstVertexBuffer);
        m_pDstVertexBuffer = CreateVertexBuffer(m_dwDstFVF, m_dwVertices, dwFlags, false);
        if (NULL == m_pDstVertexBuffer)
        {
            WriteToLog("Error creating destination vertex buffer");
            m_dwDstVBFVF = 0;
            m_dwDstVBAPI = 0;
            return false;
        }
    }
*/
    return true;
}

bool VBBatch::FillVertexBuffer(CVertexBuffer8* pVB, DWORD dwSize, DWORD dwFVF, DWORD dwAPI)
{
    void*   buf = NULL;
    WORD    i;
    HRESULT hr;

//    if (!pVB->Lock(&buf, &dwSize))
    hr = pVB->Lock(0, dwSize, (LPBYTE*)&buf, 0);
    if (FAILED(hr))
    {
        WriteToLog("Error locking vertex buffer");
        return false;
    }

    if (DIPVB_API == dwAPI)
    {
        for (i = 0; i < m_pShape->m_nVertices; i++)
            CopyVertex(&buf, dwFVF, i);
    }
    else if (DPVB_API == dwAPI)
    {
        for (i = 0; i < m_pShape->m_nIndices; i++)
            CopyVertex(&buf, dwFVF, m_pShape->m_pIndices[i]);
    }

    pVB->Unlock();
    return true;
}

void VBBatch::CopyVertex(void **buf, DWORD dwFVF, WORD n)
{
    BYTE*   p = (BYTE*)(*buf);
    DWORD   dwTex;
    DWORD   i;

    // Position    
    if (D3DFVF_XYZ == (dwFVF & D3DFVF_POSITION_MASK))
    {
        memcpy(p, &m_pShape->m_pVertices[n].x, 3*sizeof(D3DVALUE));
        p += 3*sizeof(D3DVALUE);
    }

    // Normal
    if (D3DFVF_NORMAL & dwFVF)
    {
        memcpy(p, &m_pShape->m_pVertices[n].nx, 3*sizeof(D3DVALUE));
        p += 3*sizeof(D3DVALUE);
    }

    // Diffuse color
    if (D3DFVF_DIFFUSE & dwFVF)
    {
        (*((D3DCOLOR*)p)) = RGBA_MAKE(255, 255, 255, 0);
        p += sizeof(D3DCOLOR);
    }

    // Specular color
    if (D3DFVF_SPECULAR & dwFVF)
    {
        (*((D3DCOLOR*)p)) = RGBA_MAKE(200, 200, 255, 0);
        p += sizeof(D3DCOLOR);
    }

    // Texture coordinates
    dwTex = (dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    for (i = 0; i < dwTex; i++)
    {
        memcpy(p, &m_pShape->m_pVertices[n].tu, 2*sizeof(D3DVALUE));
        p += 2*sizeof(D3DVALUE);
    }

    (*buf) = (void*)p;
}

