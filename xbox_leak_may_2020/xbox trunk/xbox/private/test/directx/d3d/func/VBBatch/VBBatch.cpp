#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "VBBatch.h"

//CD3DWindowFramework     theApp;
//VBBatch                 TestOne;

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    VBBatch* TestOne;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    TestOne = new VBBatch();
    if (!TestOne) {
        return FALSE;
    }

    // Initialize the scene
    if (!TestOne->Create(pDisplay)) {
        TestOne->Release();
        return FALSE;
    }

    bRet = TestOne->Exhibit(pnExitCode);

    // Clean up the scene
    TestOne->Release();

    return bRet;
}


VBBatch::VBBatch()
{
//    m_pMaterial = NULL;
//    m_pLight = NULL;
    for (int i = 0; i < 8; i++)
        m_pTextureArray[i] = NULL;
    m_pSrcVertexBuffer = NULL;
//    m_pDstVertexBuffer = NULL;
    m_pShape = NULL;
    m_pRemappedIndices = NULL;

    m_dwCurrentTestNumber = 0;
    m_dwBatchSize = 0;
//    m_bForceSystemMemory = true;
    m_bForceSystemMemory = false;
    m_dwFVF = 0;
    m_dwAPI = 0;
    m_bLighting = false;
    m_bSpecular = false;
    m_bSrcOptimize = false;
    m_bProcessVertices = false;
    m_bDstForceSystemMemory = true;
    m_dwDstFVF = 0;
    m_dwTotalTests = 1920;

    m_dwSrcVBFVF = 0;
    m_dwSrcVBAPI = 0;
    m_bSrcVBOptimize = false;
    m_dwDstVBFVF = 0;
    m_dwDstVBAPI = 0;
    m_bDstVBOptimize = false;

    m_dwVertices = 0;
    m_dwIndices = 0;

	m_szTestName = TEXT("VBBatch");
	m_szCommandKey = TEXT("VBBatch");
}

VBBatch::~VBBatch()
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < 8; i++)
        ReleaseTexture(m_pTextureArray[i]);
    ReleaseVertexBuffer(m_pSrcVertexBuffer);
//    RELEASE(m_pDstVertexBuffer);
    if (m_pShape)
        delete m_pShape;
    if (m_pRemappedIndices)
        delete [] m_pRemappedIndices;
}


UINT VBBatch::TestInitialize(void)
{
//	CImageLoader    Loader;
//	CImageData*		pImage = NULL;
    
    SetTestRange(1, m_dwTotalTests);
    // Check for lighting support
    if (NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS) ||
        0 == m_d3dcaps.MaxActiveLights)
    {
        WriteToLog("VBBatch: The driver does not support lighting.\n");
        return D3DTESTINIT_SKIPALL;
    }


    // Creating shape
    m_pShape = new CShapes;
    m_pShape->NewShape(CS_SPHERE, /*73*/ 50);

    // Allocating memory for remapping indices
    m_pRemappedIndices = new WORD [m_pShape->m_nIndices];
    
    
    // Get number of supported texture stages and textures
    m_wMaxSimultaneousTextures = (WORD)m_d3dcaps.MaxSimultaneousTextures;
    m_wMaxTextureBlendStages = (WORD)m_d3dcaps.MaxTextureBlendStages;


    // Loading textures
    for (int i = 0; i < m_wMaxSimultaneousTextures; i++) {
		TCHAR fbuf[10];
		wsprintf(fbuf, TEXT("tex%1d.bmp"), i);
//		pImage = Loader.Load(CIL_BMP,fbuf);
//        if (NULL == pImage)
//        {
//            WriteToLog("VBBatch: failed to load %s image\n", fbuf);
//            return D3DTESTINIT_ABORT;
//        }
        m_pTextureArray[i] = (CTexture8*)CreateTexture(m_pDevice, fbuf, D3DFMT_A8R8G8B8,
                                TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE,
                                POOL_DEFAULT, 256, 256);
//		m_pTextureArray[i] = CreateTexture(256,256,CDDS_TEXTURE_MANAGE,pImage);
//		RELEASE(pImage);
        if (NULL == m_pTextureArray[i]) 
        {
			WriteToLog("VBBatch: failed to create texture from %s image\n", fbuf);
            return D3DTESTINIT_ABORT;
        }
		SetTexture(i, m_pTextureArray[i]);
    }

	return D3DTESTINIT_RUN;
}


bool VBBatch::ExecuteTest(UINT iTest)
{
    m_dwCurrentTestNumber = (DWORD)iTest;

    if (!ResolveTestNumber())
        return false;

    if (!CreateVertexBuffers())
    {
        WriteToLog("Error creating vertex buffer");
        return false;
    }

    BeginTestCase("");

    if (!SetProperties())
    {
        Fail();
        EndTestCase();
        return false;
    }

    return true;
}

bool VBBatch::ClearFrame(void)
{
	DebugString(_T("CD3DTest::ClearFrame\n"));
    return Clear(RGB_MAKE(50,50,100));
}



void VBBatch::SceneRefresh(void) 
{
    DWORD dwSize = GetVertexSize(m_dwFVF);

    if (BeginScene())
    {
/*
        if (m_bProcessVertices)
        {
            DWORD dwVertexOp = D3DVOP_TRANSFORM;

            if (m_bLighting) 
                dwVertexOp |= D3DVOP_LIGHT;
            SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)m_bLighting);

            if (DPVB_API == m_dwAPI)
            {
                DWORD   n = 0;

                while ( (m_dwVertices - n) >= m_dwBatchSize )
                {
                    if (m_dwVersion > 0x0700)
                    {
                        SetStreamSource(0, m_pSrcVertexBuffer, NULL, GetVertexSize(m_dwFVF), CD3D_SRC);
                        SetVertexShader(m_dwFVF, 0);
                        ProcessVertices(n, n, m_dwBatchSize, m_pDstVertexBuffer, 0);
                    }
                    else
                    {
                        m_pDstVertexBuffer->ProcessVertices(dwVertexOp, n, m_dwBatchSize, 
                            m_pSrcVertexBuffer, n, false, 0);
                    }
                    DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pDstVertexBuffer, m_pRefVertexBuffer, 
                        (DWORD)n, m_dwBatchSize, NULL, 0, 0);
                    n += m_dwBatchSize;
                }
                if (n < m_dwVertices)
                {
                    if (m_dwVersion > 0x0700)
                    {
                        SetStreamSource(0, m_pSrcVertexBuffer, NULL, GetVertexSize(m_dwFVF), CD3D_SRC);
                        SetVertexShader(m_dwFVF, 0);
                        ProcessVertices(n, n, (m_dwVertices - n), m_pDstVertexBuffer, 0);
                    }
                    else
                    {
                        m_pDstVertexBuffer->ProcessVertices(dwVertexOp, n, (m_dwVertices - n), 
                            m_pSrcVertexBuffer, n, false, 0);
                    }
                    DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pDstVertexBuffer, m_pRefVertexBuffer, 
                        (DWORD)n, (m_dwVertices - n), NULL, 0, 0);
                }
            }
            else if (DIPVB_API == m_dwAPI)
            {
                WORD*   pIndices = m_pShape->m_pIndices;
                WORD    wMinIndex;
                WORD    wMaxIndex;
                WORD    wVertices;
                WORD    wCount;
                DWORD   n = 0;
                while ( (m_dwIndices - n) >= m_dwBatchSize )
                {
                    wMinIndex = m_dwIndices;
                    wMaxIndex = 0;
                    for (wCount = 0; wCount < m_dwBatchSize; wCount++)
                    {
                        wMinIndex = (wMinIndex < pIndices[n+wCount]) ? wMinIndex : pIndices[n+wCount];
                        wMaxIndex = (wMaxIndex > pIndices[n+wCount]) ? wMaxIndex : pIndices[n+wCount];
                    }
                    wVertices = wMaxIndex - wMinIndex + 1;
                    
                    // Remapping indices
                    for (wCount = 0; wCount < m_dwBatchSize; wCount++)
                        m_pRemappedIndices[wCount] = (m_dwVersion >= 0x0800) ? pIndices[n + wCount] : pIndices[n + wCount] - wMinIndex;

                    if (m_dwVersion > 0x0700)
                    {
                        SetStreamSource(0, m_pSrcVertexBuffer, NULL, GetVertexSize(m_dwFVF), CD3D_SRC);
                        SetVertexShader(m_dwFVF, 0);
                        ProcessVertices(wMinIndex, wMinIndex, wVertices, m_pDstVertexBuffer, 0);
                    }
                    else
                    {
                        m_pDstVertexBuffer->ProcessVertices(dwVertexOp, wMinIndex, wVertices, 
                            m_pSrcVertexBuffer, wMinIndex, false, 0);
                    }

                    DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pDstVertexBuffer, m_pRefVertexBuffer, 
                        wMinIndex, wVertices, m_pRemappedIndices, m_dwBatchSize, 0);
                    n += m_dwBatchSize;
                }
                if (n < m_dwIndices)
                {
                    wMinIndex = (WORD)m_pShape->m_nIndices;
                    wMaxIndex = 0;
                    for (wCount = 0; wCount < (m_dwIndices-n); wCount++)
                    {
                        wMinIndex = (wMinIndex < pIndices[n+wCount]) ? wMinIndex : pIndices[n+wCount];
                        wMaxIndex = (wMaxIndex > pIndices[n+wCount]) ? wMaxIndex : pIndices[n+wCount];
                    }
                    wVertices = wMaxIndex - wMinIndex + 1;

                    // Remapping indices
                    for (wCount = 0; wCount < (m_dwIndices-n); wCount++)
                        m_pRemappedIndices[wCount] = (m_dwVersion >= 0x0800) ? pIndices[n + wCount] : pIndices[n + wCount] - wMinIndex;

                    if (m_dwVersion > 0x0700)
                    {
                        SetStreamSource(0, m_pSrcVertexBuffer, NULL, GetVertexSize(m_dwFVF), CD3D_SRC);
                        SetVertexShader(m_dwFVF, 0);
                        ProcessVertices(wMinIndex, wMinIndex, wVertices, m_pDstVertexBuffer, 0);
                    }
                    else
                    {
                        m_pDstVertexBuffer->ProcessVertices(dwVertexOp, wMinIndex, wVertices, 
                            m_pSrcVertexBuffer, wMinIndex, false, 0);
                    }
                    DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pDstVertexBuffer, m_pRefVertexBuffer, 
                        wMinIndex, wVertices, m_pRemappedIndices, (m_dwIndices-n), 0);
                }
            }
        }
        else
        {
*/
            if (DPVB_API == m_dwAPI)
            {
                DWORD n = 0;
                m_pDevice->SetStreamSource(0, m_pSrcVertexBuffer, dwSize);
                while ( (m_dwVertices - n) >= m_dwBatchSize )
                {
                    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, (DWORD)n, m_dwBatchSize / 3);
//                    DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pSrcVertexBuffer, m_pRefVertexBuffer, 
//                        (DWORD)n, m_dwBatchSize, NULL, 0, 0);
                    n += m_dwBatchSize;
                }
                if (n < m_dwVertices)
                    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, (DWORD)n, (m_dwVertices - n) / 3);
//                    DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pSrcVertexBuffer, m_pRefVertexBuffer, 
//                        (DWORD)n, (m_dwVertices - n), NULL, 0, 0);
            }
            else if (DIPVB_API == m_dwAPI)
            {
                WORD*   pIndices = m_pShape->m_pIndices;
                WORD    wMinIndex;
                WORD    wMaxIndex;
                WORD    wVertices;
                WORD    wCount;
                DWORD   n = 0;
                CIndexBuffer8* pd3di;
                while ( (m_dwIndices - n) >= m_dwBatchSize )
                {
                    wMinIndex = (WORD)m_dwIndices;
                    wMaxIndex = 0;
                    for (wCount = 0; wCount < m_dwBatchSize; wCount++)
                    {
                        wMinIndex = (wMinIndex < pIndices[n+wCount]) ? wMinIndex : pIndices[n+wCount];
                        wMaxIndex = (wMaxIndex > pIndices[n+wCount]) ? wMaxIndex : pIndices[n+wCount];
                    }
                    wVertices = wMaxIndex - wMinIndex + 1;
                    
                    // Remapping indices
                    for (wCount = 0; wCount < m_dwBatchSize; wCount++)
                        m_pRemappedIndices[wCount] = (m_dwVersion >= 0x0800) ? pIndices[n + wCount] : pIndices[n + wCount] - wMinIndex;

                    pd3di = CreateIndexBuffer(m_pDevice, m_pRemappedIndices, m_dwBatchSize * sizeof(WORD));
                    m_pDevice->SetStreamSource(0, m_pSrcVertexBuffer, dwSize);
                    m_pDevice->SetIndices(pd3di, 0);

                    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, (DWORD)n, wVertices, 0, m_dwBatchSize / 3);
//                    DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pSrcVertexBuffer, m_pRefVertexBuffer, 
//                        wMinIndex, wVertices, m_pRemappedIndices, m_dwBatchSize, 0);
                    ReleaseIndexBuffer(pd3di);
                    n += m_dwBatchSize;
                }
                if (n < m_dwIndices)
                {
                    wMinIndex = (WORD)m_pShape->m_nIndices;
                    wMaxIndex = 0;
                    for (wCount = 0; wCount < (m_dwIndices-n); wCount++)
                    {
                        wMinIndex = (wMinIndex < pIndices[n+wCount]) ? wMinIndex : pIndices[n+wCount];
                        wMaxIndex = (wMaxIndex > pIndices[n+wCount]) ? wMaxIndex : pIndices[n+wCount];
                    }
                    wVertices = wMaxIndex - wMinIndex + 1;

                    // Remapping indices
                    for (wCount = 0; wCount < (m_dwIndices-n); wCount++)
                        m_pRemappedIndices[wCount] = (m_dwVersion >= 0x0800) ? pIndices[n + wCount] : pIndices[n + wCount] - wMinIndex;

                    pd3di = CreateIndexBuffer(m_pDevice, m_pRemappedIndices, (m_dwIndices-n) * sizeof(WORD));
                    m_pDevice->SetStreamSource(0, m_pSrcVertexBuffer, dwSize);
                    m_pDevice->SetIndices(pd3di, 0);

                    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, (DWORD)n, wVertices, 0, (m_dwIndices-n) / 3);
//                    DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pSrcVertexBuffer, m_pRefVertexBuffer, 
//                        wMinIndex, wVertices, m_pRemappedIndices, (m_dwIndices-n), 0);
                    ReleaseIndexBuffer(pd3di);
                }
            }
//        }

        EndScene();
    }

    WriteStatus(0, "Batch size", "%d", m_dwBatchSize);
    if (m_dwVersion < 0x0800)
        WriteStatus(0, "API", "%s", (m_dwAPI == DIPVB_API) ? "DrawIndexedPrimitiveVB" : "DrawPrimitiveVB");
    else
        WriteStatus(0, "API", "%s", (m_dwAPI == DIPVB_API) ? "DrawIndexedPrimitive" : "DrawPrimitive");
}

bool VBBatch::ProcessFrame(void)
{
	bool    bResult;

    bResult = GetCompareResult(0.15f, 0.78f, 0);

    if (!bResult) {
		WriteToLog("Test failed");
		Fail();
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}

bool VBBatch::TestTerminate(void)
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < 8; i++)
    {
        ReleaseTexture(m_pTextureArray[i]);
        m_pTextureArray[i] = NULL;
    }
    ReleaseVertexBuffer(m_pSrcVertexBuffer);
//    RELEASE(m_pDstVertexBuffer);
    if (m_pShape)
    {
        delete m_pShape;
        m_pShape = NULL;
    }
    if (m_pRemappedIndices)
    {
        delete [] m_pRemappedIndices;
        m_pRemappedIndices = NULL;
    }

	return true;
}
