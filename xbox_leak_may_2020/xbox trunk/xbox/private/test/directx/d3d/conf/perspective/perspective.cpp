#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Perspective.h"

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

    CPerspectiveTest*   pPerspectiveTest;
    BOOL                bQuit = FALSE, bRet = TRUE;
    UINT                i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 2 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pPerspectiveTest = new CTextureTest();
                break;
            case 1:
                pPerspectiveTest = new CColorTest();
                break;
        }

        if (!pPerspectiveTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pPerspectiveTest->Create(pDisplay)) {
            pPerspectiveTest->Release();
            return FALSE;
        }

        bRet = pPerspectiveTest->Exhibit(pnExitCode);

        bQuit = pPerspectiveTest->AbortedExit();

        // Clean up the scene
        pPerspectiveTest->Release();
    }

    return bRet;
}

CPerspectiveTest::CPerspectiveTest()
{
    pTexture = NULL;
    m_bExit = FALSE;
}

CPerspectiveTest::~CPerspectiveTest()
{
}

bool CPerspectiveTest::SetDefaultRenderStates(void)
{
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
	return true;
}

bool CPerspectiveTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

bool CPerspectiveTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    dwVertexCount = 0;
	msgString[0] = '\0';

    if (m_ModeOptions.fTextures)
    {
    	// Cleanup previous texture
	    SetTexture(0, NULL);
        ReleaseTexture(pTexture);

	    // Let's build some triangles.
	    if (uTestNum <= MAX_GRID_TESTS * 4)
	    {
		    // Build the texturing grid
		    DrawTexGrid(uTestNum); 
	    }
	    else
	    {
		    // Build the triangle
		    DrawTriangle(uTestNum - (MAX_GRID_TESTS * 4)); 
	    }

	    // Create and set the texture
//        pTexture = CreateTexture(pImage->GetWidth(),pImage->GetHeight(),CDDS_TEXTURE_VIDEO,pImage);
//        pTexture = CreateStripedTexture(m_pDevice, 64, 64, 8, Stripes, false, false, m_fmtCommon[nTextureFormat]);
        pTexture = CreateStripedTexture(m_pDevice, 64, 64, 8, Stripes, false, false, D3DFMT_A8R8G8B8);

	    // Tell the framework what texture to use
        if (NULL != pTexture)
	    {
            SetTexture(0,pTexture);

//            if (m_dwVersion <= 0x0700)
//            {
//    		    sprintf(msgString, "%sTex: ",msgString);
//        	    GetTexFmtDesc(&m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat,msgString+strlen(msgString),80-strlen(msgString));
//            }
//            else
//    		    sprintf(msgString, "%sTex: %s",msgString,D3DFmtToString(m_fmtCommon[nTextureFormat]));
    		    sprintf(msgString, "%sTex: %s",msgString,D3DFmtToString(D3DFMT_A8R8G8B8));
	    }
	    else
	    {
		    WriteToLog("CreateTexture() failed with HResult = %X.\n",GetLastError());
		    TestFinish(D3DTESTFINISH_ABORT);
		    return false;
	    }
    }
    else
    {
	    // Let's build some triangles.
	    if (uTestNum <= MAX_GRID_TESTS)
	    {
		    // Build the color grid
		    DrawColorGrid(uTestNum); 
	    }
	    else
	    {
		    // Build the triangle
		    DrawTriangle(uTestNum - MAX_GRID_TESTS); 
	    }
    }

    // Tell the log that we are starting
    BeginTestCase(msgString);

    return true;
}

void CPerspectiveTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CPerspectiveTest::ProcessFrame(void)
{
	char 	szBuffer[80];
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

bool CPerspectiveTest::TestTerminate()
{
	// Cleanup texture & image data
    if (m_ModeOptions.fTextures)
    {
	    SetTexture(0, NULL);
        ReleaseTexture(pTexture);
        pTexture = NULL;
//        RELEASE(pImage);
    }

	return true;
}

//******************************************************************************
BOOL CPerspectiveTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CPerspectiveTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CPerspectiveTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
