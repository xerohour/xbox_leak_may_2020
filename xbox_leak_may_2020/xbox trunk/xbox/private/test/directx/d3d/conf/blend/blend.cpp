#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Blend.h"

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

    CBlendTest*     pBlendTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 7 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pBlendTest = new CDefaultTest();
                break;
            case 1:
                pBlendTest = new CDecalTest();
                break;
            case 2:
                pBlendTest = new CModulateTest();
                break;
            case 3:
                pBlendTest = new CDecalAlphaTest();
                break;
            case 4:
                pBlendTest = new CModulateAlphaTest();
                break;
            case 5:
                pBlendTest = new CCopyTest();
                break;
            case 6:
                pBlendTest = new CAddTest();
                break;
        }

        if (!pBlendTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pBlendTest->Create(pDisplay)) {
            pBlendTest->Release();
            return FALSE;
        }

        bRet = pBlendTest->Exhibit(pnExitCode);

        bQuit = pBlendTest->AbortedExit();

        // Clean up the scene
        pBlendTest->Release();
    }

    return bRet;
}

CBlendTest::CBlendTest()
{
	szTextureOp = "";
	szTextureOp2 = "";

    pTexture = NULL;
    m_bExit = FALSE;
}

CBlendTest::~CBlendTest()
{
    ReleaseTexture(pTexture);
    pTexture = NULL;
}

UINT CBlendTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Request all texture formats except bumpmap
//	if (!RequestTextureFormats(PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA))
//	{
//		WriteToLog("No texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

	// Setup the Test range
	SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

    // Check the caps we are interested in
	DWORD dwSrcBlendCaps = m_d3dcaps.SrcBlendCaps;
	DWORD dwDestBlendCaps = m_d3dcaps.DestBlendCaps;
	DWORD dwShadeCaps = m_d3dcaps.ShadeCaps;

    if (m_dwVersion >= 0x0800)
    {
    	DWORD dwTextureOpCaps = m_d3dcaps.TextureOpCaps;

        // Copy == Decal for Dx8, no need to run
        if (dwBlendCap == D3DPTBLENDCAPS_COPY)
        {
		    WriteToLog("This test is not valid on Dx8.\n");
		    return D3DTESTINIT_SKIPALL;
        }

        // Check the appropriate Texture Op caps
	    if (!(dwTextureOpCaps & dwTextureOpCap))
	    {
		    WriteToLog("Device capability not supported: TextureOp %s.\n",szTextureOp);
		    return D3DTESTINIT_SKIPALL;
	    }

        // If needed, check other Texture Op caps
        if (dwTextureOpCap2 && !(dwTextureOpCaps & dwTextureOpCap2))
	    {
		    WriteToLog("Device capability not supported: TextureOp %s.\n",szTextureOp2);
		    return D3DTESTINIT_SKIPALL;
	    }
    }
/*
    else // dwVersion <= 0x0700
    {
	    DWORD dwTextureBlendCaps = m_pAdapter->Devices[n].Desc.dpcTriCaps.dwTextureBlendCaps;

        // Check the appropriate Blend caps
	    if (!(dwTextureBlendCaps & dwBlendCap))
	    {
		    WriteToLog("Device capability not supported: %s.\n",m_szTestName);
		    return D3DTESTINIT_SKIPALL;
	    }
    }
*/
	// Also check the Alpha Blending mode being used
	dwSrcAlphaCap		= (dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA);
	dwInvSrcAlphaCap	= (dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA);
	dwAlphaGouraudCap	= (dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND);

	// Setup the colors for the Stripes array

	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	// Create the stripe image data
//    CImageLoader    Image;

//    pImage = Image.LoadStripes(64,64,8,Stripes,false); 

	// Setup the colors for the AlphaStripes array

	AlphaStripes[0] = RGBA_MAKE(255,255,255,0);		// White
	AlphaStripes[1] = RGBA_MAKE(255,0,0,32);		// Red
	AlphaStripes[2] = RGBA_MAKE(0,255,0,64);		// Green
	AlphaStripes[3] = RGBA_MAKE(0,0,255,127);		// Blue
	AlphaStripes[4] = RGBA_MAKE(255,255,0,128);		// Yellow
	AlphaStripes[5] = RGBA_MAKE(255,0,255,192);		// Magenta
	AlphaStripes[6] = RGBA_MAKE(0,255,255,224);		// Cyan
	AlphaStripes[7] = RGBA_MAKE(128,128,128,255);	// Grey

	// Create the AlphaStripe image data
//    CImageLoader    AlphaImage;

//    pAlphaImage = AlphaImage.LoadStripes(64,64,8,AlphaStripes,false); 

	return D3DTESTINIT_RUN;
}

bool CBlendTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,100,255));
}

bool CBlendTest::ExecuteTest(UINT uTestNum)
{
	// Setup the tests to execute.
	bool bAlphaTexture = false;
	int nTest;
    UINT uTextureFormat;
    ARGBPIXELDESC pixd;

	// Initialize some variables
	nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;

	// Cycle the texture formats
	uTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

	// Check the texture for an alpha channel
    // TODO: need to handle DX8 formats (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat)
//	if ((m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_ALPHAPIXELS) ||
//        (m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_ALPHA))
    GetARGBPixelDesc(m_fmtCommon[uTextureFormat], &pixd);
    if (pixd.dwAMask)
		bAlphaTexture = true;

	// Set the default alpha blending renderstates
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	// Let's build some triangles.
	switch (nTest)
	{
		case (1): // Use no Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): Default, ",msgString);
			sprintf(szStatus, "%sAlpha (Tex): Default",szStatus);
			DrawBlendingGrid(255);

			// Create the texture
//		    pTexture = CreateTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),CDDS_TEXTURE_VIDEO,pAlphaImage);
            pTexture = CreateStripedTexture(m_pDevice, 64, 64, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
			break;
		case (2): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): SrcAlpha/InvSrcAlpha, ",msgString);
			sprintf(szStatus, "%sAlpha (Tex): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),CDDS_TEXTURE_VIDEO,pAlphaImage);
            pTexture = CreateStripedTexture(m_pDevice, 64, 64, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
			break;
		case (3): // Use mesh Alpha Blending
			sprintf(msgString, "%sAlpha (Mesh): SrcAlpha/InvSrcAlpha, ",msgString);
			sprintf(szStatus, "%sAlpha (Mesh): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Make sure device can do interpolated mesh alpha
			if (!dwAlphaGouraudCap)
			{
				bool bAlphaSkip = false;

				if (m_dwVersion >= 0x0700)
				{
					if ((dwBlendCap == D3DPTBLENDCAPS_DECALALPHA)   || 
						(dwBlendCap == D3DPTBLENDCAPS_MODULATEALPHA)||
						(dwBlendCap == D3DPTBLENDCAPS_ADD))
					{
						bAlphaSkip = true;
					}
				}
				else
				{
					if (!((dwBlendCap == D3DPTBLENDCAPS_DECAL) ||
						  (dwBlendCap == D3DPTBLENDCAPS_COPY)  ||
						  ((dwBlendCap == D3DPTBLENDCAPS_MODULATE) && bAlphaTexture) ||
						  ((dwBlendCap == 0xffffffff) && bAlphaTexture)))
					{
						bAlphaSkip = true;
					}
				}

				if (bAlphaSkip)
				{
					WriteToLog("Device capability not supported: Shade - AlphaGouraudBlend.\n");

					if ((dwBlendCap == D3DPTBLENDCAPS_MODULATE) || (dwBlendCap == 0xffffffff))
						SkipTests(1);
					else
						SkipTests(m_uCommonTextureFormats);

					return false;
				}
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pImage->GetWidth(),pImage->GetHeight(),CDDS_TEXTURE_VIDEO,pImage);
            pTexture = CreateStripedTexture(m_pDevice, 64, 64, 8, Stripes, false, false, m_fmtCommon[uTextureFormat]);
			break;
		case (4): // Use mesh and texture Alpha Blending
			sprintf(msgString, "%sAlpha (Both): SrcAlpha/InvSrcAlpha, ",msgString);
			sprintf(szStatus, "%sAlpha (Both): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Make sure device can do interpolated mesh alpha
			if (!dwAlphaGouraudCap)
			{
				bool bAlphaSkip = false;

				if (m_dwVersion >= 0x0700)
				{
					if ((dwBlendCap == D3DPTBLENDCAPS_DECALALPHA)   || 
						(dwBlendCap == D3DPTBLENDCAPS_MODULATEALPHA)||
						(dwBlendCap == D3DPTBLENDCAPS_ADD))
					{
						bAlphaSkip = true;
					}
				}
				else
				{
					if (!((dwBlendCap == D3DPTBLENDCAPS_DECAL) ||
						  (dwBlendCap == D3DPTBLENDCAPS_COPY)  ||
						  ((dwBlendCap == D3DPTBLENDCAPS_MODULATE) && bAlphaTexture) ||
						  ((dwBlendCap == 0xffffffff) && bAlphaTexture)))
					{
						bAlphaSkip = true;
					}
				}

				if (bAlphaSkip)
				{
					WriteToLog("Device capability not supported: Shade - AlphaGouraudBlend.\n");

					if ((dwBlendCap == D3DPTBLENDCAPS_MODULATE) || (dwBlendCap == 0xffffffff))
						SkipTests(1);
					else
						SkipTests(m_uCommonTextureFormats);

					return false;
				}
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),CDDS_TEXTURE_VIDEO,pAlphaImage);
            pTexture = CreateStripedTexture(m_pDevice, 64, 64, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
			break;
		default:
			DrawBasicGrid(uTestNum,m_uCommonTextureFormats);

			// Create the texture
//		    pTexture = CreateTexture(pImage->GetWidth(),pImage->GetHeight(),CDDS_TEXTURE_VIDEO,pImage);
            pTexture = CreateStripedTexture(m_pDevice, 64, 64, 8, Stripes, false, false, m_fmtCommon[uTextureFormat]);
			break;
	}

	// Tell the framework what texture to use
    if (NULL != pTexture)
	{
        SetTexture(0,pTexture);

//        if (m_dwVersion <= 0x0700)
//        {
//    		sprintf(msgString, "%sTex:",msgString);
//        	GetTexFmtDesc(&m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat,msgString+strlen(msgString),80-strlen(msgString));
//        }
//        else
    		sprintf(msgString, "%sTex:%d",msgString,m_fmtCommon[uTextureFormat]);//D3DFmtToString(m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat));
	}
	else
	{
/*
		if (m_dwVersion == 0x0600)
		{
            // TODO: need to handle DX8 formats (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat)
			DWORD dwFourCC = m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwFourCC;

			if ((dwFourCC == MAKEFOURCC('U','Y','V','Y')) ||
				(dwFourCC == MAKEFOURCC('Y','U','Y','2')))
			{
				char	szFourCC[5];

				szFourCC[0] = (char)(dwFourCC & 0xff);
				szFourCC[1] = (char)((dwFourCC >> 8) & 0xff);
				szFourCC[2] = (char)((dwFourCC >> 16) & 0xff);
				szFourCC[3] = (char)((dwFourCC >> 24) & 0xff);
				szFourCC[4] = '\0';

				WriteToLog("Can not create a FourCC (%s) texture on Dx6.\n",szFourCC);
				SkipTests(1);
				return false;
			}
		}
*/
		WriteToLog("CreateTexture() failed with HResult = %X.\n",GetLastError());
//		TestFinish(D3DTESTFINISH_ABORT);
		return false;
	}

	if ((m_dwVersion >= 0x0700) && (dwBlendCap != 0xffffffff))
	{
        DWORD dwBState = dwBlendState;
        if (dwBState == D3DTBLEND_MODULATE && bAlphaTexture) {
            dwBState = D3DTBLEND_MODULATE_ALPHATEXTURE;
        }
		if (!SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)dwBState))
		{
			WriteToLog("ValidateDevice Failed: %s\n",msgString);
			SkipTests(1);
			return false;
		}
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CBlendTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
	    RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CBlendTest::ProcessFrame(void)
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

	WriteStatus("$gTest Options$y",szStatus);
	WriteStatus("$gOverall Results",szBuffer);

    return bResult;
}

bool CBlendTest::TestTerminate()
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;
//    RELEASE(pImage);
//	RELEASE(pAlphaImage);

	return true;
}

//******************************************************************************
BOOL CBlendTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CBlendTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CBlendTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

