#define STRICT
#define BURN_IN_HELL
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Address.h"

// Global Test and App instance
//CD3DWindowFramework App;

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

    CAddressTest* pAddrTest;
    BOOL          bQuit = FALSE, bRet = TRUE;
    UINT          i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 19 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pAddrTest = new CWrapUTest();
                break;
            case 1:
                pAddrTest = new CWrapVTest();
                break;
            case 2:
                pAddrTest = new CWrapUVTest();
                break;
            case 3:
                pAddrTest = new CWrapTest();
                break;
            case 4:
                pAddrTest = new CMirrorTest();
                break;
            case 5:
                pAddrTest = new CClampTest();
                break;
            case 6:
                pAddrTest = new CBorderTest();
                break;
            case 7:
                pAddrTest = new CWrapMirrorTest();
                break;
            case 8:
                pAddrTest = new CWrapClampTest();
                break;
            case 9:
                pAddrTest = new CWrapBorderTest();
                break;
            case 10:
                pAddrTest = new CMirrorWrapTest();
                break;
            case 11:
                pAddrTest = new CMirrorClampTest();
                break;
            case 12:
                pAddrTest = new CMirrorBorderTest();
                break;
            case 13:
                pAddrTest = new CClampWrapTest();
                break;
            case 14:
                pAddrTest = new CClampMirrorTest();
                break;
            case 15:
                pAddrTest = new CClampBorderTest();
                break;
            case 16:
                pAddrTest = new CBorderWrapTest();
                break;
            case 17:
                pAddrTest = new CBorderMirrorTest();
                break;
            case 18:
                pAddrTest = new CBorderClampTest();
                break;
        }

        if (!pAddrTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pAddrTest->Create(pDisplay)) {
            pAddrTest->Release();
            return FALSE;
        }

        bRet = pAddrTest->Exhibit(pnExitCode);

        bQuit = pAddrTest->AbortedExit();

        // Clean up the scene
        pAddrTest->Release();
    }

    return bRet;
}

CAddressTest::CAddressTest()
{
	// Init variables to false
	bBorder = false;
	bWrapUV = false;
	bIndependentUV = false;

	// Init the AddrU & AddrV strings to empty
	szAddrU = TEXT("");
	szAddrV = TEXT("");

    pTexture = NULL;

    m_bExit = FALSE;
}

CAddressTest::~CAddressTest()
{
    ReleaseTexture(pTexture);
    pTexture = NULL;
}

UINT CAddressTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	if (bWrapUV)
		SetTestRange(1, 4);
//		SetTestRange(1, 4 * m_uCommonTextureFormats);
	else
		SetTestRange(1, MAX_TESTS);
//		SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwTextureAddressCaps = m_d3dcaps.TextureAddressCaps;

	// Check the appropriate Address caps
	if (bIndependentUV)
	{
		if (!(dwTextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV))
		{
			WriteToLog("Device capability not supported: Texture Address IndependentUV.\n");
			return D3DTESTINIT_SKIPALL;
		}

		if (!(dwTextureAddressCaps & dwAddressCapU))
		{
			WriteToLog("Device capability not supported: Texture Address %s.\n",szAddrU);
			return D3DTESTINIT_SKIPALL;
		}

		if (!(dwTextureAddressCaps & dwAddressCapV))
		{
			WriteToLog("Device capability not supported: Texture Address %s.\n",szAddrV);
			return D3DTESTINIT_SKIPALL;
		}
	}
	else if (!(dwTextureAddressCaps & dwAddressCapU) && !(dwTextureAddressCaps & dwAddressCapV))
	{
		WriteToLog("Device capability not supported: %s.\n",m_szTestName);
		return D3DTESTINIT_SKIPALL;
	}

	// Tell RenderPrimitive disable API cycling
	// m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	// Setup the colors for the Gradient array
	Gradient[0] = RGBA_MAKE(0,0,0,255);		// Black
	Gradient[1] = RGBA_MAKE(255,0,0,255);	// Red
	Gradient[2] = RGBA_MAKE(0,255,0,255);	// Green
	Gradient[3] = RGBA_MAKE(255,255,0,255);	// Yellow

	// Create the stripe image data
//    CImageLoader    Image;
  
//    pImage = Image.LoadGradient(64,64,Gradient); 

	return D3DTESTINIT_RUN;
}

bool CAddressTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

bool CAddressTest::ExecuteTest(UINT uTestNum)
{
   // Setup the tests to execute.
	int nTest;

	// Initialize some variables
	nTest = ((uTestNum-1) / 1) + 1;
//	nTest = ((uTestNum-1) m_uCommonTextureFormats) + 1;
 	dwVertexCount = 0;
	msgString[0] = 0;
	wsprintf(msgString, TEXT(""));

	// Cleanup previous texture
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);

	// Let's build some triangles.
	if (bWrapUV)
		DrawWrapGrid(nTest);
	else
		DrawGrid(nTest);

	// Cycle the texture formats
//	m_pD3DMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

	// Create and set the texture
    pTexture = CreateGradientTexture(m_pDevice, 64, 64, Gradient);
//    pTexture = CreateTexture(pImage->GetWidth(),pImage->GetHeight(), CDDS_TEXTURE_VIDEO,pImage);

	// Tell the framework what texture to use
    if (NULL != pTexture)
	{
        SetTexture(0,pTexture);

        if (m_dwVersion <= 0x0700)
        {
//    		sprintf(msgString, "%sTex:",msgString);
//        	GetTexFmtDesc(&m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat,msgString+strlen(msgString),80-strlen(msgString));
        }
        else
    		wsprintf(msgString, TEXT("%sTex:%s"),msgString,TEXT("D3DFMT_A8R8G8B8"));//D3DFmtToString(m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat));
	}
	else
	{
		WriteToLog(TEXT("CreateTexture() failed with 0x%X\n"), GetLastError());
//		TestFinish(D3DTESTFINISH_ABORT);
		return false;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CAddressTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CAddressTest::ProcessFrame(void)
{
	char	szBuffer[80];
	static	int nPass = 0;
	static  int nFail = 0;
    bool	bResult;

     // Use the standard 15%
    bResult = GetCompareResult(0.15f, 0.78f, 0);

	// Tell the logfile how the compare went
    if (bResult)
    {
		(m_fIncrementForward) ? nPass++ : nPass--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
	else
	{
		(m_fIncrementForward) ? nFail++ : nFail--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
	}

	WriteStatus("$gOverall Results",szBuffer);

    return bResult;
}

bool CAddressTest::TestTerminate()
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;
//    RELEASE(pImage);

	return true;
}

//******************************************************************************
BOOL CAddressTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CAddressTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CAddressTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
