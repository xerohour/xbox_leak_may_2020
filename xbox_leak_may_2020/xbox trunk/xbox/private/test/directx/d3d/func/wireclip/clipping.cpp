#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Clipping.h"

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

    // Check vertex blending support
#ifndef UNDER_XBOX
    return !(pd3dcaps->MaxUserClipPlanes < 4);
#else
    return TRUE;
#endif
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

    WireClip*   pClipping;
    BOOL        bRet;
    UINT        i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
#ifndef TESTTYPE_WIREFRAME
    for (i = 0; i < 6; i++) {
#else
    for (i = 0; i < 3; i++) {
#endif

        switch (i) {
            case 0:     pClipping = new WTFan;       break;
            case 1:     pClipping = new WTStrip;     break;
            case 2:     pClipping = new WTriangle;   break;
            case 3:     pClipping = new WLine;       break;
            case 4:     pClipping = new WLStrip;     break;
            case 5:     pClipping = new WPoint;      break;
        }

        if (!pClipping) {
            return FALSE;
        }

        // Initialize the scene
        if (!pClipping->Create(pDisplay)) {
            pClipping->Release();
            return FALSE;
        }

        bRet = pClipping->Exhibit(pnExitCode);

        // Clean up the scene
        pClipping->Release();
    }

    return bRet;
}

WireClip::WireClip() {
//    m_pMaterial = NULL;
//    m_pLight = NULL;
    for (int i = 0; i < 8; i++)
        m_pTextureArray[i] = NULL;

    m_dwRotations = 5;

    m_szTestName = TEXT("Clipping");
    m_szCommandKey = TEXT("Clipping");

    m_ModeOptions.fTextures = true;
    m_ModeOptions.fZBuffer = false;
    m_ModeOptions.fAntialias = true;
#ifdef TESTTYPE_UCLIP
    m_ModeOptions.uMinDXVersion = 0x0700;
#else
    m_ModeOptions.uMinDXVersion = 0x0600;
#endif
}

WireClip::~WireClip() {
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < 8; i++)
        ReleaseTexture(m_pTextureArray[i]);
}

UINT WireClip::TestInitialize(void) {

    // Test range
    SetTestRange(1,m_dwTotalTests);

#ifdef TESTTYPE_UCLIP
    // Check user clip planes support
    if (m_d3dcaps.MaxUserClipPlanes == 0)
    {
        WriteToLog("%s: Device does not support arbitrary clipping planes\n", m_szTestName);
        return D3DTESTINIT_SKIPALL;
    }
    if (m_d3dcaps.MaxUserClipPlanes < 4)
    {
        WriteToLog("%s: The test requires at least 6 arbitrary clipping planes\n", m_szTestName);
        return D3DTESTINIT_SKIPALL;
    }
#endif

    // Query maximum number of textures
    m_dwMaxTextures = m_d3dcaps.MaxSimultaneousTextures;
    if (m_d3dcaps.MaxSimultaneousTextures < m_dwMaxTextures)
        m_dwMaxTextures = m_d3dcaps.MaxSimultaneousTextures;
    
    // Load textures
    for (int i = 0; i < (int)m_dwMaxTextures; i++) {
        TCHAR fbuf[16];
        wsprintf(fbuf, TEXT("tclptex%1d.bmp"), i);

        m_pTextureArray[i] = (CTexture8*)CreateTexture(m_pDevice, fbuf, D3DFMT_R5G6B5,
                                TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE,
                                POOL_DEFAULT, 256,256);
        if (m_pTextureArray[i] == NULL)
        {
            WriteToLog("%s: Can't create texture from image\n", m_szTestName);
            return D3DTESTINIT_ABORT;
        }
    }

    return D3DTESTINIT_RUN;
}

bool WireClip::ClearFrame(void)
{
    return Clear(0);
//    return true;
}

bool WireClip::ExecuteTest(UINT uTestNumber) {
    m_dwCurrentTestNumber = uTestNumber;

    if (!ResolveTestNumber())
        return false;

    BeginTestCase(m_szTestName);

    if (!SetWorldMatrix())          goto error;
    if (!SetProjectionMatrix())     goto error;
    if (!SetVertices())             goto error;
    if (!SetProperties())           goto error;
    if (!SetUserClipPlanes())       goto error;

    return true;

error:
    Fail();
    EndTestCase();
    return false;

}


void WireClip::SceneRefresh(void) {
	RECT				rect;

	// Clear the viewport size to the blue color
	rect.left = m_pDisplay->GetWidth()/4;
	rect.top = m_pDisplay->GetHeight()/4;
	rect.right = m_pDisplay->GetWidth()*3/4;
	rect.bottom = m_pDisplay->GetHeight()*3/4;
	if (m_dwCurrentZClip) {
		rect.top += 12;
		rect.bottom -= 11;
	}
	ColorFillTarget(RGB_MAKE(0,0,0));
	ColorFillTarget(RGB_MAKE(0,0,128), &rect);

    if (BeginScene()) {
		RenderPrimitive(m_dwCurrentPrimitiveType,m_dwCurrentFVF,m_pRenderVertices,
                        m_dwVertices,NULL,0,CD3D_BOTH);
        EndScene();
    }

   	HRESULT hr = GetLastError();
	if (hr != D3D_OK)
	{
		WriteToLog("%s: SceneRefresh() returned an error on test %d\n", 
                   m_szTestName, m_dwCurrentTestNumber);
	}
	WriteStatus("Test type", m_pszCurrentProperties);
	return;
}




bool WireClip::ProcessFrame(void) { 

//    if (m_pRefTarget != NULL)
//    {
        RECT    rect;
        DWORD	dwUpperBlock = m_pDisplay->GetHeight()/4;
        DWORD	dwBottomBlock = m_pDisplay->GetHeight()/4;

        if (m_dwCurrentZClip) {
            dwUpperBlock += 12;
            dwBottomBlock += 11;
        }

        // Upper block
        rect.left = 0;
        rect.right = m_pDisplay->GetWidth();
        rect.top = 0;
        rect.bottom = dwUpperBlock;
        ColorFillTarget(RGB_MAKE(0,0,0), &rect);
        // Bottom block
        rect.left = 0;
        rect.right = m_pDisplay->GetWidth();
        rect.top = m_pDisplay->GetHeight() - dwBottomBlock;
        rect.bottom = m_pDisplay->GetHeight();
        ColorFillTarget(RGB_MAKE(0,0,0), &rect);
        // Left block
        rect.left = 0;
        rect.right = m_pDisplay->GetWidth()/4;
        rect.top = dwUpperBlock;
        rect.bottom = m_pDisplay->GetHeight() - dwBottomBlock;
        ColorFillTarget(RGB_MAKE(0,0,0), &rect);
        // Right block
        rect.left = m_pDisplay->GetWidth()*3/4;
        rect.right = m_pDisplay->GetWidth();
        rect.top = dwUpperBlock;
        rect.bottom = m_pDisplay->GetHeight() - dwBottomBlock;
        ColorFillTarget(RGB_MAKE(0,0,0), &rect);
//    }

    if (!CompareImages()) {
        WriteToLog("%s: Can't compare images\n", m_szTestName);
        Fail();
    }
    else 
    {
        bool	bResult;
        bResult = GetCompareResult(0.05f, 0.78f, 0);
        if (!bResult)
        {
            WriteToLog("%s: Bad compare results on test %d\n", m_szTestName, m_dwCurrentTestNumber);
            Fail();
        }
        Pass();
    }
	
    EndTestCase();
    return true;
}


bool WireClip::TestTerminate(void) 
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < 8; i++) {
        ReleaseTexture(m_pTextureArray[i]);
        m_pTextureArray[i] = NULL;
    }

    return true;
}
