#define STRICT
#define BURN_IN_HELL
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Mapping.h"

// Global Test and App instance
//CD3DWindowFramework	App;

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

    CMappingTest*   pMapTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 2 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pMapTest = new CMapPointTest();
                break;
            case 1:
                pMapTest = new CMapLinearTest();
                break;
        }

        if (!pMapTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pMapTest->Create(pDisplay)) {
            pMapTest->Release();
            return FALSE;
        }

        bRet = pMapTest->Exhibit(pnExitCode);

        bQuit = pMapTest->AbortedExit();

        // Clean up the scene
        pMapTest->Release();
    }

    return bRet;
}

CMappingTest::CMappingTest()
{
	// Initialize the UV array's values
	float fZero = 0.0f;
	float fOne = 1.0f - 0.001f;

	TopLeft[0] = UV(fZero, fZero);
	TopLeft[1] = UV(fZero, fOne);
	TopLeft[2] = UV(fOne,  fOne);
	TopLeft[3] = UV(fOne,  fZero);

	TopRight[0] = UV(fOne,  fZero);
	TopRight[1] = UV(fZero, fZero);
	TopRight[2] = UV(fZero, fOne);
	TopRight[3] = UV(fOne,  fOne);

	BottomLeft[0] = UV(fZero, fOne);
	BottomLeft[1] = UV(fOne,  fOne);
	BottomLeft[2] = UV(fOne,  fZero);
	BottomLeft[3] = UV(fZero, fZero);

	BottomRight[0] = UV(fOne,  fOne);
	BottomRight[1] = UV(fOne,  fZero);
	BottomRight[2] = UV(fZero, fZero);
	BottomRight[3] = UV(fZero, fOne);

    pTexture = NULL;
    m_bExit = FALSE;
}

CMappingTest::~CMappingTest()
{
}
 
UINT CMappingTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1, MAX_TESTS);
//	SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwTextureFilterCaps = m_d3dcaps.TextureFilterCaps;

	// Check the appropriate Min Filter cap
	if (!(dwTextureFilterCaps & dwMinFilterCap))
	{
		WriteToLog("Device capability not supported: (Min) Texture Filter %s.\n",m_szCommandKey);
		return D3DTESTINIT_SKIPALL;
	}

	// Check the appropriate Mag Filter cap
	if (!(dwTextureFilterCaps & dwMagFilterCap))
	{
		WriteToLog("Device capability not supported: (Mag) Texture Filter %s.\n",m_szCommandKey);
		return D3DTESTINIT_SKIPALL;
	}

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	// Setup the colors for the Gradient array
//	D3DCOLOR	Gradient[4];

	Gradient[0] = RGBA_MAKE(0,0,0,255);		// Black
	Gradient[1] = RGBA_MAKE(255,0,0,255);	// Red
	Gradient[2] = RGBA_MAKE(0,255,0,255);	// Green
	Gradient[3] = RGBA_MAKE(255,255,0,255);	// Yellow

	// Create the stripe image data
//    CImageLoader    Image;

//    pImage = Image.LoadGradient(64,64,Gradient); 

	return D3DTESTINIT_RUN;
}

bool CMappingTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

bool CMappingTest::ExecuteTest(UINT uTestNum)
{
   // Setup the tests to execute.
	int nTest;
    int nTextureFormat;

	// Initialize some variables
	nTest = ((uTestNum-1) / 1) + 1;
//	nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
 	dwVertexCount = 0;
	msgString[0] = 0;
	sprintf(msgString, "");

	// Cleanup previous texture
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);

	// Let's build some triangles.
	DrawGrid(nTest, 1); 
//	DrawGrid(nTest, m_uCommonTextureFormats);

	// Cycle the texture formats
//	m_pD3DMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;
	nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

	// Create and set the texture
//    pTexture = CreateTexture(pImage->GetWidth(),pImage->GetHeight(),CDDS_TEXTURE_VIDEO,pImage);
    pTexture = CreateGradientTexture(m_pDevice, 64, 64, Gradient, m_fmtCommon[nTextureFormat]);

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
    		sprintf(msgString, "%sTex:%s",msgString,D3DFmtToString(m_fmtCommon[nTextureFormat]));
	}
	else
	{
		WriteToLog("CreateTexture() failed with HResult = %X.\n",GetLastError());
		TestFinish(D3DTESTFINISH_ABORT);
		return false;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CMappingTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CMappingTest::ProcessFrame(void)
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

bool CMappingTest::TestTerminate()
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
//    RELEASE(pImage);

	return true;
}

//******************************************************************************
BOOL CMappingTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CMappingTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CMappingTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
