//
// SRT.CPP - Implementation of the SetRenderTarget test
//

#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "srt.h"

// Global Test and App instance
//CD3DWindowFramework	App;

#define SRT_GREEN   D3DRGBA(0.0, 1.0, 0.0, 1.0)
#define SRT_RED     D3DRGBA(1.0, 0.0, 0.0, 1.0)
#define SRT_BLUE    D3DRGBA(0.0, 0.0, 1.0, 1.0)
#define SRT_WHITE   D3DRGBA(1.0, 1.0, 1.0, 0.0)
#define SRT_WHITENA D3DRGBA(1.0, 1.0, 1.0, 1.0)

SRT_TLVERTEX g_SmallTris[] =
    { { 0.5f,  256.5f, 0.1f, 10.0f, SRT_RED,   0.0f, 1.0f },
      { 43.1f, 0.5f,   0.1f, 10.0f, SRT_WHITE, 0.5f, 0.0f },
      { 85.7f, 256.5f, 0.1f, 10.0f, SRT_RED,   1.0f, 1.0f }, 

      { 85.7f,  256.5f, 0.1f, 10.0f, SRT_GREEN, 0.0f, 1.0f },
      { 128.3f, 0.5f,   0.1f, 10.0f, SRT_WHITE, 0.5f, 0.0f },
      { 170.9f, 256.5f, 0.1f, 10.0f, SRT_GREEN, 1.0f, 1.0f }, 

      { 170.9f, 256.5f, 0.1f, 10.0f, SRT_BLUE,  0.0f, 1.0f },
      { 213.5f, 0.5f,   0.1f, 10.0f, SRT_WHITE, 0.5f, 0.0f },
      { 256.1f, 256.5f, 0.1f, 10.0f, SRT_BLUE,  1.0f, 1.0f } };

SRT_TLVERTEX g_BigQuad[] =
    { { 0.0f,   279.5f, 0.1f, 10.0f, SRT_WHITENA, 0.0f, 1.0f },
      { 0.0f,   0.0f,   0.1f, 10.0f, SRT_WHITENA, 0.0f, 0.0f },
      { 319.5f, 279.5f, 0.1f, 10.0f, SRT_WHITENA, 1.0f, 1.0f },
      { 319.5f, 0.0f,   0.1f, 10.0f, SRT_WHITENA, 1.0f, 0.0f } };

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

    CSRT*           pSRT;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 5 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pSRT = new CSRTTexture();
                break;
            case 1:
                pSRT = new CSRTNoZTexture();
                break;
            case 2:
                pSRT = new CSRTOffscreen();
                break;
            case 3:
                pSRT = new CSRTNoZOffscreen();
                break;
            case 4:
                pSRT = new CSRTInvalid();
                break;
        }

        if (!pSRT) {
            return FALSE;
        }

        // Initialize the scene
        if (!pSRT->Create(pDisplay)) {
            pSRT->Release();
            return FALSE;
        }

        bRet = pSRT->Exhibit(pnExitCode);

        bQuit = pSRT->AbortedExit();

        // Clean up the scene
        pSRT->Release();
    }

    return bRet;
}

CSRT::CSRT()
{
    m_RTWidth = SRT_SIZE;
    m_RTHeight = SRT_SIZE;
//    m_pImageData = NULL;
    m_pStripes = NULL;
    m_bInvalid = false;
    m_bOffscreen = false;

    m_pOriginalSrcRT = NULL;
    m_pOriginalSrcZ = NULL;
    m_pTextureRT = NULL;
    m_pTextureRTS = NULL;
    m_pSrcZ = NULL;
    m_pOffSrcSurf = NULL;
    m_bExit = FALSE;
}

CSRT::~CSRT()
{
}

void CSRT::CommandLineHelp(void)
{
//    WriteCommandLineHelp("$y-Width:$wn\t\t$wSets the texture width to n (default 256)");
//    WriteCommandLineHelp("$y-Height:$wn\t\t$wSets the texture height to n (default 256)");
}

bool CSRT::SetDefaultRenderStates(void)
{
    SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
	return true;
}
     
UINT CSRT::TestInitialize(void)
{
    D3DSURFACE_DESC d3dsd;

	// Request only RGB texture formats
//	if (!RequestTextureFormats(PF_RGB))
//    {
//		WriteToLog("No texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

    if (m_ModeOptions.fZBuffer)
        SetTestRange(1, m_uCommonTextureFormats * m_uCommonZBufferFormats);
    else
        SetTestRange(1, m_uCommonTextureFormats);

//    m_pOriginalSrcRT = m_pSrcTarget;
//    m_pOriginalRefRT = m_pRefTarget;
//    m_pOriginalSrcZ = m_pSrcZBuffer;
//    m_pOriginalRefZ = m_pRefZBuffer;

    m_pDevice->GetRenderTarget(&m_pOriginalSrcRT);
    m_pDevice->GetDepthStencilSurface(&m_pOriginalSrcZ);

//    m_nOriginalZFormat = m_pMode->nZBufferFormat;
    m_pOriginalSrcZ->GetDesc(&d3dsd);
    m_fmtdOriginal = d3dsd.Format;

//    m_pOriginalSrcRT->AddRef();
//    m_pOriginalRefRT->AddRef();

//    if (m_ModeOptions.fZBuffer)
//    {
//        m_pOriginalSrcZ->AddRef();
//        m_pOriginalRefZ->AddRef();
//    }
    
    ReadInteger("Width", SRT_SIZE, &m_RTWidth);
    ReadInteger("Height", SRT_SIZE, &m_RTHeight);

    g_SmallTris[0].y = (float)(m_RTHeight - 1);
    g_SmallTris[2].y = (float)(m_RTHeight - 1);
    g_SmallTris[3].y = (float)(m_RTHeight - 1);
    g_SmallTris[5].y = (float)(m_RTHeight - 1);
    g_SmallTris[6].y = (float)(m_RTHeight - 1);
    g_SmallTris[8].y = (float)(m_RTHeight - 1);

    g_SmallTris[1].x = (float)m_RTWidth / 6.0f;
    g_SmallTris[2].x = (float)m_RTWidth / 3.0f;
    g_SmallTris[3].x = (float)m_RTWidth / 3.0f;
    g_SmallTris[4].x = (float)m_RTWidth / 2.0f;
    g_SmallTris[5].x = (float)m_RTWidth / 1.5f;
    g_SmallTris[6].x = (float)m_RTWidth / 1.5f;
    g_SmallTris[7].x = (float)m_RTWidth / 1.2f;
    g_SmallTris[8].x = (float)m_RTWidth;

    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

    return D3DTESTINIT_RUN;
}

bool CSRT::ClearFrame(void)
{
    bool bResult;

//    m_pMode->nZBufferFormat = m_nOriginalZFormat;

	bResult =  Clear(RGB_MAKE(0,0,100));

//    if (m_ModeOptions.fZBuffer)
//        m_pMode->nZBufferFormat = m_nZFormat;

    return bResult;
}

bool CSRT::ExecuteTest(UINT uiTest)
{
    char buf[256];
    bool bZBuffer;
 	DWORD	dwPasses;
	HRESULT	hResult;
    bool bDoAttach = false;
    CSurface8 *pTempSrc = NULL;
    CSurface8 *pTempRef = NULL;

    // Cleanup previous texture and zbuffers
    SetTexture(0, NULL);
    RELEASE(m_pTextureRTS);
    ReleaseTexture(m_pTextureRT);
    m_pTextureRT = NULL;
    RELEASE(m_pSrcZ);
//    RELEASE(m_pRefZ);
    RELEASE(m_pOffSrcSurf);
//    RELEASE(m_pOffRefSurf);

    if (m_ModeOptions.fZBuffer)
    {
//        m_nTexFormat = (uiTest-1) / m_uCommonZBufferFormats;
        m_nTexFormat = (uiTest-1) % m_uCommonTargetFormats;
        m_nZFormat = (uiTest-1) % m_uCommonZBufferFormats;
        bZBuffer = true;
    }
    else
    {
//        m_nTexFormat = uiTest-1;
        m_nTexFormat = (uiTest-1) % m_uCommonTargetFormats;
        m_nZFormat = 0;
        bZBuffer = false;
    }

    // Start the log
    DescribeTest(buf, m_nTexFormat, m_nZFormat);
    BeginTestCase(buf);

    // Tell framework what formats to use
//    m_pMode->nTextureFormat = m_nTexFormat;

    if (bZBuffer)
    {
//        m_pMode->nZBufferFormat = m_nZFormat;

        // Don't create a new ZBuffer for the original
        if (m_fmtdCommon[m_nZFormat] == m_fmtdOriginal)
        {
            bZBuffer = false;
            bDoAttach = true;
            m_pSrcZ = m_pOriginalSrcZ;
//            m_pRefZ = m_pOriginalRefZ;
            m_pSrcZ->AddRef();
//            m_pRefZ->AddRef();
        }
    }

    // Attempt to create the rendertarget
    if (m_bOffscreen)
    {
        if (!CreateDestBuffer(bZBuffer, m_pSrcZ, m_pRefZ))
        {
            WriteToLog("(INFO) CreateDestBuffer failed.\n");
    	    SkipTests(1);
            return false;
        }

//        m_pTextureRT = CreateTexture(m_RTWidth, m_RTHeight, CDDS_TEXTURE_VIDEO, NULL);
        m_pTextureRT = (CTexture8*)CreateTexture(m_pDevice, m_RTWidth, m_RTHeight, m_fmtrCommon[m_nTexFormat]);

        // For Dx7, all offscreen cases need to attach zbuffer
//        if ((m_dwVersion <= 0x0700) && bZBuffer)
//            bDoAttach = true;
    }
    else
    {
//        m_pTextureRT = CreateTextureAsRenderTarget(m_RTWidth, m_RTHeight, CDDS_TEXTURE_VIDEO, bZBuffer, m_pSrcZ, m_pRefZ, m_pImageData);
        m_pTextureRT = (CTexture8*)CreateTexture(m_pDevice, m_RTWidth, m_RTHeight, m_fmtrCommon[m_nTexFormat], TTYPE_TARGET);
        if (m_bInvalid) {
            CTexture8* pd3dt;
            pd3dt = CreateStripedTexture(m_pDevice, m_RTWidth, m_RTHeight, m_RTWidth, m_pStripes, false, false, D3DFMT_A8R8G8B8);
            CopyTexture(m_pTextureRT, NULL, pd3dt, NULL, D3DX_FILTER_POINT);
            ReleaseTexture(pd3dt);
        }
        if (m_pTextureRT && bZBuffer) {
            m_pDevice->CreateDepthStencilSurface(m_RTWidth, m_RTHeight, m_fmtdCommon[m_nZFormat], D3DMULTISAMPLE_NONE, &m_pSrcZ);
            if (!m_pSrcZ) {
                ReleaseTexture(m_pTextureRT);
                m_pTextureRT = NULL;
            }
        }
    }

    if (NULL == m_pTextureRT)
    {
        WriteToLog("(INFO) CreateTexture failed. (%X)\n", GetLastError());
    	SkipTests(1);
        return false;
    }

    m_pTextureRT->GetSurfaceLevel(0, &m_pTextureRTS);
    if (!m_pTextureRTS) {
        ReleaseTexture(m_pTextureRT);
        m_pTextureRT = NULL;
    }

    // Setup temp pointers based on test
    if (m_bOffscreen)
    {
        pTempSrc = m_pOffSrcSurf;
        pTempRef = m_pOffRefSurf;
    }
    else
    {
//        pTempSrc = m_pTextureRT->m_pSrcSurface;
        pTempSrc = m_pTextureRTS;
//        pTempRef = m_pTextureRT->m_pRefSurface;
    }
/*
    // For Dx7, may need a special attach call
    if ((m_dwVersion <= 0x700) && bDoAttach)
    {
        // Attach current ZBuffer to src surface
        if (!pTempSrc->AttachSurface(m_pSrcZ))
        {
            WriteToLog("(Info) Failed Src AttachSurface(). (%s)\n", GetHResultString(GetLastError()).c_str());
            SkipTests(1);
            return false;
        }

        // Attach current ZBuffer to ref surface
        if (!pTempRef->AttachSurface(m_pRefZ))
        {
            WriteToLog("(Info) Failed Ref AttachSurface(). (%s)\n", GetHResultString(GetLastError()).c_str());
            SkipTests(1);
            return false;
        }
    }
*/
    // Set the texture src surface as the render target
//    if (!SetRenderTarget(pTempSrc, false, m_pSrcZ))
    if (FAILED(m_pDevice->SetRenderTarget(pTempSrc, m_pSrcZ)))
    {
        WriteToLog("(Info) Failed Src SetRenderTarget(). (%X)\n", GetLastError());
        SkipTests(1);
        return false;
    }
/*
    // Set the texture ref surface as the render target
    if (!SetRenderTarget(pTempRef, true, m_pRefZ))
    {
        WriteToLog("(Info) Failed Ref SetRenderTarget(). (%s)\n", GetHResultString(GetLastError()).c_str());
        SkipTests(1);
        return false;
    }
*/
    SetupViewport();

    Clear(RGB_MAKE(0,0,100));

    if (m_bInvalid)
    {
        SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
        SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
        SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
        SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
        SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);

        SetTexture(0, m_pTextureRT);
    } 
    else 
    {
        SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
        SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_DISABLE);

        SetTexture(0, NULL);
    }

    SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
    SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

#ifndef UNDER_XBOX
	if (!ValidateDevice(&hResult,&dwPasses))
    {
        // Validate failed, reset RT and skip
//        SetRenderTarget(m_pOriginalSrcRT, false, m_pOriginalSrcZ);
        m_pDevice->SetRenderTarget(m_pOriginalSrcRT, m_pOriginalSrcZ);
//        SetRenderTarget(m_pOriginalRefRT, true, m_pOriginalRefZ);
        SetupViewport();
        WriteToLog("(Info) Failed TexRT ValidateDevice. (%X)\n", hResult);
        SkipTests(1);
        return false;
    }
#endif // !UNDER_XBOX

    return true;
}

BOOL CSRT::Render() {

    HRESULT  hr;

    SwitchAPI((UINT)m_fFrame);

    SceneRefresh();

    // Begin the scene
    m_pDevice->BeginScene();

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    CompareImages();

    ProcessFrame();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    return TRUE;
}

void CSRT::SceneRefresh()
{
 	DWORD	dwPasses;
	HRESULT	hResult;

    // Draw to the new render target
    if (BeginScene())
    {
        if (!RenderPrimitive(D3DPT_TRIANGLELIST, SRT_FVFTLVERTEX, g_SmallTris, 9))
            WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

        if (!EndScene())
            WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
    }
    
    // If using offscreen surface, copy rect to texture
    if (m_bOffscreen)
    {
        if (!CopyDestBuffer())
            WriteToLog("(ERROR): CopyDestBuffer failed.\n");
    }

    // Reset to the original render target
//    if (!SetRenderTarget(m_pOriginalSrcRT, false, m_pOriginalSrcZ))
    if (FAILED(hResult = m_pDevice->SetRenderTarget(m_pOriginalSrcRT, m_pOriginalSrcZ)))
        WriteToLog("(ERROR): Couldn't reset Src RenderTarget. (%X)\n", hResult);

//    if (!SetRenderTarget(m_pOriginalRefRT, true, m_pOriginalRefZ))
//        WriteToLog("(ERROR): Couldn't reset Ref RenderTarget. (%X)\n", GetLastError());

    SetupViewport();

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);

    SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

    SetTexture(0, m_pTextureRT);

#ifndef UNDER_XBOX
    // This shouldn't ever happen but it doesn't hurt to check
	if (!ValidateDevice(&hResult,&dwPasses))
    {
        WriteToLog("(Info) Failed ValidateDevice. (%X)\n", hResult);
        SkipTests(1);
        return;
    }
#endif // !UNDER_XBOX

    // Draw to the real target
    if (BeginScene())
    {
        if (!RenderPrimitive(D3DPT_TRIANGLESTRIP, SRT_FVFTLVERTEX, g_BigQuad, 4))
            WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

        if (!EndScene())
            WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
    }
}

bool CSRT::ProcessFrame()
{
    char    szBuffer[80];
    static  int nPass = 0;
    static  int nFail = 0;
    bool    bResult = true;

    // Use the standard 15% for non Invalid cases
//    if (m_bInvalid)
//        m_fPassPercentage = 1.0;
//    else
//        bResult = GetCompareResult(0.15f, 0.78f, 0);

    // Tell the logfile how the compare went
    if (bResult)
    {
        (m_fIncrementForward) ? nPass++ : nPass--;
        sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Pass();

//        m_vSupportedFormats.push_back( std::pair<DWORD, DWORD>(m_nTexFormat, m_nZFormat) );
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

bool CSRT::TestTerminate()
{
    //  Write out list of supported formats
/*
    WriteToLog("\nSummary of supported formats\n");
    WriteToLog("----------------------------\n");
    std::vector< std::pair<DWORD, DWORD> >::iterator iPairs = m_vSupportedFormats.begin();
    while(iPairs != m_vSupportedFormats.end())
    {
        TCHAR   buf[256];
        DescribeTest(buf, iPairs->first, iPairs->second);
        WriteToLog("%s\n", buf);
        iPairs++;
    }
*/
    // Clean up memory
    SetTexture(0, NULL);
    RELEASE(m_pTextureRTS);
    ReleaseTexture(m_pTextureRT);
    m_pTextureRT = NULL;
    RELEASE(m_pSrcZ);
//    RELEASE(m_pRefZ);
    RELEASE(m_pOffSrcSurf);
//    RELEASE(m_pOffRefSurf);

    if (m_pOriginalSrcRT) {
    m_pOriginalSrcRT->Release();
//    m_pOriginalRefRT->Release();
    }

//    if (m_ModeOptions.fZBuffer)
    if (m_pOriginalSrcZ)
    {
        m_pOriginalSrcZ->Release();
//        m_pOriginalRefZ->Release();
    }

    if (m_bInvalid)
    {
        delete m_pStripes;
//        RELEASE(m_pImageData);
    }

    return true;
}

//******************************************************************************
BOOL CSRT::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CSRT::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CSRT::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

