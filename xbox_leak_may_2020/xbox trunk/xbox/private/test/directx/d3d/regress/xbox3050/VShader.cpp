#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "VShader.h"

//USETESTFRAME

//CD3DWindowFramework		theApp;
//VShader                 LangTest;
//Fixed                   FixedTest;

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

    VShader* pShader;
    BOOL     bQuit = FALSE, bRet = TRUE;
    UINT     i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 1; i < 2 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pShader = new VShader();
                break;
            case 1:
                pShader = new Fixed();
                break;
        }

        if (!pShader) {
            return FALSE;
        }

        // Initialize the scene
        if (!pShader->Create(pDisplay)) {
            pShader->Release();
            return FALSE;
        }

        bRet = pShader->Exhibit(pnExitCode);

        bQuit = pShader->AbortedExit();

        // Clean up the scene
        pShader->Release();
    }

    return bRet;
}


VShader::VShader()
{
    m_dwShaders = 0;
    ZeroMemory(m_pShaders, sizeof(DWORD)*MAXSHADERS);

	m_pTexture = NULL;
	m_pSrcVB = NULL;
	m_pRefVB = NULL;
	m_pSrcVB1 = NULL;
	m_pRefVB1 = NULL;
	m_pSrcVB2 = NULL;
	m_pRefVB2 = NULL;
	m_pSrcVB3 = NULL;
	m_pRefVB3 = NULL;
	m_pSrcIB = NULL;
	m_pRefIB = NULL;

	m_szTestName = TEXT("VSOps");
	m_szCommandKey = TEXT("VSOps");

    m_bExit = FALSE;
}

VShader::~VShader()
{
	
}

UINT VShader::TestInitialize(void)
{
//	CImageLoader    Loader;
//	CImageData		*pImageData;
    CShapes			*pShape;
    VOID			*pBuf;
	int				i;
    HRESULT         hr;

    m_dwTotalTests = 25 * 2;
    SetTestRange(1, m_dwTotalTests);

    // Check for D3D version
    if (m_dwVersion < 0x0800)
    {   
        WriteToLog("VShader: Test requires DirectX version 8 or later.\n");
        return D3DTESTINIT_SKIPALL;
    }

    // Check vertex shader support
    if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion) < 1)
    {
        WriteToLog("VShader: Vertex shader version must be at least 1 for this test to run.\n");
        return D3DTESTINIT_SKIPALL;
    } 
    m_dwShaderVersion = m_d3dcaps.VertexShaderVersion;

    // Check multistream rendering support
	m_dwMaxStreams = m_d3dcaps.MaxStreams;

    // Create render object
    pShape = new CShapes;
    if (NULL == pShape)
    {
		WriteToLog("VShader: Not enough memory to create primitive\n");
        return D3DTESTINIT_ABORT;
    }
	pShape->NewShape(CS_SPHERE);
    for (i = 0; i < pShape->m_nVertices; i++)
    {
        ((VERTEX0*)(pShape->m_pVertices))[i].x = ((VERTEX0*)(pShape->m_pVertices))[i].x * 2.f;
        ((VERTEX0*)(pShape->m_pVertices))[i].y = ((VERTEX0*)(pShape->m_pVertices))[i].y * 2.f;
        ((VERTEX0*)(pShape->m_pVertices))[i].z += 0.5f;
    }

    // Copy vertices / indices
    m_dwVertices = pShape->m_nVertices;
    m_dwIndices = pShape->m_nIndices;
    
    // Vertex buffer 0
    m_pSrcVB = CreateVertexBuffer(m_pDevice, pShape->m_pVertices, m_dwVertices * sizeof(VERTEX0));
    if (NULL == m_pSrcVB)
    {
        WriteToLog("VShader: Could not create source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
/*
	m_pSrcVB = CreateVertexBuffer(0, sizeof(VERTEX)*m_dwVertices, 0, false);
    if (NULL == m_pSrcVB)
    {
        WriteToLog("VShader: Could not create source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pSrcVB->Lock(&pBuf))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pShape->m_pVertices, m_dwVertices*sizeof(VERTEX));
    m_pSrcVB->Unlock();
    
    m_pRefVB = CreateVertexBuffer(0, sizeof(VERTEX)*m_dwVertices, 0, true);
    if (NULL == m_pRefVB)
    {
        WriteToLog("VShader: Could not create reference vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pRefVB->Lock(&pBuf))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pShape->m_pVertices, m_dwVertices*sizeof(VERTEX));
    m_pRefVB->Unlock();
*/    
    // Vertex buffer 1
    m_pSrcVB1 = CreateVertexBuffer(m_pDevice, pShape->m_pVertices, m_dwVertices * sizeof(VERTEX0));
    if (NULL == m_pSrcVB1)
    {
        WriteToLog("VShader: Could not create source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
/*
	m_pSrcVB1 = CreateVertexBuffer(0, sizeof(VERTEX)*m_dwVertices, 0, false);
    if (NULL == m_pSrcVB1)
    {
        WriteToLog("VShader: Could not create source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pSrcVB1->Lock(&pBuf))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pShape->m_pVertices, m_dwVertices*sizeof(VERTEX));
    m_pSrcVB1->Unlock();
    
    m_pRefVB1 = CreateVertexBuffer(0, sizeof(VERTEX)*m_dwVertices, 0, true);
    if (NULL == m_pRefVB1)
    {
        WriteToLog("VShader: Could not create reference vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pRefVB1->Lock(&pBuf))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pShape->m_pVertices, m_dwVertices*sizeof(VERTEX));
    m_pRefVB1->Unlock();
*/    
    // Vertex buffer 2
    m_pSrcVB2 = CreateVertexBuffer(m_pDevice, NULL, m_dwVertices * sizeof(VERTEX1));
//	m_pSrcVB2 = CreateVertexBuffer(0, sizeof(VERTEX1)*m_dwVertices, 0, false);
    if (NULL == m_pSrcVB2)
    {
        WriteToLog("VShader: Could not create source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
//    if (!m_pSrcVB2->Lock(&pBuf))
    hr = m_pSrcVB2->Lock(0, sizeof(VERTEX1)*m_dwVertices, (LPBYTE*)&pBuf, 0);
    if (FAILED(hr))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < (int)m_dwVertices; i++)
	{
		((VERTEX1*)pBuf)[i].coord[0] = ((VERTEX0*)pShape->m_pVertices)[i].x;
		((VERTEX1*)pBuf)[i].coord[1] = ((VERTEX0*)pShape->m_pVertices)[i].y;
		((VERTEX1*)pBuf)[i].coord[2] = ((VERTEX0*)pShape->m_pVertices)[i].z;
		((VERTEX1*)pBuf)[i].coord[3] = 1.f;
		((VERTEX1*)pBuf)[i].normal = (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].nx + 1.f)*0x7f)) << 16) |
									 (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].ny + 1.f)*0x7f)) << 8) |
									 (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].nz + 1.f)*0x7f)) << 0);

	}
    m_pSrcVB2->Unlock();
/*    
    m_pRefVB2 = CreateVertexBuffer(0, sizeof(VERTEX1)*m_dwVertices, 0, true);
    if (NULL == m_pRefVB2)
    {
        WriteToLog("VShader: Could not create reference vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pRefVB2->Lock(&pBuf))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < m_dwVertices; i++)
	{
		((VERTEX1*)pBuf)[i].coord[0] = ((VERTEX0*)pShape->m_pVertices)[i].x;
		((VERTEX1*)pBuf)[i].coord[1] = ((VERTEX0*)pShape->m_pVertices)[i].y;
		((VERTEX1*)pBuf)[i].coord[2] = ((VERTEX0*)pShape->m_pVertices)[i].z;
		((VERTEX1*)pBuf)[i].coord[3] = 1.f;
		((VERTEX1*)pBuf)[i].normal = (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].nx + 1.f)*0x7f)) << 16) |
									 (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].ny + 1.f)*0x7f)) << 8) |
									 (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].nz + 1.f)*0x7f)) << 0);

	}
    m_pRefVB2->Unlock();
*/    
    // Vertex buffer 3
    m_pSrcVB3 = CreateVertexBuffer(m_pDevice, NULL, m_dwVertices * sizeof(VERTEX2));
//	m_pSrcVB3 = CreateVertexBuffer(0, sizeof(VERTEX2)*m_dwVertices, 0, false);
    if (NULL == m_pSrcVB3)
    {
        WriteToLog("VShader: Could not create source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
//    if (!m_pSrcVB3->Lock(&pBuf))
    hr = m_pSrcVB3->Lock(0, sizeof(VERTEX2)*m_dwVertices, (LPBYTE*)&pBuf, 0);
    if (FAILED(hr))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < (int)m_dwVertices; i++)
	{
		((VERTEX2*)pBuf)[i].coord[0] = (short)(((VERTEX0*)pShape->m_pVertices)[i].x * 10000.f);
		((VERTEX2*)pBuf)[i].coord[1] = (short)(((VERTEX0*)pShape->m_pVertices)[i].y * 10000.f);
		((VERTEX2*)pBuf)[i].coord[2] = (short)(((VERTEX0*)pShape->m_pVertices)[i].z * 10000.f);
		((VERTEX2*)pBuf)[i].coord[3] = 10000;
		((VERTEX2*)pBuf)[i].normal = (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].nx + 1.f)*0x7f)) << 0) |
									 (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].ny + 1.f)*0x7f)) << 8) |
									 (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].nz + 1.f)*0x7f)) << 16);
		((VERTEX2*)pBuf)[i].tex[0] = (short)(((VERTEX0*)pShape->m_pVertices)[i].tu * 10000.f);
		((VERTEX2*)pBuf)[i].tex[1] = (short)(((VERTEX0*)pShape->m_pVertices)[i].tv * 10000.f);
	}
    m_pSrcVB3->Unlock();
/*    
    m_pRefVB3 = CreateVertexBuffer(0, sizeof(VERTEX2)*m_dwVertices, 0, true);
    if (NULL == m_pRefVB2)
    {
        WriteToLog("VShader: Could not create reference vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pRefVB3->Lock(&pBuf))
    {
        WriteToLog("VShader: Could not lock source vertex buffer\n");
        return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < m_dwVertices; i++)
	{
		((VERTEX2*)pBuf)[i].coord[0] = (short)(((VERTEX0*)pShape->m_pVertices)[i].x * 10000.f);
		((VERTEX2*)pBuf)[i].coord[1] = (short)(((VERTEX0*)pShape->m_pVertices)[i].y * 10000.f);
		((VERTEX2*)pBuf)[i].coord[2] = (short)(((VERTEX0*)pShape->m_pVertices)[i].z * 10000.f);
		((VERTEX2*)pBuf)[i].coord[3] = 10000;
		((VERTEX2*)pBuf)[i].normal = (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].nx + 1.f)*0x7f)) << 0) |
									 (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].ny + 1.f)*0x7f)) << 8) |
									 (((BYTE)((((VERTEX0*)pShape->m_pVertices)[i].nz + 1.f)*0x7f)) << 16);
		((VERTEX2*)pBuf)[i].tex[0] = (short)(((VERTEX0*)pShape->m_pVertices)[i].tu * 10000.f);
		((VERTEX2*)pBuf)[i].tex[1] = (short)(((VERTEX0*)pShape->m_pVertices)[i].tv * 10000.f);
	}
    m_pRefVB3->Unlock();
*/    
    // Index buffer
    m_pSrcIB = CreateIndexBuffer(m_pDevice, pShape->m_pIndices, m_dwIndices * sizeof(WORD));
/*
	m_pSrcIB = CreateIndexBuffer(m_dwIndices, D3DFMT_INDEX16, false);
    if (NULL == m_pSrcIB)
    {
        WriteToLog("VShader: Could not create source index buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pSrcIB->Lock(&pBuf))
    {
        WriteToLog("VShader: Could not lock source index buffer");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pShape->m_pIndices, m_dwIndices*sizeof(WORD));
    m_pSrcIB->Unlock();

    m_pRefIB = CreateIndexBuffer(m_dwIndices, D3DFMT_INDEX16, true);
    if (NULL == m_pRefIB)
    {
        WriteToLog("VShader: Could not create reference index buffer\n");
        return D3DTESTINIT_ABORT;
    }
    if (!m_pRefIB->Lock(&pBuf))
    {
        WriteToLog("VShader: Could not lock reference index buffer\n");
        return D3DTESTINIT_ABORT;
    }
    memcpy(pBuf, pShape->m_pIndices, m_dwIndices*sizeof(WORD));
    m_pRefIB->Unlock();
*/    
    delete pShape;

    // Background texture
    DWORD   dwColors[8] = { 0x00ffffff, 0x00ff0000, 0x00ffff00, 0x0000ff00,
							0x00ff00ff, 0x0000ffff, 0x000000ff, 0x00000000,};
//    pImageData = Loader.LoadStripes(256, 256, 8, dwColors, false);
//	m_pTexture = CreateTexture(256,256,CDDS_TEXTURE_MANAGE,pImageData);
//	RELEASE(pImageData);
    m_pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, dwColors, false, false);
	if (m_pTexture == NULL)
    {
		WriteToLog("LightBall: Error creating RGB texture\n");
        return D3DTESTINIT_ABORT;
    }

    if (!CreateShaders())
    {
        return D3DTESTINIT_ABORT;
    }

	// Default settings
	SetRenderState(D3DRS_SPECULARENABLE, (DWORD)TRUE);
	SetTexture(0, m_pTexture);
	SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
	SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
	
	return D3DTESTINIT_RUN;
}

bool VShader::ExecuteTest(UINT uCurrentTest)
{
    DWORD   dwCount = 0;

    m_dwCurrentTestNumber = 1;//(DWORD)uCurrentTest;
    m_dwCurrentShader = 0;//(m_dwCurrentTestNumber-1)%m_dwShaders;

	if (m_pShaders[m_dwCurrentShader].bSupported == false)
	{
		SkipTests(1);
		return false;
	}


    BeginTestCase("");

    if (!SetCurrentShaderConstants())
    {
        WriteToLog("VShader: SetVertexShaderConstant failed\n");
        Fail();
        return false;
    }

    if (!SetVertexShader(m_pShaders[m_dwCurrentShader].dwHandle))
    {
        WriteToLog("VShader: SetVertexShader failed\n");
        Fail();
        return false;
    }
    return true;
}

bool VShader::SetCurrentShaderConstants()
{
    return true;
}


void VShader::SceneRefresh(void)
{
#ifndef UNDER_XBOX
    if (!SetCurrentShaderConstants())
    {
        WriteToLog("VShader: SetVertexShaderConstant failed\n");
        Fail();
        return;
    }

    if (!SetVertexShader(m_pShaders[m_dwCurrentShader].dwHandle))
    {
        WriteToLog("VShader: SetVertexShader failed\n");
        Fail();
        return;
    }
#endif


    ClearFrame();
    if (BeginScene())
    {
        if (m_pSrcVB != NULL)// && m_pRefVB != NULL)
			SetStreamSource(0, m_pSrcVB,  m_pRefVB,  sizeof(VERTEX0),  0);
        if (m_pSrcVB1 != NULL)// && m_pRefVB1 != NULL)
	        SetStreamSource(1, m_pSrcVB1, m_pRefVB1, sizeof(VERTEX0),  0);
        if (m_pSrcVB2 != NULL)// && m_pRefVB2 != NULL)
		    SetStreamSource(2, m_pSrcVB2, m_pRefVB2, sizeof(VERTEX1), 0);
        if (m_pSrcVB3 != NULL)// && m_pRefVB3 != NULL)
			SetStreamSource(3, m_pSrcVB3, m_pRefVB3, sizeof(VERTEX2), 0);
        if (m_pSrcIB != NULL)// && m_pRefIB != NULL)
	        SetIndices(m_pSrcIB, m_pRefIB, 0, 0);
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwVertices, 0, m_dwIndices/3);
        EndScene();
    }
}

bool VShader::ProcessFrame(void)
{
	bool    bResult;

    bResult = GetCompareResult(0.05f, 0.95f, 0);

    if (!bResult) {
		WriteToLog("Failed\n");
		Fail();
    }
	else {
		Pass();
	}
    WriteStatus("Vertex Shader", "%d", m_dwCurrentShader+1);

	return true;
}

bool VShader::TestTerminate(void)
{
    m_pDevice->SetVertexShader(FVF_VERTEX);
    ReleaseShaders();
//m_pDevice->SetStreamSource(0, NULL, 0);
    ReleaseVertexBuffer(m_pSrcVB);
    ReleaseVertexBuffer(m_pRefVB);
//m_pDevice->SetStreamSource(1, NULL, 0);
    ReleaseVertexBuffer(m_pSrcVB1);
    ReleaseVertexBuffer(m_pRefVB1);
//m_pDevice->SetStreamSource(2, NULL, 0);
    ReleaseVertexBuffer(m_pSrcVB2);
    ReleaseVertexBuffer(m_pRefVB2);
//m_pDevice->SetStreamSource(3, NULL, 0);
    ReleaseVertexBuffer(m_pSrcVB3);
    ReleaseVertexBuffer(m_pRefVB3);
//m_pDevice->SetIndices(NULL, 0);
    ReleaseIndexBuffer(m_pSrcIB);
    ReleaseIndexBuffer(m_pRefIB);
m_pDevice->SetTexture(0, NULL);
	ReleaseTexture(m_pTexture);
    return true;
}

//******************************************************************************
BOOL VShader::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void VShader::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL VShader::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_ESCAPE: // Exit
                    m_bExit = TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CD3DTest::WndProc(plr, hWnd, uMsg, wParam, lParam);
}

