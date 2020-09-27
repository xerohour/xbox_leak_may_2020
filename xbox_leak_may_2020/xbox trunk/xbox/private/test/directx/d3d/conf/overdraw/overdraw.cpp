#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Overdraw.h"

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

    COverdrawTest*  pOverdrawTest;
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
                pOverdrawTest = new CAlpha_OverdrawTest();
                break;
            case 1:
                pOverdrawTest = new CBlt_OverdrawTest();
                break;
        }

        if (!pOverdrawTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pOverdrawTest->Create(pDisplay)) {
            pOverdrawTest->Release();
            return FALSE;
        }

        bRet = pOverdrawTest->Exhibit(pnExitCode);

        bQuit = pOverdrawTest->AbortedExit();

        // Clean up the scene
        pOverdrawTest->Release();
    }

    return bRet;
}

COverdrawTest::COverdrawTest()
{
    // Init variables
	bAlpha = false;

    // Only initialize the source
    m_ModeOptions.fReference = false;

    // Disable textures
    m_ModeOptions.fTextures = false;

    // Disable ZBuffers
    m_ModeOptions.fZBuffer = false;

    m_bExit = FALSE;
}

COverdrawTest::~COverdrawTest()
{
    m_pDisplay->SetVerificationThreshold(1.0f);
}

bool COverdrawTest::SetDefaultRenderStates(void)
{
    if (bAlpha)
    {
	    // Set the alpha blending renderstates for alpha method
	    SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
	    SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	    SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    }

	return true;
}

UINT COverdrawTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1, MAX_TESTS);

/*
    if (m_dwVersion <= 0x0700)
    {
	    // Check the caps we are interested in
	    DWORD dwRasterCaps = m_pAdapter->Devices[n].Desc.dpcTriCaps.dwRasterCaps;

	    // Check the SubPixel caps
	    if (!(dwRasterCaps & D3DPRASTERCAPS_SUBPIXEL) &&
		    !(dwRasterCaps & D3DPRASTERCAPS_SUBPIXELX))
	    {
		    WriteToLog("Device capability not supported: Raster Subpixel.\n");
		    return D3DTESTINIT_ABORT;
	    }
    }
*/

    // Force the Blt option to be given on command line
    if (!bAlpha && !KeySet("Blt"))
		return D3DTESTINIT_SKIPALL;

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool COverdrawTest::ClearFrame(void)
{
    return Clear(RGBA_MAKE(0,0,0,255));
}

bool COverdrawTest::ExecuteTest(UINT uTestNum)
{
	// Generate a unique random number for the grid offset
	srand(((MAX_TESTS + 1) - uTestNum) * 9);
	nOffset = rand() % (MAX_TESTS/2);
	float fStart = (float)nOffset;
    int nXDivisions, nYDivisions;

	// Initialize some variables
	bool bResult = true;
	msgString[0] = '\0';

	// Start the test case message
	sprintf(msgString, "%sStarting Pos: ",msgString);

    // Setup the X & Y divisions based on test number
    if (uTestNum <= 10)
	{
        nXDivisions = 1;
        nYDivisions = 1;
	}
	else if (uTestNum <= 15)
	{
        nXDivisions = 5;
        nYDivisions = uTestNum - 10;
	}
	else if (uTestNum <= 20)
	{
        nXDivisions = uTestNum - 15;
        nYDivisions = 5;
	}
	else 
	{
        nXDivisions = uTestNum - 10;
        nYDivisions = uTestNum - 10;
	}

m_pDevice->SetViewport(&m_vpFull);
ClearFrame();

	// Start rendering the primitives.
    if (bAlpha)
    {
		bResult = AlphaGrid(
							uTestNum,		// Test case 
							fStart, fStart,	// Start X, Start Y
							nXDivisions,	// X divisions 
							nYDivisions		// Y divisions
						   );			
    }
    else
    {
		bResult = BltGrid(
						  uTestNum,		// Test case 
						  fStart, fStart,	// Start X, Start Y
						  nXDivisions,	// X divisions 
						  nYDivisions		// Y divisions
						 );			
    }

	sprintf(msgString, "%s(%2d, %2d), ",msgString,nOffset,nOffset);
	sprintf(msgString, "%sWidth Div: %2d, ",msgString,nXDivisions);
	sprintf(msgString, "%sHeight Div: %2d",msgString,nYDivisions);

	// Check the result from OverdrawGrid()
	if (bResult)
	{
		// Tell the log that we are starting
		BeginTestCase(msgString);
	}

    return bResult;
}

bool COverdrawTest::ProcessFrame(void)
{
    CSurface8* pd3ds;
    D3DLOCKED_RECT d3dlr;
    bool	bResult;
	RECT	rSurf;
	DWORD	dwColor;
	void	*pBuf, *pColor;
	char	szBuffer[80];
	static	int nPass = 0;
	static  int nFail = 0;
    HRESULT hr;
    D3DSURFACE_DESC d3dsd;

	// Build the subrect we want
	rSurf.top = nOffset;
	rSurf.left = nOffset;
	rSurf.bottom = rSurf.top + 256;
	rSurf.right = rSurf.left + 256;

    hr = m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return false;
    }
    pd3ds->GetDesc(&d3dsd);

	// Get the Surface data pointer for our subrect
//    pBuf = m_pSrcTarget->Lock(&rSurf);
    hr = pd3ds->LockRect(&d3dlr, &rSurf, D3DLOCK_TILED);
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
        return false;
    }
    pBuf = d3dlr.pBits;

    if (pBuf == NULL)
	{
		WriteToLog("ProcessFrame: Lock failed on back buffer.");
        return false;
	}

	// Initialize variables
	int nRedPixels = 0;
	int nGreenPixels = 0;
	int nBlackPixels = 0;
	int nYellowPixels = 0;

	// Look for any Yellow or Black pixels
	for (int j=0; j < 256; j++)		// Height loop
	{
		pColor = (char*)pBuf + (d3dlr.Pitch * j);

		for (int i=0; i < 256; i++)	// Width loop
		{
			DWORD red, green, blue;

//    		dwColor = m_pSrcTarget->ReadColor(&pColor);
            if (FormatToBitDepth(d3dsd.Format) == 16) {
                ARGBPIXELDESC pixd;
                DWORD i, dwC = 0;
                GetARGBPixelDesc(d3dsd.Format, &pixd);
                for (dwC = (*(LPDWORD)pColor & pixd.dwRMask) >> pixd.dwRShift, i = pixd.dwRMask >> pixd.dwRShift; !(i & 0x80); i <<= 1, dwC <<= 1);
                dwColor = dwC;
                for (dwC = (*(LPDWORD)pColor & pixd.dwGMask) >> pixd.dwGShift, i = pixd.dwGMask >> pixd.dwGShift; !(i & 0x80); i <<= 1, dwC <<= 1);
                dwColor |= dwC;
                for (dwC = (*(LPDWORD)pColor & pixd.dwBMask) >> pixd.dwBShift, i = pixd.dwBMask >> pixd.dwBShift; !(i & 0x80); i <<= 1, dwC <<= 1);
                dwColor |= dwC;
                pColor = ((LPWORD)pColor) + 1;
            }
            else {
                dwColor = *(LPDWORD)pColor;
                pColor = ((LPDWORD)pColor) + 1;
            }

			red   = RGBA_GETRED(dwColor);
			green = RGBA_GETGREEN(dwColor);
			blue  = RGBA_GETBLUE(dwColor);

            if (bAlpha)
            {
                // Count the Underdraw & Overdraw pixels in the surface
                if (!(red || green || blue) || (red && green) || (red > 150) || (green > 150))
                {
    			    if (!(red || green || blue))
                    {
	    			    nBlackPixels++;
//                        dwColor = m_pSrcTarget->ColorConvert(RGBA_MAKE(255,255,255,255));
                        dwColor = ColorToPixel(d3dsd.Format, NULL, RGBA_MAKE(255,255,255,255));
                    }

                    if (red && green)
                    {
				        nYellowPixels++;
//                        dwColor = m_pSrcTarget->ColorConvert(RGBA_MAKE(255,255,0,255));
                        dwColor = ColorToPixel(d3dsd.Format, NULL, RGBA_MAKE(255,255,0,255));
                    }

                    if (red > 150)
                    {
                        nRedPixels++;
//                        dwColor = m_pSrcTarget->ColorConvert(RGBA_MAKE(255,0,0,255));
                        dwColor = ColorToPixel(d3dsd.Format, NULL, RGBA_MAKE(255,0,0,255));
                    }

                    if (green > 150)
                    {
                        nGreenPixels++;
//                        dwColor = m_pSrcTarget->ColorConvert(RGBA_MAKE(0,255,0,255));
                        dwColor = ColorToPixel(d3dsd.Format, NULL, RGBA_MAKE(0,255,0,255));
                    }

                    int nBytes = FormatToBitDepth(d3dsd.Format) / 8;
                    BYTE *pHighlight = (PBYTE)&dwColor;
                    BYTE *pWrite = (PBYTE)pColor;
                    pHighlight += nBytes;
            
                    for (int k=0; k < nBytes; k++)
                    {
                        pWrite--;
                        pHighlight--;
                        *pWrite = *pHighlight;
                    }
                }
            }
            else // Using Blt method
            {
			    // Count the Underdraw pixels in the surface
			    if (!(red || green || blue))
				    nBlackPixels++;

    		    // Count the Overdraw pixels in the surface
                if (red && green) 
				    nYellowPixels++;
            }
		}
	}

//	bResult = m_pSrcTarget->Unlock();
    bResult = SUCCEEDED(pd3ds->UnlockRect());  
    pd3ds->Release();
    
    if (!bResult)
 	{
		WriteToLog("ProcessFrame: Unlock failed on back buffer.");
        return false;
	}

	// Tell the logfile how it went
	if ((nBlackPixels == 0) && (nYellowPixels == 0) && (nRedPixels == 0) && (nGreenPixels == 0))
	{
		(m_fIncrementForward) ? nPass++ : nPass--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
//		m_fPassPercentage = 1.0;
        m_pDisplay->SetVerificationThreshold(1.0f);
		bResult = true;
		Pass();
	}
	else 
	{
        // If alpha method, Blt the highlighted failures 
        if (bAlpha)
        {
//            if (m_dwVersion <= 0x0700)
//            {
//                POINT Pt = {0,0};
//				Blt(m_pSrcTarget,Pt);
//            }
//            else // m_dwVersion >= 0x0800
//            {
                POINT Pt = {0,20};
                Present8(Pt);
//            }
        }

        if (nBlackPixels)
		{
			WriteToLog("UnderDraw Pixels detected!\n");
			WriteToLog("Found %d Black pixels (highlighted White).\n", nBlackPixels);
		}

		if (nYellowPixels)
		{
			WriteToLog("OverDraw Pixels detected!\n");
			WriteToLog("Found %d Yellow pixels.\n", nYellowPixels);
		}

		if (nRedPixels)
		{
			WriteToLog("OverDraw Pixels detected!\n");
			WriteToLog("Found %d overbright Red pixels.\n", nRedPixels);
		}

		if (nGreenPixels)
		{
			WriteToLog("OverDraw Pixels detected!\n");
			WriteToLog("Found %d overbright Green pixels.\n", nGreenPixels);
		}

		(m_fIncrementForward) ? nFail++ : nFail--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
//		m_fPassPercentage = 1.0f - ((nBlackPixels + nYellowPixels + nGreenPixels + nRedPixels) / 65536.0f);
        m_pDisplay->SetVerificationThreshold(1.0f - ((nBlackPixels + nYellowPixels + nGreenPixels + nRedPixels) / 65536.0f));
		bResult = false;
		Fail();
	}

	WriteStatus("$gOverall Results",szBuffer);
	
    return bResult;
}

BOOL COverdrawTest::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

    if (!m_bCreated) {
        return FALSE;
    }

    hr = m_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) {
            return TRUE;
        }
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!Reset()) {
                return FALSE;
            }
        }
    }

    // Clear the rendering target
    m_pDevice->SetViewport(&m_vpFull);

//    ClearFrame();

    SwitchAPI((UINT)m_fFrame);

//    SceneRefresh();

    // Begin the scene
    m_pDevice->BeginScene();

    // Display the adapter mode
//    ShowDisplayMode();

    // Display the frame rate
//    ShowFrameRate();

    // Display the console
//    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    CompareImages();

    ProcessFrame();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    return TRUE;
}

//******************************************************************************
bool COverdrawTest::Present8(POINT & Pt)
{
    CSurface8* pd3ds;
    D3DSURFACE_DESC d3dsd;
    RECT rSrcRect, rDstRect;

    if (FAILED(m_pDevice->GetRenderTarget(&pd3ds))) {
        return false;
    }

    if (FAILED(pd3ds->GetDesc(&d3dsd))) {
        pd3ds->Release();
        return FALSE;
    }

    pd3ds->Release();

    rSrcRect.left = 0;
    rSrcRect.top = 0;
    rSrcRect.right = d3dsd.Width;
    rSrcRect.bottom = d3dsd.Height;

    rSrcRect.left = Pt.x;
    rSrcRect.top = Pt.y;
    rSrcRect.right = d3dsd.Width + Pt.x;
    rSrcRect.bottom = d3dsd.Height + Pt.y;

//    if (!SetLastError(m_pDevice->Present(&rSrcRect, &rDstRect, NULL, NULL)))
    if (!SetLastError(m_pDevice->Present(NULL, NULL, NULL, NULL)))
        return false;
    return true;
}

//******************************************************************************
BOOL COverdrawTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void COverdrawTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL COverdrawTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
