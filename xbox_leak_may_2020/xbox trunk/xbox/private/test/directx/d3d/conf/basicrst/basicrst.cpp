#define STRICT
#define BURN_IN_HELL
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "BasicRst.h"

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

    CBasicRstTest* pBasicRst;
    BOOL           bQuit = FALSE, bRet = TRUE;
    UINT           i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 10 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pBasicRst = new CCullingTest();
                break;
            case 1:
                pBasicRst = new CDitherTest();
                break;
            case 2:
                pBasicRst = new CFillTest();
                break;
            case 3:
                pBasicRst = new CShadingTest();
                break;
            case 4:
                pBasicRst = new CZWriteTest();
                break;
            case 5:
                pBasicRst = new CMeshTLTest();
                break;
            case 6:
                pBasicRst = new CMeshLTest();
                break;
            case 7:
                pBasicRst = new CMeshD3DTest();
                break;
            case 8:
                pBasicRst = new CColorWriteTest();
                break;
            case 9:
                pBasicRst = new CNoDiffuseTest();
                break;
        }

        if (!pBasicRst) {
            return FALSE;
        }

        // Initialize the scene
        if (!pBasicRst->Create(pDisplay)) {
            pBasicRst->Release();
            return FALSE;
        }

        bRet = pBasicRst->Exhibit(pnExitCode);

        bQuit = pBasicRst->AbortedExit();

        // Clean up the scene
        pBasicRst->Release();
    }

    return bRet;
}

CBasicRstTest::CBasicRstTest()
{
    // Disable textures
    m_ModeOptions.fTextures = false;
    m_bExit = FALSE;
}

CBasicRstTest::~CBasicRstTest()
{
}

bool CBasicRstTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

void CBasicRstTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}
 
bool CBasicRstTest::ProcessFrame(void)
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

//******************************************************************************
BOOL CBasicRstTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CBasicRstTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CBasicRstTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

