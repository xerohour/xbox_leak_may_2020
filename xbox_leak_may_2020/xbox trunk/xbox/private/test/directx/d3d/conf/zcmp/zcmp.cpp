#define STRICT
#define BURN_IN_HELL
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "ZCmp.h"

//extern bool Unique(DWORD dwVersion, PBUFFER *pBuffers, LPPIXELFORMAT pFormat);

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

    CZCmpTest*      pZCmpTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 8 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pZCmpTest = new CZNeverTest();
                break;
            case 1:
                pZCmpTest = new CZLessTest();
                break;
            case 2:
                pZCmpTest = new CZEqualTest();
                break;
            case 3:
                pZCmpTest = new CZLessEqualTest();
                break;
            case 4:
                pZCmpTest = new CZGreaterTest();
                break;
            case 5:
                pZCmpTest = new CZNotEqualTest();
                break;
            case 6:
                pZCmpTest = new CZGreaterEqualTest();
                break;
            case 7:
                pZCmpTest = new CZAlwaysTest();
                break;
        }

        if (!pZCmpTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pZCmpTest->Create(pDisplay)) {
            pZCmpTest->Release();
            return FALSE;
        }

        bRet = pZCmpTest->Exhibit(pnExitCode);

        bQuit = pZCmpTest->AbortedExit();

        // Clean up the scene
        pZCmpTest->Release();
    }

    return bRet;
}

CZCmpTest::CZCmpTest()
{
    // Disable textures
    m_ModeOptions.fTextures = false;

    m_pd3dsOriginalZ = NULL;
    m_bExit = FALSE;
}

CZCmpTest::~CZCmpTest()
{
    // Restore the original depth buffer
    if (m_pd3dsOriginalZ) {
        m_pDevice->SetRenderTarget(NULL, m_pd3dsOriginalZ);
    }
}
/*
bool CZCmpTest::AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize)
{
    PBUFFER pBuffers = NULL;

	// Test all of the common ZBuffer Formats
    if (IsConformance())
    {
        for (int i=0; i < m_uCommonZBufferFormats; i++)
		{
            if (Unique(m_dwVersion, &pBuffers, &m_pCommonZBufferFormats[i].ddpfPixelFormat))
            {
				pMode->D3DMode.nZBufferFormat = i;

				CD3DTest::AddModeList(pMode,pData,dwDataSize);
			}
        }
    }
    else
    {
		// Let the framework handle stress mode
		CD3DTest::AddModeList(pMode,pData,dwDataSize);
	}

    // Cleanup any memory that we alloc'd
    if (pBuffers)
    {
        PBUFFER pBuffer, pTemp;

        for (pBuffer = pBuffers; pBuffer; pBuffer = pTemp)
        {
            pTemp = pBuffer->pNext;
            free(pBuffer);
        }
    }

    return true;
}
*/
UINT CZCmpTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

    // Save the original depth buffer
    m_pDevice->GetDepthStencilSurface(&m_pd3dsOriginalZ);

	// Setup the Test range
	if ((dwZCmpCap == D3DPCMPCAPS_EQUAL) || (dwZCmpCap == D3DPCMPCAPS_NOTEQUAL))
        m_uMaxTests = MAX_TESTS-2;
	else
        m_uMaxTests = MAX_TESTS;

	SetTestRange(1,m_uMaxTests * m_uCommonZBufferFormats);

	// Check the caps we are interested in
	DWORD dwZCmpCaps = m_d3dcaps.ZCmpCaps;

	// Check the appropriate Z Compare caps
	if (!(dwZCmpCaps & dwZCmpCap))
	{
		WriteToLog("Device capability not supported: %s.\n",m_szTestName);
		return D3DTESTINIT_ABORT;
	}
  
    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	// Init the msgString to empty
	msgString[0] = '\0';

	// Log the current Z Buffer format
//    if (m_dwVersion <= 0x0700)
//    {
//        PIXELFORMAT Format;
//        memcpy(&Format, &m_pCommonZBufferFormats[m_pMode->nZBufferFormat].ddpfPixelFormat, sizeof(Format));

//	    sprintf(msgString, "%sZ Buffer Format, Z: %d-bit (0x%x), Stencil: %d-bit (0x%x).\n",msgString,Format.dwZBufferBitDepth - Format.dwStencilBitDepth,Format.dwZBitMask,Format.dwStencilBitDepth,Format.dwStencilBitMask);
//    }
//    else
//	    sprintf(msgString, "%sZ Buffer Format: %s.\n",msgString,D3DFmtToString(m_pCommonZBufferFormats[m_pMode->nZBufferFormat].d3dfFormat));

//    WriteToLog(msgString);

	return D3DTESTINIT_RUN;
}

bool CZCmpTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

bool CZCmpTest::ExecuteTest(UINT uTestNum)
{
    UINT uTest = (uTestNum - 1) % m_uMaxTests + 1;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

    if (uTest == 1) {
        CSurface8* pd3ds;
        CSurface8* pd3dsRT;
        D3DSURFACE_DESC d3dsd;
        int nZBufferFormat = (uTestNum - 1) / m_uMaxTests;
        if (FAILED(m_pDevice->GetRenderTarget(&pd3dsRT))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetRenderTarget failed"));
            SkipTests(m_uMaxTests);
            return false;
        }
        pd3dsRT->GetDesc(&d3dsd);
        pd3dsRT->Release();
        if (FAILED(m_pDevice->CreateDepthStencilSurface(d3dsd.Width, d3dsd.Height, m_fmtdCommon[nZBufferFormat], D3DMULTISAMPLE_NONE, &pd3ds))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateDepthStencilSurface failed on format %s"), D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
            SkipTests(m_uMaxTests);
            return false;
        }
    	msgString[0] = '\0';
	    sprintf(msgString, "%sZ Buffer Format: %s.\n",msgString,D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
        WriteToLog(msgString);
        if (FAILED(m_pDevice->SetRenderTarget(NULL, pd3ds))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed on depth format %s"), D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
            SkipTests(m_uMaxTests);
            pd3ds->Release();
            return false;
        }
        pd3ds->Release();

    	msgString[0] = '\0';
    }

	// Let's build some triangles.
	switch (dwZCmpCap)
	{
		case D3DPCMPCAPS_NEVER:
			AlwaysTests((int)uTest);
			break;
		case D3DPCMPCAPS_LESS:
			LessTests((int)uTest);
			break;
		case D3DPCMPCAPS_EQUAL:
			EqualTests((int)uTest);
			break;
		case D3DPCMPCAPS_LESSEQUAL:
			LessTests((int)uTest);
			break;
		case D3DPCMPCAPS_GREATER:
			GreaterTests((int)uTest);
			break;
		case D3DPCMPCAPS_NOTEQUAL:
			EqualTests((int)uTest);
			break;
		case D3DPCMPCAPS_GREATEREQUAL:
			GreaterTests((int)uTest);
			break;
		case D3DPCMPCAPS_ALWAYS:
			AlwaysTests((int)uTest);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CZCmpTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CZCmpTest::ProcessFrame(void)
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
BOOL CZCmpTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CZCmpTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CZCmpTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
