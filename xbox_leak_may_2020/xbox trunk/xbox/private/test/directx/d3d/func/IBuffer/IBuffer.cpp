#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "IBuffer.h"

//CD3DWindowFramework	theApp;
//IBuffer             TestOne;

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

    IBuffer* pIBuffer;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pIBuffer = new IBuffer();
    if (!pIBuffer) {
        return FALSE;
    }

    // Initialize the scene
    if (!pIBuffer->Create(pDisplay)) {
        pIBuffer->Release();
        return FALSE;
    }

    bRet = pIBuffer->Exhibit(pnExitCode);

    // Clean up the scene
    pIBuffer->Release();

    return bRet;
}

IBuffer::IBuffer()
{
//	m_pMaterial = NULL;
//    m_pLight = NULL;
    for (int i = 0; i < SHAPES_NUMBER; i++)
        m_pShapes[i] = NULL;
    m_pSrcVertexBuffer = NULL;
    m_pRefVertexBuffer = NULL;
    m_pSrcIndexBuffer = NULL;
    m_pRefIndexBuffer = NULL;

    m_eIndexPool = D3DPOOL_DEFAULT;
    m_eVertexPool = D3DPOOL_DEFAULT;
    m_dwUsageScenario = 0;
    m_bRecreateBuffers = true;
    m_bFillSumBuffer = true;

    m_szTestName = TEXT("IBuffer");
	m_szCommandKey = TEXT("ibuffer");
}

IBuffer::~IBuffer()
{
}


UINT IBuffer::TestInitialize(void)
{
    CS_SHAPES eShape;

    SetTestRange(1, 360);

    m_dwMaxVertices = 0;
    m_dwSumVertices = 0;
    m_dwMaxIndices = 0;
    m_dwSumIndices = 0;
    for (int i = 0; i < SHAPES_NUMBER; i++)
    {
        m_pShapes[i] = new CShapes;
        if (NULL == m_pShapes[i])
            return D3DTESTINIT_ABORT;
        
        switch (i)
        {
        case 0: eShape = CS_BOX;            break;
        case 1: eShape = CS_SPHERE;         break;
        case 2: eShape = CS_PYRAMID;        break;
        case 3: eShape = CS_CYLINDER;       break;
        case 4: eShape = CS_CONE;           break;
        case 5: eShape = CS_MESH;           break;
        case 6: eShape = CS_BOID;           break;
        case 7: eShape = CS_ICOSOHEDRON;    break;
        }
        
        if (!m_pShapes[i]->NewShape(eShape, 65, 65)) {
            return D3DTESTINIT_ABORT;
        }

        m_pShapes[i]->Rotate(0.5f, 0.5f, 0.f);

        m_dwMaxVertices = (m_dwMaxVertices > (DWORD)m_pShapes[i]->m_nVertices) ? m_dwMaxVertices : (DWORD)m_pShapes[i]->m_nVertices;
        m_dwMaxIndices = (m_dwMaxIndices > (DWORD)m_pShapes[i]->m_nIndices) ? m_dwMaxIndices : (DWORD)m_pShapes[i]->m_nIndices;
        m_dwSumVertices += (DWORD)m_pShapes[i]->m_nVertices;
        m_dwSumIndices += (DWORD)m_pShapes[i]->m_nIndices;
    }
   
    return D3DTESTINIT_RUN;
}

bool IBuffer::ExecuteTest(UINT iTestNumber)
{
    DWORD   dwSkip;

    m_dwCurrentTestNumber = (DWORD)iTestNumber;
    dwSkip = ResolveTestNumber();
    if (dwSkip != 0)
    {
        SkipTests(dwSkip);
        return false;
    }

    BeginTestCase("IBuffer");

    if (m_bRecreateBuffers && !CreateBuffers())
    {
        Fail();
        EndTestCase();
        return false;
    }

    if (!FillBuffers())
    {
        Fail();
        EndTestCase();
        return false;
    }


    return true;
}


void IBuffer::SceneRefresh(void)
{
    if (BeginScene())
    {
        SetVertexShader(FVF_IBVERTEX);
        SetStreamSource(0, m_pSrcVertexBuffer, m_pRefVertexBuffer, GetVertexSize(FVF_IBVERTEX), 0);
        SetIndices(m_pSrcIndexBuffer, m_pRefIndexBuffer, 0, 0);
//        DrawIndexedPrimitive8(D3DPT_TRIANGLELIST, m_dwMinIndex, m_dwRenderVertices, m_dwStartIndex, m_dwRenderIndices/3, 0);
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_dwMinIndex, m_dwRenderVertices, m_dwStartIndex, m_dwRenderIndices / 3);
        EndScene();
    }
}

bool IBuffer::ProcessFrame(void)
{
	bool    bResult;

    // This range (0.00002) basically allows 1 pixel to be different
    bResult = GetCompareResult(0.15f);

    if (!bResult) {
		WriteToLog("IBuffer: Test failed\n");
		Fail();
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}

bool IBuffer::TestTerminate(void)
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < SHAPES_NUMBER; i++)
    {
        if (m_pShapes[i])
        {
            delete m_pShapes[i];
            m_pShapes[i] = NULL;
        }
    }
    ReleaseVertexBuffer(m_pSrcVertexBuffer);
    m_pSrcVertexBuffer = NULL;
    ReleaseVertexBuffer(m_pRefVertexBuffer);
    m_pRefVertexBuffer = NULL;
    ReleaseIndexBuffer(m_pSrcIndexBuffer);
    m_pSrcIndexBuffer = NULL;
    ReleaseIndexBuffer(m_pRefIndexBuffer);
    m_pRefIndexBuffer = NULL;
    return true;
}
