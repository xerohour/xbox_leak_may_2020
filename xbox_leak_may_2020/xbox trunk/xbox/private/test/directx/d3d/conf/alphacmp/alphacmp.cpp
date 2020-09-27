#define STRICT
#define BURN_IN_HELL
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaCmp.h"

// Global Test and App instance
//CD3DWindowFramework		App;

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

    CAlphaCmpTest* pAlphaCmp;
    BOOL           bQuit = FALSE, bRet = TRUE;
    UINT           i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 8 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pAlphaCmp = new CAlphaNeverTest();
                break;
            case 1:
                pAlphaCmp = new CAlphaLessTest();
                break;
            case 2:
                pAlphaCmp = new CAlphaEqualTest();
                break;
            case 3:
                pAlphaCmp = new CAlphaLessEqualTest();
                break;
            case 4:
                pAlphaCmp = new CAlphaGreaterTest();
                break;
            case 5:
                pAlphaCmp = new CAlphaNotEqualTest();
                break;
            case 6:
                pAlphaCmp = new CAlphaGreaterEqualTest();
                break;
            case 7:
                pAlphaCmp = new CAlphaAlwaysTest();
                break;
        }

        if (!pAlphaCmp) {
            return FALSE;
        }

        // Initialize the scene
        if (!pAlphaCmp->Create(pDisplay)) {
            pAlphaCmp->Release();
            return FALSE;
        }

        bRet = pAlphaCmp->Exhibit(pnExitCode);

        bQuit = pAlphaCmp->AbortedExit();

        // Clean up the scene
        pAlphaCmp->Release();
    }

    return bRet;
}

CAlphaCmpTest::CAlphaCmpTest()
{
	// Init to flat shading
	bGouraud = false;

	// Init Alpha value array
	nAlphaValue[0] = 1;
	nAlphaValue[1] = 85;
	nAlphaValue[2] = 128;
	nAlphaValue[3] = 170;
	nAlphaValue[4] = 254;

    // Disable textures
    m_ModeOptions.fTextures = false;

    m_bExit = FALSE;
}

CAlphaCmpTest::~CAlphaCmpTest()
{
}

bool CAlphaCmpTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);
	return true;
}

UINT CAlphaCmpTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Are we using Gouraud?
    if (KeySet("Gouraud"))
		bGouraud = true;

	// Setup the Test range
	if (bGouraud)
		SetTestRange(1,MAX_TESTS);
	else
		SetTestRange(1,MAX_TESTS * 3);

	// Check the caps we are interested in
	DWORD dwAlphaCmpCaps = m_d3dcaps.AlphaCmpCaps;

	// Check to see if Alpha testing is supported at all
	if ((dwAlphaCmpCaps == D3DPCMPCAPS_NEVER) || (dwAlphaCmpCaps == D3DPCMPCAPS_ALWAYS))
	{
		WriteToLog("Alpha testing is not supported.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check the appropriate Alpha Compare cap
	if (!(dwAlphaCmpCaps & dwAlphaCmpCap))
	{
		WriteToLog("Device capability not supported: %s.\n",m_szTestName);
		return D3DTESTINIT_SKIPALL;
	}
  
    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool CAlphaCmpTest::ExecuteTest(UINT uTestNum)
{
	int nTest, nRefValue;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	if (bGouraud)
	{
		nTest = uTestNum - 1;
		nRefValue = nAlphaValue[nTest];

		if (nRefValue == 1)
			nRefValue = 0;
		if (nRefValue == 254)
			nRefValue = 255;
	}
	else
	{
		nTest = (uTestNum - 1) / 3;
		nRefValue = nAlphaValue[nTest] + ((uTestNum - 1) % 3) - 1;
	}

	// Set the Reference Alpha Value
	SetRenderState(D3DRENDERSTATE_ALPHAREF, (DWORD)nRefValue);

	// Let's build some triangles
	DrawTriangle(nAlphaValue[nTest]);

	// Update the log and status window
	if (bGouraud)
	{
//		sprintf(msgString, "%sAlphaRef: %d, Green Triangle: top %d, bottom %d",msgString,nRefValue,nAlphaValue[nTest]-1,nAlphaValue[nTest]+1);
//		sprintf(szStatus, "%sAlphaRef: %d, Green Triangle: top %d, bottom %d",szStatus,nRefValue,nAlphaValue[nTest]-1,nAlphaValue[nTest]+1);

		sprintf(msgString, "%sAlphaRef: %d, Green Triangle: top 0, bottom 255",msgString,nRefValue);
		sprintf(szStatus, "%sAlphaRef: %d, Green Triangle: top 0, bottom 255",szStatus,nRefValue);
	}
	else
	{
		sprintf(msgString, "%sAlphaRef: %d, Green Triangle: %d",msgString,nRefValue,nAlphaValue[nTest]);
		sprintf(szStatus, "%sAlphaRef: %d, Green Triangle: %d",szStatus,nRefValue,nAlphaValue[nTest]);
	}


	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CAlphaCmpTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CAlphaCmpTest::ProcessFrame(void)
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

	WriteStatus("$gAlpha Values$y",szStatus);
	WriteStatus("$gOverall Results",szBuffer);

    return bResult;
}

//******************************************************************************
BOOL CAlphaCmpTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CAlphaCmpTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CAlphaCmpTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

