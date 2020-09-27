#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Texform.h"

//CD3DWindowFramework theApp;
//Texform             TestOne;

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

    Texform* TestOne;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    TestOne = new Texform();
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

Texform::Texform()
{
//    m_pLight = NULL;
//    m_pMaterial = NULL;
    m_pTexture = NULL;
    m_pShape = NULL;
    m_pVertices = NULL;

    m_dwCurrentTestNumber = 0;
    m_pszTestName = "";

    m_szTestName = TEXT("Texform");
    m_szCommandKey = TEXT("texform");

}

Texform::~Texform()
{
    if (m_pShape) 
    {
        delete m_pShape;
        m_pShape = NULL;
    }
    if (m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
    }
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
	ReleaseTexture(m_pTexture);
}


UINT Texform::TestInitialize(void)
{
//	CImageData*     pImageData;
//    CImageLoader    Loader;

    SetTestRange(1, 1000);

	// Create primitive to render
    m_pShape = new CShapes;
	if (NULL == m_pShape)
    {
		WriteToLog("Texform: Not enough memory to create primitive\n");
        return D3DTESTINIT_ABORT;
    }
	if (!m_pShape->NewShape(CS_SPHERE))
    {
		WriteToLog("Texform: Can't create sphere vertices\n");
        return D3DTESTINIT_ABORT;
    }

    // Textures
    DWORD dwColors[7] = {
        RGBA_MAKE(  0,   0, 255, 255), 
        RGBA_MAKE(  0, 255,   0, 255), 
        RGBA_MAKE(  0, 255, 255, 255), 
        RGBA_MAKE(255,   0,   0, 255), 
        RGBA_MAKE(255,   0, 255, 255), 
        RGBA_MAKE(255, 255,   0, 255), 
        RGBA_MAKE(255, 255, 255, 255)
    };
    // Allocate buffer for vertices
    m_pVertices = new BYTE [sizeof(float) * 10 * m_pShape->m_nVertices];
    if (NULL == m_pVertices)
    {
        WriteToLog("Texform: Can't allocate memory for vertices");
        return D3DTESTINIT_ABORT;
    }

/*
    pImageData = Loader.LoadStripes(256, 256, 7, dwColors, false, true);
    if (NULL == pImageData)
    {
        WriteToLog("Texform: Can't create source image\n");
        return D3DTESTINIT_ABORT;
    }
	m_pTexture = CreateTexture(256,256,CDDS_TEXTURE_MANAGE,pImageData);
	RELEASE(pImageData);
	if (m_pTexture == NULL)
    {
		WriteToLog("Texform: Error creating RGB texture\n");
        return D3DTESTINIT_ABORT;
    }
*/
    m_pTexture = CreateStripedTexture(m_pDevice, 256, 256, 7, dwColors, false, true);
    if (!m_pTexture)
    {
		WriteToLog("Texform: Error creating RGB texture\n");
        return D3DTESTINIT_ABORT;
    }

	return D3DTESTINIT_RUN;
}



bool Texform::ExecuteTest(UINT iTestNumber)
{
	m_dwCurrentTestNumber = (DWORD)iTestNumber;
	
    if (!SetProperties())
        return false;
	BeginTestCase(m_pszTestName);

    return true;
}

void Texform::SceneRefresh(void)
{
    
	SetTexture(0, m_pTexture);
    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
    
    if (BeginScene())
	{
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                D3DMATRIX   M = TranslateMatrix(0.5f*i-0.5f, 0.5f*j-0.5f, 0.f);
                SetTransform(D3DTRANSFORMSTATE_WORLD, &M);
                SetVertices();
                RenderPrimitive(D3DPT_TRIANGLELIST, m_dwFVF, 
                    m_pVertices, m_pShape->m_nVertices,
                    m_pShape->m_pIndices, m_pShape->m_nIndices, 0);
            }
        }
        
        EndScene();
	}
}

bool Texform::ProcessFrame(void)
{
	bool    bResult = GetCompareResult(0.1f, 0.78f, 0);

    if (!bResult) {
		WriteToLog("Test fails on frame %d\n", m_dwCurrentTestNumber);
		Fail();
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}



bool Texform::TestTerminate(void)
{
    if (m_pShape) 
    {
        delete m_pShape;
        m_pShape = NULL;
    }
    if (m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
    }
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
	ReleaseTexture(m_pTexture);
    m_pTexture = NULL;
	return true;
}
