#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "IBuffer.h"


DWORD IBuffer::ResolveTestNumber()
{
    DWORD       dwNum = m_dwCurrentTestNumber - 1;
    D3DPOOL     eIndexPool = D3DPOOL_DEFAULT;
    D3DPOOL     eVertexPool = D3DPOOL_DEFAULT;
    DWORD       dwUsageScenario = 0;

    // Primitive: Combinations - 8; Skip - 1
    m_dwCurrentShape = dwNum % SHAPES_NUMBER;
    dwNum /= SHAPES_NUMBER;

    // Index buffer memory type: Combinations - 3; Skip - 8;
//    switch (dwNum%3)
//    {
//    case 0:     eIndexPool = D3DPOOL_DEFAULT;       break;
//    case 1:     eIndexPool = D3DPOOL_MANAGED;       break;
//    case 2:     eIndexPool = D3DPOOL_SYSTEMMEM;     break;
//    }
    dwNum /= 3;

    // Vertex buffer memory type: Combinations - 3; Skip - 24;
//    switch (dwNum%3)
//    {
//    case 0:     eVertexPool = D3DPOOL_DEFAULT;      break;
//    case 1:     eVertexPool = D3DPOOL_MANAGED;      break;
//    case 2:     eVertexPool = D3DPOOL_SYSTEMMEM;    break;
//    }
    dwNum /= 3;

    // Usage scenario: Combinations - 5; Skip - 72;
    dwUsageScenario = dwNum%5;
    dwNum /= 5;
    // Total: 360

#ifdef UNDER_XBOX // ##HACK
m_d3dcaps.MaxVertexIndex = 0xffff;
#endif

    // Validate test
    if ((dwUsageScenario == 0 || dwUsageScenario == 2) && 
        (m_d3dcaps.MaxVertexIndex < (DWORD)m_pShapes[m_dwCurrentShape]->m_nIndices))
    {
        // Skip one test case to the new shape
        return 1;
    }
    if ((dwUsageScenario == 2 || dwUsageScenario == 3 || dwUsageScenario == 4) &&
        (m_d3dcaps.MaxVertexIndex  <= 0xffff))
    {
        // Skip to the next scenario
        return (72 - (m_dwCurrentTestNumber%72));
    }
    if ((dwUsageScenario == 1 || dwUsageScenario == 3) &&
        (m_d3dcaps.MaxVertexIndex < m_dwMaxIndices) )
    {
        // Skip to the next scenario
        return (72 - (m_dwCurrentTestNumber%72));
    }
    if (dwUsageScenario == 4 &&
        (m_d3dcaps.MaxVertexIndex < m_dwSumIndices) )
    {
        // Skip to the next scenario
        return (72 - (m_dwCurrentTestNumber%72));
    }

    // Recreate buffers
    if (m_pSrcVertexBuffer == NULL || m_pRefVertexBuffer == NULL ||
        m_pSrcIndexBuffer == NULL || m_pRefIndexBuffer == NULL ||
        m_eIndexPool != eIndexPool ||
        m_eVertexPool != eVertexPool ||
        m_dwUsageScenario != dwUsageScenario ||
        dwUsageScenario == 0 ||
        dwUsageScenario == 2)
    {
        m_bRecreateBuffers = true;
        m_eIndexPool = eIndexPool;
        m_eVertexPool = eVertexPool;
        m_dwUsageScenario = dwUsageScenario;
    }

    return 0;
}

bool IBuffer::CreateBuffers()
{
    DWORD   dwVertexBufferSize;
    DWORD   dwIndexBufferSize;
    HRESULT hr;

    ReleaseVertexBuffer(m_pSrcVertexBuffer);
    m_pSrcVertexBuffer = NULL;
    ReleaseVertexBuffer(m_pRefVertexBuffer);
    m_pRefVertexBuffer = NULL;
    ReleaseIndexBuffer(m_pSrcIndexBuffer);
    m_pSrcIndexBuffer = NULL;
    ReleaseIndexBuffer(m_pRefIndexBuffer);
    m_pRefIndexBuffer = NULL;

    switch(m_dwUsageScenario)
    {
    // Recreate 16 bit buffer
    case 0:
        m_dwBufferVertices = m_pShapes[m_dwCurrentShape]->m_nVertices;
        m_dwBufferIndices = m_pShapes[m_dwCurrentShape]->m_nIndices;
        dwVertexBufferSize = m_dwBufferVertices * GetVertexSize(FVF_IBVERTEX);
        dwIndexBufferSize = m_dwBufferIndices * 2;
        m_eIndexFormat = D3DFMT_INDEX16;
        break;
    // Rewrite 16 bit buffer
    case 1:
        m_dwBufferVertices = m_dwMaxVertices;
        m_dwBufferIndices = m_dwMaxIndices;
        dwVertexBufferSize = m_dwBufferVertices * GetVertexSize(FVF_IBVERTEX);
        dwIndexBufferSize = m_dwBufferIndices * 2;
        m_eIndexFormat = D3DFMT_INDEX16;
        break;
    // Recreate 32 bit buffers
#ifndef UNDER_XBOX
    case 2:
        m_dwBufferVertices = m_pShapes[m_dwCurrentShape]->m_nVertices;
        m_dwBufferIndices = m_pShapes[m_dwCurrentShape]->m_nIndices;
        dwVertexBufferSize = m_dwBufferVertices * GetVertexSize(FVF_IBVERTEX);
        dwIndexBufferSize = m_dwBufferIndices * 4;
        m_eIndexFormat = D3DFMT_INDEX32;
        break;
    // Rewrite 32 bit buffer
    case 3:
        m_dwBufferVertices = m_dwMaxVertices;
        m_dwBufferIndices = m_dwMaxIndices;
        dwVertexBufferSize = m_dwBufferVertices * GetVertexSize(FVF_IBVERTEX);
        dwIndexBufferSize = m_dwBufferIndices * 4;
        m_eIndexFormat = D3DFMT_INDEX32;
        break;
    // Single 32 bit buffer
    case 4:
        m_dwBufferVertices = m_dwSumVertices;
        m_dwBufferIndices = m_dwSumIndices;
        dwVertexBufferSize = m_dwBufferVertices * GetVertexSize(FVF_IBVERTEX);
        dwIndexBufferSize = m_dwBufferIndices * 4;
        m_eIndexFormat = D3DFMT_INDEX32;
        break;
#else
    default:
        DebugString(TEXT("Unsupported index buffer type"));
        __asm int 3;
#endif
    }

//    if (m_pSrcDevice8)
//    {
        DWORD   dwUsage = 0;
    
//        if (m_pAdapter->Devices[m_pMode->nSrcDevice].bSetSoftwareVertexProcessing)
//            dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;
        
        // Vertex Buffer
//        m_pSrcVertexBuffer = new CVertexBuffer(this);
//        if (!SetLastError(m_pSrcDevice8->CreateVertexBuffer(dwVertexBufferSize, dwUsage, FVF_IBVERTEX, m_eVertexPool, &m_pSrcVertexBuffer->m_pVB8)))
        hr = m_pDevice->CreateVertexBuffer(dwVertexBufferSize, dwUsage, FVF_IBVERTEX, m_eVertexPool, &m_pSrcVertexBuffer);
        if (FAILED(hr))
        {
            WriteToLog("IBuffer: CreateVertexBuffer() on source device failed\n");
            return false;
        }
        m_vbdesc.m_dwFVF = FVF_IBVERTEX;
        m_vbdesc.m_dwVertexCount = m_dwBufferVertices;
        m_vbdesc.m_dwSize = dwVertexBufferSize;
        m_vbdesc.m_dwUsage = dwUsage;
        m_vbdesc.m_fReference = false;

        // Index buffer
//        m_pSrcIndexBuffer = new CIndexBuffer(this);
//        if (!SetLastError(m_pSrcDevice8->CreateIndexBuffer(dwIndexBufferSize, dwUsage, m_eIndexFormat, m_eIndexPool, &m_pSrcIndexBuffer->m_pIB8)))
        hr = m_pDevice->CreateIndexBuffer(dwIndexBufferSize, dwUsage, m_eIndexFormat, m_eIndexPool, &m_pSrcIndexBuffer);
        if (FAILED(hr)) 
        {
            WriteToLog("IBuffer: CreateIndexBuffer() on source device failed\n");
            return false;
        }
        m_ibdesc.m_fReference = false;
        m_ibdesc.m_d3dfFormat = m_eIndexFormat;
        m_ibdesc.m_dwIndexCount = m_dwBufferIndices;
        m_ibdesc.m_dwSize = dwIndexBufferSize;
        m_ibdesc.m_dwUsage = dwUsage;
//    }
/*
    if (m_pRefDevice8)
    {
        DWORD   dwUsage = 0;
    
        if (m_pAdapter->Devices[m_pMode->nRefDevice].bSetSoftwareVertexProcessing)
            dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;
        
        // Vertex Buffer
        m_pRefVertexBuffer = new CVertexBuffer(this);
        if (!SetLastError(m_pRefDevice8->CreateVertexBuffer(dwVertexBufferSize, dwUsage, FVF_IBVERTEX, m_eVertexPool, &m_pRefVertexBuffer->m_pVB8)))
        {
            WriteToLog("IBuffer: CreateVertexBuffer() on reference device failed\n");
            return false;
        }
        m_pRefVertexBuffer->m_dwFVF = FVF_IBVERTEX;
        m_pRefVertexBuffer->m_dwVertexCount = m_dwBufferVertices;
        m_pRefVertexBuffer->m_dwSize = dwVertexBufferSize;
        m_pRefVertexBuffer->m_dwUsage = dwUsage;
        m_pRefVertexBuffer->m_fReference = false;

        // Index buffer
        m_pRefIndexBuffer = new CIndexBuffer(this);
        if (!SetLastError(m_pRefDevice8->CreateIndexBuffer(dwIndexBufferSize, dwUsage, m_eIndexFormat, m_eIndexPool, &m_pRefIndexBuffer->m_pIB8)))
        {
            WriteToLog("IBuffer: CreateIndexBuffer() on reference device failed\n");
            return false;
        }
        m_pRefIndexBuffer->m_fReference = false;
        m_pRefIndexBuffer->m_d3dfFormat = m_eIndexFormat;
        m_pRefIndexBuffer->m_dwIndexCount = m_dwBufferIndices;
        m_pRefIndexBuffer->m_dwSize = dwIndexBufferSize;
        m_pRefIndexBuffer->m_dwUsage = dwUsage;
    }
*/
    m_bRecreateBuffers = false;
    m_bFillSumBuffer = true;
    return true;   
}

bool IBuffer::FillBuffers()
{
    IBVERTEX *pVertices;
    WORD     *pwIndices;
    DWORD    *pdwIndices;
    int      i, j;
    HRESULT  hr;

    if (m_dwUsageScenario == 4)
    {
        if (m_bFillSumBuffer)
        {
            DWORD   dwShift;
            DWORD   dwIndex;

            // Source vertex buffer
//            if (m_pSrcVertexBuffer->Lock((void**)&pVertices, 0))
            hr = m_pSrcVertexBuffer->Lock(0, m_vbdesc.m_dwSize, (BYTE**)&pVertices, 0);
            if (SUCCEEDED(hr))
            {
                for (i = 0; i < SHAPES_NUMBER; i++)
                {
                    memcpy(pVertices, m_pShapes[i]->m_pVertices, sizeof(IBVERTEX)*m_pShapes[i]->m_nVertices);
                    pVertices += m_pShapes[i]->m_nVertices;
                }
                m_pSrcVertexBuffer->Unlock();
            }
            else
            {
                WriteToLog("IBuffer: Failed to lock vertex buffer on source device\n");
                return false;
            }

            // Source index buffer
            dwShift = 0;
            dwIndex = 0;
//            if (m_pSrcIndexBuffer->Lock((void**)&pdwIndices))
            hr = m_pSrcIndexBuffer->Lock(0, m_ibdesc.m_dwSize, (BYTE**)&pdwIndices, 0);
            if (SUCCEEDED(hr))
            {
                for (i = 0; i < SHAPES_NUMBER; i++)
                {
                    for (j = 0; j < m_pShapes[i]->m_nIndices; j++)
                    {
                        pdwIndices[dwIndex] = m_pShapes[i]->m_pIndices[j] + dwShift;
                        dwIndex++;
                    }
                    dwShift += m_pShapes[i]->m_nVertices;
                }
                m_pSrcIndexBuffer->Unlock();
            }
            else
            {
                WriteToLog("IBuffer: Failed to lock index buffer on source device\n");
                return false;
            }
/*
            // Reference vertex buffer
            if (m_pRefVertexBuffer->Lock((void**)&pVertices, 0))
            {
                for (i = 0; i < SHAPES_NUMBER; i++)
                {
                    memcpy(pVertices, m_pShapes[i]->m_pVertices, sizeof(VERTEX)*m_pShapes[i]->m_nVertices);
                    pVertices += m_pShapes[i]->m_nVertices;
                }
                m_pRefVertexBuffer->Unlock();
            }
            else
            {
                WriteToLog("IBuffer: Failed to lock vertex buffer on reference device\n");
                return false;
            }

            // Reference index buffer
            dwShift = 0;
            dwIndex = 0;
            if (m_pRefIndexBuffer->Lock((void**)&pdwIndices))
            {
                for (i = 0; i < SHAPES_NUMBER; i++)
                {
                    for (j = 0; j < m_pShapes[i]->m_nIndices; j++)
                    {
                        pdwIndices[dwIndex] = m_pShapes[i]->m_pIndices[j] + dwShift;
                        dwIndex++;
                    }
                    dwShift += m_pShapes[i]->m_nVertices;
                }
                m_pRefIndexBuffer->Unlock();
            }
            else
            {
                WriteToLog("IBuffer: Failed to lock index buffer on reference device\n");
                return false;
            }
*/            
            m_bFillSumBuffer = false;
        }

        // Set parameters
        m_dwMinIndex = 0;
        for (i = 0 ; i < (int)m_dwCurrentShape; i++)
            m_dwMinIndex += m_pShapes[i]->m_nVertices;
        m_dwRenderVertices = m_pShapes[m_dwCurrentShape]->m_nVertices;
        m_dwStartIndex = 0;
        for (i = 0 ; i < (int)m_dwCurrentShape; i++)
            m_dwStartIndex += m_pShapes[i]->m_nIndices;
        m_dwRenderIndices = m_pShapes[m_dwCurrentShape]->m_nIndices;

        return true;
    }
    else
    {
        CShapes *pShape = m_pShapes[m_dwCurrentShape];

        m_dwMinIndex = 0;
        m_dwRenderVertices = pShape->m_nVertices;
        m_dwStartIndex = 0;
        m_dwRenderIndices = pShape->m_nIndices;

        // Source vertex buffer
//        if (m_pSrcVertexBuffer->Lock((void**)&pVertices, 0))
        hr = m_pSrcVertexBuffer->Lock(0, m_vbdesc.m_dwSize, (BYTE**)&pVertices, 0);
        if (SUCCEEDED(hr))
        {
            memcpy(pVertices, pShape->m_pVertices, sizeof(IBVERTEX)*m_dwRenderVertices);
            m_pSrcVertexBuffer->Unlock();
        }
        else
        {
            WriteToLog("IBuffer: Failed to lock vertex buffer on source device\n");
            return false;
        }

        // Source index buffer
        if (m_eIndexFormat == D3DFMT_INDEX16)
        {
//            if (m_pSrcIndexBuffer->Lock((void**)&pwIndices))
            hr = m_pSrcIndexBuffer->Lock(0, m_ibdesc.m_dwSize, (BYTE**)&pwIndices, 0);
            if (SUCCEEDED(hr))
            {
                memcpy(pwIndices, pShape->m_pIndices, sizeof(WORD)*m_dwRenderIndices);
                m_pSrcIndexBuffer->Unlock();
            }
            else
            {
                WriteToLog("IBuffer: Failed to lock index buffer on source device\n");
                return false;
            }
        }
        else
        {
//            if (m_pSrcIndexBuffer->Lock((void**)&pdwIndices))
            hr = m_pSrcIndexBuffer->Lock(0, m_ibdesc.m_dwSize, (BYTE**)&pdwIndices, 0);
            if (SUCCEEDED(hr))
            {
                for (int i = 0; i < (int)m_dwRenderIndices; i++)
                    pdwIndices[i] = pShape->m_pIndices[i];
                m_pSrcIndexBuffer->Unlock();
            }
            else
            {
                WriteToLog("IBuffer: Failed to lock index buffer on source device\n");
                return false;
            }
        }
/*
        // Reference vertex buffer
        if (m_pRefVertexBuffer->Lock((void**)&pVertices, 0))
        {
            memcpy(pVertices, pShape->m_pVertices, sizeof(VERTEX)*m_dwRenderVertices);
            m_pRefVertexBuffer->Unlock();
        }
        else
        {
            WriteToLog("IBuffer: Failed to lock vertex buffer on reference device\n");
            return false;
        }

        // Reference index buffer
        if (m_eIndexFormat == D3DFMT_INDEX16)
        {
            if (m_pRefIndexBuffer->Lock((void**)&pwIndices))
            {
                memcpy(pwIndices, pShape->m_pIndices, sizeof(WORD)*m_dwRenderIndices);
                m_pRefIndexBuffer->Unlock();
            }
            else
            {
                WriteToLog("IBuffer: Failed to lock index buffer on reference device\n");
                return false;
            }
        }
        else
        {
            if (m_pRefIndexBuffer->Lock((void**)&pdwIndices))
            {
                for (int i = 0; i < m_dwRenderIndices; i++)
                    pdwIndices[i] = pShape->m_pIndices[i];
                m_pRefIndexBuffer->Unlock();
            }
            else
            {
                WriteToLog("IBuffer: Failed to lock index buffer on reference device\n");
                return false;
            }
        }
*/
    }
    return true;
}
