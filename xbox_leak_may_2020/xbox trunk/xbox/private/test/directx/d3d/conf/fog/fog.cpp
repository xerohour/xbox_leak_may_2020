#define STRICT
#define BURN_IN_HELL
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Fog.h"

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

    CFogTest*       pFogTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 14 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pFogTest = new CVertexTest();
                break;
            case 1:
                pFogTest = new CTableLinearTest();
                break;
            case 2:
                pFogTest = new CTableExpTest();
                break;
            case 3:
                pFogTest = new CTableExp2Test();
                break;
            case 4:
                pFogTest = new CWLinearTest();
                break;
            case 5:
                pFogTest = new CWExpTest();
                break;
            case 6:
                pFogTest = new CWExp2Test();
                break;
            case 7:
                pFogTest = new CTextureVertexTest();
                break;
            case 8:
                pFogTest = new CTextureTableLinearTest();
                break;
            case 9:
                pFogTest = new CTextureTableExpTest();
                break;
            case 10:
                pFogTest = new CTextureTableExp2Test();
                break;
            case 11:
                pFogTest = new CTextureWLinearTest();
                break;
            case 12:
                pFogTest = new CTextureWExpTest();
                break;
            case 13:
                pFogTest = new CTextureWExp2Test();
                break;
        }

        if (!pFogTest) {
            return FALSE;
        }

        // Initialize the scene
        if (pFogTest->Create(pDisplay)) {

            bRet = pFogTest->Exhibit(pnExitCode);

            bQuit = pFogTest->AbortedExit();
        }

        // Clean up the scene
        pFogTest->Release();
    }

    return bRet;
}

CFogTest::CFogTest()
{
    pTexture = NULL;
//    pImage = NULL;

    m_bExit = FALSE;
}
 
CFogTest::~CFogTest()
{
}

bool CFogTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

bool CFogTest::ExecuteTest(UINT uTestNum)
{
    int nTextureFormat;
    int nTest = uTestNum;
	char	szBuffer[80];

	// Initialize some variables
	dwVertexCount = 0;
	msgString[0] = '\0';
	szBuffer[0] = '\0';

    if (m_bTextures)
    {
        // Reset test number based on texture formats
 	    nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;

        // Cleanup previous texture
	    SetTexture(0, NULL);
	    ReleaseTexture(pTexture);
        pTexture = NULL;
    }

    // Let's build some triangles.
	switch (dwFog)
	{
		case VERTEX:
            if (m_bTextures)
			    TextureVertexTests(nTest);
            else
                VertexTests(nTest);
			break;
		case TABLE_LINEAR:
			{
   				float	fStart, fEnd;

                // Build the Table Fog triangles
				TableTests();

                if (m_bTextures)
                {
                    fStart = (float) (1 - (nTest / (TEXTURE_TABLE_TESTS - 1)));
                    fEnd = (float) (1 - (nTest / TEXTURE_TABLE_TESTS));
                }
                else
                {
       				int nDepth = (TABLE_LINEAR_TESTS / 2) - nTest;

				    if (nDepth >= 0)
				    {
					    fStart = 0.0f;
					    fEnd = fStart + (nDepth / 10.0f);
				    }
				    else
				    {
					    fEnd = 1.0f;
					    fStart = fEnd - ((nDepth + 11) / 10.0f);
				    }
                }

				// Update the color and value status buffer
				sprintf(szBuffer,"Fog: Blue, Triangles: White, Strips: Red");
				WriteStatus("$gColor Values$y",szBuffer);

				sprintf(szBuffer,"Start: %.2f, End: %.2f", fStart, fEnd);
				WriteStatus("$gFog Values$y",szBuffer);

				// Update the logfile
				sprintf(msgString, "%sFog: Blue, Start: %.2f, End: %.2f",msgString, fStart, fEnd);

				SetRenderState(D3DRS_FOGSTART, F2DW(fStart));
				SetRenderState(D3DRS_FOGEND, F2DW(fEnd));
//				SetRenderState(D3DRENDERSTATE_FOGTABLESTART, fStart);
//				SetRenderState(D3DRENDERSTATE_FOGTABLEEND, fEnd);
			}
			break;
		case TABLE_EXP:
		case TABLE_EXP2:
			{
                float fDensity;

				if (m_bTextures)
                    fDensity = 1 - (TEXTURE_TABLE_TESTS - nTest) / 2.0f;
                else
                    fDensity = (TABLE_EXP_TESTS - nTest) / 10.0f;

				// Build the Table Fog triangles
				TableTests();

				// Update the color and value status buffer
				sprintf(szBuffer,"Fog: Blue, Triangles: White, Strips: Red");
				WriteStatus("$gColor Values$y",szBuffer);

				sprintf(szBuffer,"Density: %.2f", fDensity);
				WriteStatus("$gFog Values$y",szBuffer);

				// Update the logfile
				sprintf(msgString, "%sFog: Blue, Density: %.2f",msgString, fDensity);

//				SetRenderState(D3DRENDERSTATE_FOGTABLEDENSITY, fDensity);
				SetRenderState(D3DRENDERSTATE_FOGDENSITY, F2DW(fDensity));
			}
			break;
		case W_LINEAR:
			{
				float	fStart, fEnd;

				// Build the W Fog triangles
				WTests();

                if (m_bTextures)
                {
                    if (nTest == 1)
                        fStart = fEnd = 10.0f;
                    else if (nTest == 3)
                        fStart = fEnd = 1.0f;
                    else
                    {
                        fStart = 1.0f;
                        fEnd = 5.0f;
                    }
                }
                else
                {
   				    int	nDepth = (W_LINEAR_TESTS / 2) - nTest;

				    if ((nTest / 20) == 0)
				    {
					    fStart = 1.0f;
					    fEnd = fStart + ((nTest-1) * 0.5f);
				    }
				    else
				    {
					    fEnd = 10.0f;
					    fStart = fEnd - ((nTest-20) * 0.5f);
				    }
                }

				// Update the color and value status buffer
				sprintf(szBuffer,"Fog: Red, Triangles: White, Strips: Red");
				WriteStatus("$gColor Values$y",szBuffer);

				sprintf(szBuffer,"Start: %.2f, End: %.2f", fStart, fEnd);
				WriteStatus("$gFog Values$y",szBuffer);

				// Update the logfile
				sprintf(msgString, "%sFog: Red, Start: %.2f, End: %.2f",msgString, fStart, fEnd);

//				SetRenderState(D3DRENDERSTATE_FOGTABLESTART, fStart);
//				SetRenderState(D3DRENDERSTATE_FOGTABLEEND, fEnd);
				SetRenderState(D3DRENDERSTATE_FOGSTART, F2DW(fStart));
				SetRenderState(D3DRENDERSTATE_FOGEND, F2DW(fEnd));
			}
			break;
		case W_EXP:
		case W_EXP2:
			{
                float fDensity;

				if (m_bTextures)
                    fDensity = 1 - (TEXTURE_W_TESTS - nTest) / 2.0f;
                else
                    fDensity = (W_EXP_TESTS - nTest) / 10.0f;

				// Build the W Fog triangles
				WTests();

				// Update the color and value status buffer
				sprintf(szBuffer,"Fog: Red, Triangles: White, Strips: Red");
				WriteStatus("$gColor Values$y",szBuffer);

				sprintf(szBuffer,"Density: %.2f", fDensity);
				WriteStatus("$gFog Values$y",szBuffer);

				// Update the logfile
				sprintf(msgString, "%sFog: Red, Density: %.2f",msgString, fDensity);

//				SetRenderState(D3DRENDERSTATE_FOGTABLEDENSITY, fDensity);
				SetRenderState(D3DRENDERSTATE_FOGDENSITY, F2DW(fDensity));
			}
			break;
	}

	// Set the fog color
	SetRenderState(D3DRENDERSTATE_FOGCOLOR, FogColor);

    if (m_bTextures)
    {
	    // Cycle the common texture formats
	    nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

	    // Create and set the texture
	    pTexture = (CTexture8*)CreateStripedTexture(m_pDevice, 64, 64, 8, Stripes, false);

	    // Tell the framework what texture to use
	    if (NULL != pTexture)
	    {
		    SetTexture(0,pTexture);

            if (m_dwVersion <= 0x0700)
            {
//    		    sprintf(msgString, "%s, Tex: ",msgString);
//        	    GetTexFmtDesc(&m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat,msgString+strlen(msgString),80-strlen(msgString));
            }
            else
#ifndef UNICODE
    		    sprintf(msgString, "%s, Tex: %s",msgString,D3DFmtToString(m_fmtCommon[nTextureFormat]));
#else
    		    sprintf(msgString, "%s, Tex: %S",msgString,D3DFmtToString(m_fmtCommon[nTextureFormat]));
#endif // UNICODE
	    }
	    else
	    {
/*
		    if (m_dwVersion == 0x0600)
		    {
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

            TestFinish(D3DTESTFINISH_ABORT);
		    return false;
	    }
    }

    // Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CFogTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		if (dwFog == VERTEX)
		{
			RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);
		}
		else
		{
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,VertexList,8,NULL,0,0);
			RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList+8,dwVertexCount-8,NULL,0,0);
		}

		EndScene();
	}	
}

bool CFogTest::ProcessFrame(void)
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

bool CFogTest::TestTerminate(void)
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;
//    RELEASE(pImage);

	return true;
}

//******************************************************************************
BOOL CFogTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CFogTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CFogTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

