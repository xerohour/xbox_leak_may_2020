#define STRICT
#define BURN_IN_HELL
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Gradient.h"

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

    CGradientTest*  pGradientTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 9 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pGradientTest = new CTexture8x8Test();
                break;
            case 1:
                pGradientTest = new CTexture16x16Test();
                break;
            case 2:
                pGradientTest = new CTexture32x32Test();
                break;
            case 3:
                pGradientTest = new CTexture64x64Test();
                break;
            case 4:
                pGradientTest = new CTexture128x128Test();
                break;
            case 5:
                pGradientTest = new CTexture256x256Test();
                break;
            case 6:
                pGradientTest = new CTexture512x512Test();
                break;
            case 7:
                pGradientTest = new CTexture1024x1024Test();
                break;
            case 8:
                pGradientTest = new CTexture2048x2048Test();
                break;
        }

        if (!pGradientTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pGradientTest->Create(pDisplay)) {
            pGradientTest->Release();
            return FALSE;
        }

        bRet = pGradientTest->Exhibit(pnExitCode);

        bQuit = pGradientTest->AbortedExit();

        // Clean up the scene
        pGradientTest->Release();
    }

    return bRet;
}

CGradientTest::CGradientTest()
{
    pTexture = NULL;
    m_bExit = FALSE;
}

CGradientTest::~CGradientTest()
{
}

UINT CGradientTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice; 
	UINT Result = D3DTESTINIT_RUN;

	// Setup the Test range
	SetTestRange(1, MAX_TESTS);
//	SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);
 
	// Check the caps we are interested in
	DWORD dwTextureCaps = m_d3dcaps.TextureCaps;
	DWORD dwMinTextureWidth  = 1;
	DWORD dwMinTextureHeight = 1;
	DWORD dwMaxTextureWidth  = m_d3dcaps.MaxTextureWidth;
	DWORD dwMaxTextureHeight = m_d3dcaps.MaxTextureHeight;
	DWORD dwMaxTextureRepeat = m_d3dcaps.MaxTextureRepeat;

	// Special check to make sure the texture size caps are filled out
	if ((dwMinTextureWidth == 0) && (dwMinTextureHeight == 0) &&
		(dwMaxTextureWidth == 0) && (dwMaxTextureHeight == 0))
	{
		// Log an error that we MUST set the texture size caps
		WriteToLog("Must set Min/Max Texture Width/Height caps.\n");
		return D3DTESTINIT_ABORT;
	}

	// See if the MinTextureWidth is supported
	if (nTextureSize < (int)dwMinTextureWidth)
	{
		WriteToLog("Texture width (%d) < MinTextureWidth (%d)\n",nTextureSize,dwMinTextureWidth);
		Result = D3DTESTINIT_SKIPALL;
	}

	// See if the MinTextureHeight is supported
	if (nTextureSize < (int)dwMinTextureHeight)
	{
		WriteToLog("Texture height (%d) < MinTextureHeight (%d)\n",nTextureSize,dwMinTextureHeight);
		Result = D3DTESTINIT_SKIPALL;
	}

	// See if the MaxTextureWidth is supported
	if (nTextureSize > (int)dwMaxTextureWidth)
	{
		WriteToLog("Texture width (%d) > MaxTextureWidth (%d)\n",nTextureSize,dwMaxTextureWidth);
		Result = D3DTESTINIT_SKIPALL;
	}

	// See if the MaxTextureHeight is supported
	if (nTextureSize > (int)dwMaxTextureHeight)
	{
		WriteToLog("Texture height (%d) > MaxTextureHeight (%d)\n",nTextureSize,dwMaxTextureHeight);
		Result = D3DTESTINIT_SKIPALL;
	}

	// Init the Maximum Scale variable
	nMaxScale = 4;

	// See if we need to adjust the maximum scale value
	if (!(dwTextureCaps & D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE))
	{
		if (!dwMaxTextureRepeat)
		{
			// Must set the MaxTextureRepeat cap if TexRepeatNotScaledBySize is not set
			WriteToLog("Must set MaxTextureRepeat cap if TexRepeatNotScaledBySize not set.\n");
//MPL-Hack			return false;
		}

		else  //MPL-Hack
		if ((nTextureSize * 4) > (int)dwMaxTextureRepeat)
		{
			nMaxScale = nTextureSize / (dwMaxTextureRepeat / 4);
		}
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

	// Create the gradient image data
//    CImageLoader    Image;

//    pImage = Image.LoadGradient(nTextureSize,nTextureSize,Gradient); 

	return Result;
}

bool CGradientTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

bool CGradientTest::ExecuteTest(UINT uTestNum)
{
   // Setup the tests to execute.
	int nTest;
    int nFormat;
 
	// Initialize some variables
	nTest = ((uTestNum-1) / 1) + 1;
//	nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
 	dwVertexCount = 0;
	msgString[0] = 0;
	sprintf(msgString, "");

	// Cleanup previous texture
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;

	// Let's build some triangles.
	DrawGrid(nTest);
 
	// Cycle the texture formats
//	if (CycleFormats)
//		m_pD3DMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;
//	else

	// Pick the fist 16-bit texture enumerated
	for (int i=0; i < (int)m_uCommonTextureFormats; i++)
	{
        // TODO: need to handle DX8 formats (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat)
//		if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwRGBBitCount == 16)
        if (FormatToBitDepth(m_fmtCommon[i]) == 16)
		{
//			m_pMode->nTextureFormat = i;
            nFormat = i;
			break;
		}
	}

	// Create the texture
	pTexture = (CTexture8*)CreateGradientTexture(m_pDevice, nTextureSize,nTextureSize,Gradient);

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
#ifndef UNICODE
    		sprintf(msgString, "%sTex:%s",msgString,D3DFmtToString(m_fmtCommon[nFormat]));
#else
    		sprintf(msgString, "%sTex:%S",msgString,D3DFmtToString(m_fmtCommon[nFormat]));
#endif
	}
	else
	{
		WriteToLog("CreateTexture() failed with HResult = %X.\n",GetLastError());
 
        if (GetLastError() == E_OUTOFMEMORY)
			TestFinish(D3DTESTFINISH_SKIPREMAINDER);
		else
			TestFinish(D3DTESTFINISH_ABORT);

		return false;
	}
 
	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CGradientTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CGradientTest::ProcessFrame(void)
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

bool CGradientTest::TestTerminate()
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;
//    RELEASE(pImage);

	return true;
}

//******************************************************************************
BOOL CGradientTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CGradientTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CGradientTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

 
