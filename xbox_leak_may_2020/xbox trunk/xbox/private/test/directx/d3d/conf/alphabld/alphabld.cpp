#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "False.h"
#include "Zero.h"
#include "One.h"
#include "SrcColor.h"
#include "InvSrcColor.h"
#include "SrcAlpha.h"
#include "InvSrcAlpha.h"
#include "DestAlpha.h"
#include "InvDestAlpha.h"
#include "DestColor.h"
#include "InvDestColor.h"
#include "SrcAlphaSat.h"
#include "Both.h"

// Global App instance
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

    CAlphaBldTest* pAlphaTest;
    BOOL           bQuit = FALSE, bRet = TRUE;
    UINT           i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

#ifndef UNDER_XBOX
    for (i = 1; i < 125 && !bQuit && bRet; i++) {
#else
    for (i = 1; i < 123 && !bQuit && bRet; i++) {
#endif // UNDER_XBOX

        // Create the scene
        switch (i) {
            case 1:
                pAlphaTest = new CFalseTest();
                break;
            case 2:
                pAlphaTest = new CZeroZeroTest();
                break;
            case 3:
                pAlphaTest = new CZeroOneTest();
                break;
            case 4:
                pAlphaTest = new CZeroSrcColorTest();
                break;
            case 5:
                pAlphaTest = new CZeroInvSrcColorTest();
                break;
            case 6:
                pAlphaTest = new CZeroSrcAlphaTest();
                break;
            case 7:
                pAlphaTest = new CZeroInvSrcAlphaTest();
                break;
            case 8:
                pAlphaTest = new CZeroDestAlphaTest();
                break;
            case 9:
                pAlphaTest = new CZeroInvDestAlphaTest();
                break;
            case 10:
                pAlphaTest = new CZeroDestColorTest();
                break;
            case 11:
                pAlphaTest = new CZeroInvDestColorTest();
                break;
            case 12:
                pAlphaTest = new CZeroSrcAlphaSatTest();
                break;
            case 13:
                pAlphaTest = new COneZeroTest();
                break;
            case 14:
                pAlphaTest = new COneOneTest();
                break;
            case 15:
                pAlphaTest = new COneSrcColorTest();
                break;
            case 16:
                pAlphaTest = new COneInvSrcColorTest();
                break;
            case 17:
                pAlphaTest = new COneSrcAlphaTest();
                break;
            case 18:
                pAlphaTest = new COneInvSrcAlphaTest();
                break;
            case 19:
                pAlphaTest = new COneDestAlphaTest();
                break;
            case 20:
                pAlphaTest = new COneInvDestAlphaTest();
                break;
            case 21:
                pAlphaTest = new COneDestColorTest();
                break;
            case 22:
                pAlphaTest = new COneInvDestColorTest();
                break;
            case 23:
                pAlphaTest = new COneSrcAlphaSatTest();
                break;
            case 24:
                pAlphaTest = new CSrcColorZeroTest();
                break;
            case 25:
                pAlphaTest = new CSrcColorOneTest();
                break;
            case 26:
                pAlphaTest = new CSrcColorSrcColorTest();
                break;
            case 27:
                pAlphaTest = new CSrcColorInvSrcColorTest();
                break;
            case 28:
                pAlphaTest = new CSrcColorSrcAlphaTest();
                break;
            case 29:
                pAlphaTest = new CSrcColorInvSrcAlphaTest();
                break;
            case 30:
                pAlphaTest = new CSrcColorDestAlphaTest();
                break;
            case 31:
                pAlphaTest = new CSrcColorInvDestAlphaTest();
                break;
            case 32:
                pAlphaTest = new CSrcColorDestColorTest();
                break;
            case 33:
                pAlphaTest = new CSrcColorInvDestColorTest();
                break;
            case 34:
                pAlphaTest = new CSrcColorSrcAlphaSatTest();
                break;
            case 35:
                pAlphaTest = new CInvSrcColorZeroTest();
                break;
            case 36:
                pAlphaTest = new CInvSrcColorOneTest();
                break;
            case 37:
                pAlphaTest = new CInvSrcColorSrcColorTest();
                break;
            case 38:
                pAlphaTest = new CInvSrcColorInvSrcColorTest();
                break;
            case 39:
                pAlphaTest = new CInvSrcColorSrcAlphaTest();
                break;
            case 40:
                pAlphaTest = new CInvSrcColorInvSrcAlphaTest();
                break;
            case 41:
                pAlphaTest = new CInvSrcColorDestAlphaTest();
                break;
            case 42:
                pAlphaTest = new CInvSrcColorInvDestAlphaTest();
                break;
            case 43:
                pAlphaTest = new CInvSrcColorDestColorTest();
                break;
            case 44:
                pAlphaTest = new CInvSrcColorInvDestColorTest();
                break;
            case 45:
                pAlphaTest = new CInvSrcColorSrcAlphaSatTest();
                break;
            case 46:
                pAlphaTest = new CSrcAlphaZeroTest();
                break;
            case 47:
                pAlphaTest = new CSrcAlphaOneTest();
                break;
            case 48:
                pAlphaTest = new CSrcAlphaSrcColorTest();
                break;
            case 49:
                pAlphaTest = new CSrcAlphaInvSrcColorTest();
                break;
            case 50:
                pAlphaTest = new CSrcAlphaSrcAlphaTest();
                break;
            case 51:
                pAlphaTest = new CSrcAlphaInvSrcAlphaTest();
                break;
            case 52:
                pAlphaTest = new CSrcAlphaDestAlphaTest();
                break;
            case 53:
                pAlphaTest = new CSrcAlphaInvDestAlphaTest();
                break;
            case 54:
                pAlphaTest = new CSrcAlphaDestColorTest();
                break;
            case 55:
                pAlphaTest = new CSrcAlphaInvDestColorTest();
                break;
            case 56:
                pAlphaTest = new CSrcAlphaSrcAlphaSatTest();
                break;
            case 57:
                pAlphaTest = new CInvSrcAlphaZeroTest();
                break;
            case 58:
                pAlphaTest = new CInvSrcAlphaOneTest();
                break;
            case 59:
                pAlphaTest = new CInvSrcAlphaSrcColorTest();
                break;
            case 60:
                pAlphaTest = new CInvSrcAlphaInvSrcColorTest();
                break;
            case 61:
                pAlphaTest = new CInvSrcAlphaSrcAlphaTest();
                break;
            case 62:
                pAlphaTest = new CInvSrcAlphaInvSrcAlphaTest();
                break;
            case 63:
                pAlphaTest = new CInvSrcAlphaDestAlphaTest();
                break;
            case 64:
                pAlphaTest = new CInvSrcAlphaInvDestAlphaTest();
                break;
            case 65:
                pAlphaTest = new CInvSrcAlphaDestColorTest();
                break;
            case 66:
                pAlphaTest = new CInvSrcAlphaInvDestColorTest();
                break;
            case 67:
                pAlphaTest = new CInvSrcAlphaSrcAlphaSatTest();
                break;
            case 68:
                pAlphaTest = new CDestAlphaZeroTest();
                break;
            case 69:
                pAlphaTest = new CDestAlphaOneTest();
                break;
            case 70:
                pAlphaTest = new CDestAlphaSrcColorTest();
                break;
            case 71:
                pAlphaTest = new CDestAlphaInvSrcColorTest();
                break;
            case 72:
                pAlphaTest = new CDestAlphaSrcAlphaTest();
                break;
            case 73:
                pAlphaTest = new CDestAlphaInvSrcAlphaTest();
                break;
            case 74:
                pAlphaTest = new CDestAlphaDestAlphaTest();
                break;
            case 75:
                pAlphaTest = new CDestAlphaInvDestAlphaTest();
                break;
            case 76:
                pAlphaTest = new CDestAlphaDestColorTest();
                break;
            case 77:
                pAlphaTest = new CDestAlphaInvDestColorTest();
                break;
            case 78:
                pAlphaTest = new CDestAlphaSrcAlphaSatTest();
                break;
            case 79:
                pAlphaTest = new CInvDestAlphaZeroTest();
                break;
            case 80:
                pAlphaTest = new CInvDestAlphaOneTest();
                break;
            case 81:
                pAlphaTest = new CInvDestAlphaSrcColorTest();
                break;
            case 82:
                pAlphaTest = new CInvDestAlphaInvSrcColorTest();
                break;
            case 83:
                pAlphaTest = new CInvDestAlphaSrcAlphaTest();
                break;
            case 84:
                pAlphaTest = new CInvDestAlphaInvSrcAlphaTest();
                break;
            case 85:
                pAlphaTest = new CInvDestAlphaDestAlphaTest();
                break;
            case 86:
                pAlphaTest = new CInvDestAlphaInvDestAlphaTest();
                break;
            case 87:
                pAlphaTest = new CInvDestAlphaDestColorTest();
                break;
            case 88:
                pAlphaTest = new CInvDestAlphaInvDestColorTest();
                break;
            case 89:
                pAlphaTest = new CInvDestAlphaSrcAlphaSatTest();
                break;
            case 90:
                pAlphaTest = new CDestColorZeroTest();
                break;
            case 91:
                pAlphaTest = new CDestColorOneTest();
                break;
            case 92:
                pAlphaTest = new CDestColorSrcColorTest();
                break;
            case 93:
                pAlphaTest = new CDestColorInvSrcColorTest();
                break;
            case 94:
                pAlphaTest = new CDestColorSrcAlphaTest();
                break;
            case 95:
                pAlphaTest = new CDestColorInvSrcAlphaTest();
                break;
            case 96:
                pAlphaTest = new CDestColorDestAlphaTest();
                break;
            case 97:
                pAlphaTest = new CDestColorInvDestAlphaTest();
                break;
            case 98:
                pAlphaTest = new CDestColorDestColorTest();
                break;
            case 99:
                pAlphaTest = new CDestColorInvDestColorTest();
                break;
            case 100:
                pAlphaTest = new CDestColorSrcAlphaSatTest();
                break;
            case 101:
                pAlphaTest = new CInvDestColorZeroTest();
                break;
            case 102:
                pAlphaTest = new CInvDestColorOneTest();
                break;
            case 103:
                pAlphaTest = new CInvDestColorSrcColorTest();
                break;
            case 104:
                pAlphaTest = new CInvDestColorInvSrcColorTest();
                break;
            case 105:
                pAlphaTest = new CInvDestColorSrcAlphaTest();
                break;
            case 106:
                pAlphaTest = new CInvDestColorInvSrcAlphaTest();
                break;
            case 107:
                pAlphaTest = new CInvDestColorDestAlphaTest();
                break;
            case 108:
                pAlphaTest = new CInvDestColorInvDestAlphaTest();
                break;
            case 109:
                pAlphaTest = new CInvDestColorDestColorTest();
                break;
            case 110:
                pAlphaTest = new CInvDestColorInvDestColorTest();
                break;
            case 111:
                pAlphaTest = new CInvDestColorSrcAlphaSatTest();
                break;
            case 112:
                pAlphaTest = new CSrcAlphaSatZeroTest();
                break;
            case 113:
                pAlphaTest = new CSrcAlphaSatOneTest();
                break;
            case 114:
                pAlphaTest = new CSrcAlphaSatSrcColorTest();
                break;
            case 115:
                pAlphaTest = new CSrcAlphaSatInvSrcColorTest();
                break;
            case 116:
                pAlphaTest = new CSrcAlphaSatSrcAlphaTest();
                break;
            case 117:
                pAlphaTest = new CSrcAlphaSatInvSrcAlphaTest();
                break;
            case 118:
                pAlphaTest = new CSrcAlphaSatDestAlphaTest();
                break;
            case 119:
                pAlphaTest = new CSrcAlphaSatInvDestAlphaTest();
                break;
            case 120:
                pAlphaTest = new CSrcAlphaSatDestColorTest();
                break;
            case 121:
                pAlphaTest = new CSrcAlphaSatInvDestColorTest();
                break;
            case 122:
                pAlphaTest = new CSrcAlphaSatSrcAlphaSatTest();
                break;
#ifndef UNDER_XBOX
            case 123:
                pAlphaTest = new CBothSrcAlphaTest();
                break;
            case 124:
                pAlphaTest = new CBothInvSrcAlphaTest();
                break;
#endif // !UNDER_XBOX
        }

        if (!pAlphaTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pAlphaTest->Create(pDisplay)) {
            pAlphaTest->Release();
            return FALSE;
        }

        bRet = pAlphaTest->Exhibit(pnExitCode);

        bQuit = pAlphaTest->AbortedExit();

        // Clean up the scene
        pAlphaTest->Release();
    }

    return bRet;
}

CAlphaBldTest::CAlphaBldTest()
{
	// Init Enable variable
	nAlphaEnable = TRUE;

	// Init Alpha value array
	nAlphaValue[0] = 0;
	nAlphaValue[1] = 64;
	nAlphaValue[2] = 128;
	nAlphaValue[3] = 192;
	nAlphaValue[4] = 255;

    // Init BlendOp variables
    dwBlendOverride = 0;
    bBlendOps = false;
    nBlendOp = 0;

    // Init the BlendOp array
    BlendOp[0] = D3DBLENDOP_ADD;
    BlendOp[1] = D3DBLENDOP_SUBTRACT;
    BlendOp[2] = D3DBLENDOP_REVSUBTRACT;
    BlendOp[3] = D3DBLENDOP_MIN; 
    BlendOp[4] = D3DBLENDOP_MAX;

    // Check for BlendOverride key
    if (KeySet("add") && !dwBlendOverride)
        dwBlendOverride = D3DBLENDOP_ADD;

    if (KeySet("subtract") && !dwBlendOverride)
        dwBlendOverride = D3DBLENDOP_SUBTRACT;

    if (KeySet("revsubtract") && !dwBlendOverride)
        dwBlendOverride = D3DBLENDOP_REVSUBTRACT;

    if (KeySet("min") && !dwBlendOverride)
        dwBlendOverride = D3DBLENDOP_MIN;

    if (KeySet("max") && !dwBlendOverride)
        dwBlendOverride = D3DBLENDOP_MAX;

    // Init the Src and Dest names to empty string
	szSrcName = "";
	szDestName = "";

    // Disable textures
    m_ModeOptions.fTextures = false;

    m_bExit = FALSE;
}

CAlphaBldTest::~CAlphaBldTest()
{
}
/*
bool CAlphaBldTest::AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize)
{
	// For Dx8, may need to try all five BlendOps
    if ((m_dwVersion >= 0x0800))
    {
	    DWORD dwMiscCaps = m_d3dcaps.PrimitiveMiscCaps;

	    // Check if the BlendOp cap is supported
	    if (dwMiscCaps & D3DPMISCCAPS_BLENDOP)
        {
            bBlendOps = true;

            if (dwBlendOverride)
            {
                // Use the override BlendOp
                BlendOp[0] = dwBlendOverride;

        	    CD3DTest::AddModeList(pMode,pData,dwDataSize);
            }
            else
            {
                // Use all five BlendOps 
    	        CD3DTest::AddModeList(pMode,pData,dwDataSize);
		        CD3DTest::AddModeList(pMode,pData,dwDataSize);
    	        CD3DTest::AddModeList(pMode,pData,dwDataSize);
		        CD3DTest::AddModeList(pMode,pData,dwDataSize);
    	        CD3DTest::AddModeList(pMode,pData,dwDataSize);
            }
        }
        else
        {
    		// Just use the default BlendOp
	    	CD3DTest::AddModeList(pMode,pData,dwDataSize);
        }
    }
    else
    {
		// Let the framework handle stress mode
		CD3DTest::AddModeList(pMode,pData,dwDataSize);
	}

    return true;
}
*/
UINT CAlphaBldTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1, MAX_TESTS);

	// Check the caps we are interested in
	DWORD dwSrcBlendCaps = m_d3dcaps.SrcBlendCaps;
	DWORD dwDestBlendCaps = m_d3dcaps.DestBlendCaps;

	DWORD dwMiscCaps = m_d3dcaps.PrimitiveMiscCaps;

	// Check if the BlendOp cap is supported
	if (dwMiscCaps & D3DPMISCCAPS_BLENDOP)
    {
        bBlendOps = true;
    }

	// Check the appropriate Source Blend caps
	if (!(dwSrcBlendCaps & dwSrcCap))
	{
		WriteToLog("Device capability not supported: Source Blend %s.\n",szSrcName);
		return D3DTESTINIT_SKIPALL;
	}

	// Check the appropriate Dest Blend caps
	if (!(dwDestBlendCaps & dwDestCap))
	{
		WriteToLog("Device capability not supported: Destination Blend %s.\n",szDestName);
		return D3DTESTINIT_SKIPALL;
	}

	// Generate the checkerboard background 
//    if (KeySet("checker"))
//    	DrawBackground();

	return D3DTESTINIT_RUN;
}

bool CAlphaBldTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(116,116,116,255));
}

bool CAlphaBldTest::ExecuteTest(UINT uTestNum)
{
	int		nTest;
 
	// Initialize some variables
	nTest = (uTestNum - 1) % MAX_TESTS;
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';
	szBlend[0] = '\0';

//    if (bBlendOps)
        nBlendOp = (uTestNum - 1) / 6;
//    else
//        nBlendOp = 0;

	// Are we the circle test
	bCircle = (nTest % 6) ? false : true;

    // Tell log and status window which Blend Op we are using
    switch (BlendOp[nBlendOp])
    {
        case D3DBLENDOP_ADD: sprintf(szBlend, "Add (default)",szBlend);
                             sprintf(msgString, "%sBlendOp: Add, ",msgString);
                             break;

        case D3DBLENDOP_SUBTRACT: sprintf(szBlend, "Subtract",szBlend);
                                  sprintf(msgString, "%sBlendOp: Subtract, ",msgString);
                                  break;

        case D3DBLENDOP_REVSUBTRACT: sprintf(szBlend, "RevSubtract",szBlend);
                                     sprintf(msgString, "%sBlendOp: RevSubtract, ",msgString);
                                     break;

        case D3DBLENDOP_MIN: sprintf(szBlend, "Min",szBlend);
                             sprintf(msgString, "%sBlendOp: Min, ",msgString);
                             break;

        case D3DBLENDOP_MAX: sprintf(szBlend, "Max",szBlend);
                             sprintf(msgString, "%sBlendOp: Max, ",msgString);
                             break;

        default: WriteToLog("Error: Unknown BlendOp: %d.\n",BlendOp[nBlendOp]);
                 SkipTests(1);
                 return false;
    }

	// Let's build some triangles and update status window.
	if (bCircle)
	{
		sprintf(msgString, "%sOverlapping Circles: %d",msgString,nAlphaValue[nTest/6]);
		sprintf(szStatus, "%sOverlapping Circles: %d",szStatus,nAlphaValue[nTest/6]);
	}
	else
	{
		DrawGrid(nTest, (nTest / 6));

		sprintf(msgString, "%sMulticolor Mesh: %d, Triangles: %d",msgString,nAlphaValue[nTest/6],nAlphaValue[(nTest%6)-1]);
		sprintf(szStatus, "%sMulticolor Mesh: %d, Triangles: %d",szStatus,nAlphaValue[nTest/6],nAlphaValue[(nTest%6)-1]);
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CAlphaBldTest::SceneRefresh()
{
	int nTest = (m_uLastTestNum - 1) % MAX_TESTS;

	if (BeginScene())
	{
		// Render the Background image
        if (KeySet("checker"))
        {
		    SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
		    RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,BackgroundList,400,NULL,0,0);
        }

		// Set the appropriate AlphaBlendEnable render state.
		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)nAlphaEnable);

        // If BlendOps supported, set the appropriate BlendOp
        if (bBlendOps)
            SetRenderState(D3DRS_BLENDOP, (DWORD)BlendOp[nBlendOp]);

		// Render the appropriate test case.
		if (bCircle)
		{
			D3DVECTOR Center;
			float     fRadius;
			DWORD	  dwWidth = m_vpTest.Width;//m_pSrcTarget->m_dwWidth;
			DWORD	  dwHeight = m_vpTest.Height;//m_pSrcTarget->m_dwHeight;
			D3DCOLOR  ColorArray[] = {RGBA_MAKE(255,0,0,nAlphaValue[nTest / 6]),
									  RGBA_MAKE(0,255,0,nAlphaValue[nTest / 6]),
									  RGBA_MAKE(0,0,255,nAlphaValue[nTest / 6])};

			fRadius = ((dwWidth < dwHeight) ? dwWidth : dwHeight) * 0.335f;

			for (int i=0; i < 3; i++)
			{
				Center.x = (float)m_vpTest.X + (dwWidth / 2.0f) + ((fRadius / 1.5f) * (float)cos(2.0944f * i));
				Center.y = (float)m_vpTest.Y + (dwHeight / 2.0f) + ((fRadius / 1.5f) * (float)sin(2.0944f * i));
				Center.z = 0.5f;

				Center.x -= 16.0f;

				DrawCircle(&Center, fRadius, 36, ColorArray[i]);
			}
		}
		else
		{
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,VertexList,8,NULL,0,0);
			RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList+8,dwVertexCount-8,NULL,0,0);
		}

		EndScene();
	}	
}

bool CAlphaBldTest::ProcessFrame(void)
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
	WriteStatus("$gBlend Op$y",szBlend);
	WriteStatus("$gOverall Results",szBuffer);

    return bResult;
}

bool CAlphaBldTest::TestTerminate(void)
{
    // If BlendOps supported, advance to the next one 
    if (bBlendOps)
        nBlendOp++;

	return true;
}

//******************************************************************************
BOOL CAlphaBldTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CAlphaBldTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CAlphaBldTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

